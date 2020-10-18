#include "Net.h"
#include "screenshoter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>



using namespace std;
using namespace net;

int main(int argc, char* argv[])
{
	// initialize socket layer
	thread screenshott(screenshoter);
	screenshott.detach();
	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	// create socket

	int port = 5556;

	printf("creating socket on port %d\n", port);

	Socket socket;

	if (!socket.Open(port))
	{
		printf("failed to create socket!\n");
		return 1;
	}

	// send and receive packets to ourself until the user ctrl-breaks...
	string lastinfo = "";
	while (true)
	{
		string filename;
		unsigned char filenamechar[256];
		Address sender;
		socket.Receive(sender, filenamechar, sizeof(filenamechar));
		filename = std::string((const char*)filenamechar);
		if (filename == lastinfo)
			continue;
		if (filename.length() < 2)
			continue;

		std::cout << "Send " + filename + "\n";
		ifstream file(filename, ios::binary | ios::ate);
		string sendstring = "";
		if (!file.is_open()) {
			sendstring = "1";
			socket.Send(sender, sendstring.c_str(), 1);
		}

		else {
			int filesize = file.tellg();
			file = ifstream(filename, ios::binary);
			string fileinfo = "";
			char* lastanswer = (char*)filename.c_str();
			socket.Send(sender, to_string(filesize).c_str(), 4096);
			for (int i = 1; !file.eof(); i++) {
				fileinfo = "";
				char temp[4096];
				memset(temp, NULL, sizeof(temp));

				file.read(temp, 4096);



				fileinfo = string();
				fileinfo = temp;
				int read = file.tellg();

				printf("bytes read: %d, part:%d, pos: %ld \n", fileinfo.length(), i, read);

				socket.Send(sender, temp, 4096);
				cout << i << " " << read << " " << i * 4096 << "\n ";
				if (read < 1)
					cout << temp << "\n";
				char answerbuf[4096];
				while (atoi(answerbuf) != i) {
					socket.Receive(sender, answerbuf, 4096);
				}
				lastanswer = answerbuf;
			}
			socket.Send(sender, "NOP", 3);
		}
		file.close();
		lastinfo = filename;
		filename = "";

		/*std::cin >> data;


				while ( true )
				{

					unsigned char buffer[256];
					int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
					if ( !bytes_read )
						break;
					printf( "received packet from %d.%d.%d.%d:%d (%d bytes)\n",
						sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(),
						sender.GetPort(), bytes_read );
						std::cout << buffer <<endl;
				}

				wait( 0.25f );*/
	}

	// shutdown socket layer

	ShutdownSockets();

	return 0;
}
