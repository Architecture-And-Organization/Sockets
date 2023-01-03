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
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include <arpa/inet.h>	// for inet_ntop()

#include <iostream>

int main()
{
	int sock;
	socklen_t length;
	struct sockaddr_in server;
	int msgsock;
	char buf[1024];
	int rval;

     /* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
     if (sock < 0) {
	  perror("opening stream socket");
	  exit(1);
     }
     /* Name socket using wildcards */
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;

     //server.sin_port = 0;
     server.sin_port = htons(9965);

     if (bind(sock, (sockaddr*)&server, sizeof(server))) {
	  perror("binding stream socket");
	  exit(1);
     }

	/* Find out assigned port number and print it out */
	length = sizeof(server);
	if (getsockname(sock, (sockaddr*)&server, &length)) {
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(server.sin_port));

	/* Start accepting connections */
	listen(sock, 5);
	do {
		struct sockaddr_in	from;
		socklen_t from_len = sizeof(from);
		msgsock = accept(sock, (struct sockaddr*)&from, &from_len);

		if (msgsock == -1)
		{
			perror("accept");
		}
		else 
		{
			inet_ntop(from.sin_family, &from.sin_addr, buf, sizeof(buf));
			printf("Accepted connection from '%s', port %d\n", buf, ntohs(from.sin_port));

			do {
				//bzero(buf, sizeof(buf));
				//if ((rval = read(msgsock, buf, 1024)) < 0)
				if ((rval = read(msgsock, buf, sizeof(buf)-1)) < 0)
					perror("reading stream message");
				if (rval == 0)
					printf("Ending connection\n");
				else
				{
					buf[rval] = '\0';
					std::cout << "---- read " << rval << " bytes --->>>" << buf << "<<<" << std::endl;
					//printf("-->%s\n", buf);
				}
			} while (rval != 0);

	  		close(msgsock);
		}
	} while (true);

     /*
      * Since this program has an infinite loop, the socket "sock" is
      * never explicitly closed.  However, all sockets will be closed
      * automatically when a process is killed or terminates normally.
      */
}
