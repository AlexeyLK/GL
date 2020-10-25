Alexey Panchenko
________________

Description:
Client/Server application to see user screens on remote host.
- Client has to make &quot;print-screen&quot; each 60 seconds.
- All screens must be stored in client app folder.
- Each screen name consists of date, when it has been taken + number of screen.
Ex: 21.07.2018_15.png 21.07.2018_16.png ...etc
- Server can request screens by those names.
- File must be transferred to server via 5555 port.
_______________________________________________________________________________

Detailed instructions for building:

1) `git clone https://github.com/AlexeyLK/GL.git`

2) If you are using linux machine use command: `make linux` \
   If you are using windows machine use command: `cmake.exe --build ./bin --target all`
   
   



