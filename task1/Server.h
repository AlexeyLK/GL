#include "Net.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>

#include <cstring>
using namespace std;
using namespace net;

int main(int argc, char* argv[])
{
	// initialize socket layer

	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	// create socket

	int port = 5555;

	printf("creating socket on port %d\n", port);

	Socket socket;

	if (!socket.Open(port))
	{
		printf("failed to create socket!\n");
		return 1;
	}

	// send and receive packets to ourself until the user ctrl-breaks...
	std::cout << "enter ip address(example \n127\n0\n0\n1\n)\n";
	unsigned int ipa;
	unsigned int ipb;
	unsigned int ipc;
	unsigned int ipd;
	std::cin >> ipa;
	std::cin >> ipb;
	std::cin >> ipc;
	std::cin >> ipd;
	Address server1(ipa, ipb, ipc, ipd, port + 1);
	//Address server2(ipa,ipb,ipc,ipd,port+1);
	while (true)
	{
		string filename;
		std::cout << "Enter FileName\n";
		std::cin >> filename;
		socket.Send(server1, filename.c_str(), sizeof(filename));
		this_thread::sleep_for(chrono::seconds(1));
		socket.Send(server1, "", 1);
		while (true)
		{
			Address sender;
			char buffer[4096];
			int bytes_read = socket.Receive(sender, buffer, sizeof(buffer));
			if (!bytes_read)
				break;
			if (bytes_read == 1) {
				cout << "File Not Exists" << endl;
			}
			else {
				ofstream file(filename, ios::binary);
				char* lastdata = (char*)"";
				string filedata;
				int filesize = atoi(buffer);
				bool passed = false;
				//file.write(buffer, 4096);
				for (int i = 1;; i++) {
					memset(buffer, NULL, sizeof(buffer));
					bytes_read = socket.Receive(sender, buffer, sizeof(buffer));
					if (buffer[0] == 'N' && buffer[1] == 'O' && buffer[2] == 'P' && bytes_read < 6)
						break;



					/*if (!strcmp(lastdata,buffer)) {
						continue;
					}*/
					filedata = filedata.substr(0, filedata.length() - 3);
					filedata += string(buffer);


					if (filesize - file.tellp() > 4096) {
						file.write(buffer, sizeof(buffer));
					}
					else {
						file.write(buffer, filesize - file.tellp());
					}
					printf("bytes write: %d, part:%d, pos: %ld \n", filedata.length(), i, file.tellp());
					lastdata = buffer;
					socket.Send(sender, to_string(i).c_str(), 4096);
					this_thread::sleep_for(chrono::milliseconds(10));
				}
				//file << filedata;
			}



		}

		wait(0.25f);
	}

	// shutdown socket layer

	ShutdownSockets();

	return 0;
}
