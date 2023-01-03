/*
 Copyright (c) 1986, 1993
 The Regents of the University of California.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement:
 This product includes software developed by the University of
 California, Berkeley and its contributors.
 4. Neither the name of the University nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.

    Modifications to make this build & run on Linux by John Winans, 2021
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

constexpr char data[] = " Copyright (c) 1986, 1993\n"
    " The Regents of the University of California.  All rights reserved.\n"
    "\n"
    " Redistribution and use in source and binary forms, with or without\n"
    " modification, are permitted provided that the following conditions\n"
    " are met:\n"
    " 1. Redistributions of source code must retain the above copyright\n"
    "    notice, this list of conditions and the following disclaimer.\n"
    " 2. Redistributions in binary form must reproduce the above copyright\n"
    "    notice, this list of conditions and the following disclaimer in the\n"
    "    documentation and/or other materials provided with the distribution.\n"
    " 3. All advertising materials mentioning features or use of this software\n"
    "    must display the following acknowledgement:\n"
    " This product includes software developed by the University of\n"
    " California, Berkeley and its contributors.\n"
    " 4. Neither the name of the University nor the names of its contributors\n"
    "    may be used to endorse or promote products derived from this software\n"
    "    without specific prior written permission.\n"
    "\n"
    " THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND\n"
    " ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
    " IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n"
    " ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE\n"
    " FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"
    " DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS\n"
    " OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n"
    " HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n"
    " LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n"
    " OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF\n"
    " SUCH DAMAGE.\n";



/**
* Same as write() but includes a loop to complete any partials.
**************************************************************************/
static ssize_t safe_write(int fd, const char *buf, size_t len)
{
	while(len>0)
	{
		ssize_t wlen = write(fd, buf, len);
		if (wlen == -1)
			return -1;		// write returned an unrecoverable error, errno wil be set
std::cout << "sent " << wlen << " bytes" << std::endl;
		len -= wlen;		// reduce the remaining number of bytes to send
		buf += wlen;		// advance the buffer pointer past the written data
	}
	return len;				// if we get here then we sent the full requested length!
}

/**
* Read data from the given socket fd & print it until we reach EOF
**************************************************************************/
static int print_response(int fd)
{
	char buf[1024];
	int rval = 1;	// prime the while loop 
	while(rval>0)
	{
		if ((rval = read(fd, buf, sizeof(buf)-1)) == -1)
		{
			perror("reading stream message");
			return -1;						// let the caller know badness happened
		}
		else if (rval > 0)
		{
				buf[rval] = '\0';
			std::cout << "---- read " << rval << " bytes --->>>" << buf << "<<<" << std::endl;
		}
	}
	return 0;	// all went OK
}




/**
**************************************************************************/
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in server;

	assert(argc==3 && "Missing ip number & port!");

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}

	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;		// IP version 4

	if(inet_pton(AF_INET, argv[1], &server.sin_addr)<=0) 
	{
		fprintf(stderr, "%s: invalid address/format\n", argv[1]);
		exit(2);
	}

	server.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}
	if (safe_write(sock, data, sizeof(data)) < 0)
		perror("writing on stream socket");

	// Tell the server that no more data will be sent.
	// this will cause any subsequent read() calls to return EOF on the server side!
	shutdown(sock, SHUT_WR);	

	print_response(sock);

	close(sock);
}
