# AF_INET/SOCK_STREAM example programs based on the BSD IPC Tutorial

These example programs are based on those appearing in 
[An Introductory 4.4BSD Interprocess Communication Tutorial](https://docs.freebsd.org/44doc/psd/20.ipctut/paper.pdf)

- `client.cc` Minimal port from IPC tutorial to run on Linux 
- `server.cc` Minimal port from IPC tutorial to run on Linux 

- `client2.cc`	
	- Change gethostbyname() to inet_pton()
- `server2.cc`	
	- Add inet_ntop() print the IP and port of the connection's peer after the accept()

- `client3.cc`
	- Add a proper write() loop to complete partial writes
	- Change macro into constexpr and made the string to send MUCH longer
		- Why does the server get 2 chunks of data rather than one?
		- Why does the server print junk after the first block?
			- because the bzero only works if the full 1024 bytes are not received!

- `server3.cc`
	- Properly append a null char to end of buffer after read completes if/when printing by changing the read size with the bzero and then removing the bzero & replace with a single null before printing.

- `client4.cc`
	- Write data and then read a reply
		- what happens if no reply is ever received? 
- `server4.cc`
	- Read until EOF and then send a reply message
		- This will deadlock if use client4 without a shutdown!
		- This will SIGPIPE if use client3 because it will close the socket too soon!

- `client5.cc`
	- Do a shutdown after the write() has completed

- `server5.cc`
	- ignore SIGPIPE
