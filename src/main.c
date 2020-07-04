/*
 * Copyright (C) Copyright.
 *
 * Author: Mahesh R Devadiga
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the PG_ORGANIZATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY	THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS-IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "s2e_ess.h"
#include "s2e_ess.c"



//#define BUFFSIZE 256
#define BUFFSIZE 256
#define TTY_DEVICE "/dev/tnt0"
void int_sigHandler();




struct s2e_conf s2e;

fd_set READSET, WRITESET;
char netBuff[BUFFSIZE];
char ttyBuff[BUFFSIZE];

struct timeval timeout;


int main(void) {
        int ret;
        int tty_fd;
        int sock_fd;
        int max_fd;
        int tty_write_size = 0;
        int net_write_size = 0;
        int net_write_pos = 0;
        int net_write_force = 0;
        int tty_write_pos = 0;
        int tty_read_pos = 0;
        int tty_delim_pos = 0;
        char tty_delim_len = 0;
        int net_tcp_conn = 1;

        timeout.tv_sec = 10;			/*Giving a ten second timeout just in case */
        timeout.tv_usec = 0;


        s2e.sock_fd = -1;
        s2e.tty_fd = -1;

        s2e.net_port = 60001;
        //s2e.net_mode = NET_MODE_SERVER;
        s2e.net_mode = NET_MODE_CLIENT;
        //s2e.net_proto = NET_PROTO_TCP;
        s2e.net_proto = NET_PROTO_UDP;



        s2e.tty_baudrate = 115200;
        strcpy(s2e.tty_device, TTY_DEVICE);
        strcpy(s2e.net_remote_ip, "127.0.0.1");


        s2e.net_buffsz = BUFFSIZE;
        s2e.tty_buffsz = BUFFSIZE;
        s2e.tty_tsize = s2e.net_buffsz;
        s2e.tty_buffer = ttyBuff;
        s2e.net_buffer = netBuff;
        s2e.tty_delim_code[0] = (int)'\n';
        s2e.tty_delim_len = 0;

    	net_write_pos = 0;
    	net_write_size = s2e.tty_tsize;


    	signal(SIGINT, int_sigHandler);

        while(1) {

        	if (s2e.sock_fd < 0 && net_tcp_conn == 1) {
        	    		//fprint(lp, "\n opening socket (closed)");
        	    		if ((ret = net_open(&s2e)) < 0) {
        	    			printf("\n Socket open failed. Retrying...");
        	    			sleep(1);
        	    			continue;
        	    		}
        	    		net_tcp_conn = 1; //Changed to 1 for UDP
        	    	}


        	if (s2e.tty_fd < 0) {
        		if((ret = tty_open(&s2e)) < 0) {
        			printf("\n tty open failed. Retrying...");
        			sleep(1);
        			continue;
        		}
        	}

        	tty_fd = s2e.tty_fd;
        	sock_fd = s2e.sock_fd;


			FD_ZERO(&READSET);
			FD_ZERO(&WRITESET);

			if (tty_write_size == 0)
				FD_SET(sock_fd, &READSET);
			else
				FD_SET(tty_fd, &WRITESET);

			if (net_write_pos < net_write_size)
				FD_SET(tty_fd, &READSET);
			if ((net_write_size <= net_write_pos) || net_write_force)
				FD_SET(sock_fd, &WRITESET);

			//printf("\n Waiting for connection");
			//fflush(stdout);
			max_fd = (s2e.tty_fd > s2e.sock_fd) ? tty_fd : sock_fd;

			ret = select(max_fd + 1, &READSET, &WRITESET, NULL, &timeout/*NULL*/);

			if (ret < 0) {
				perror("\n main: select error");
				net_close(&s2e);
				tty_close(&s2e);
				continue;
			}
			else if(ret == 0) {
				perror("\n main: timed out ");
				timeout.tv_sec = 10; //resetting the timer
				if (net_write_pos != 0)
					net_write_force = 1;
				if (s2e.net_proto == NET_PROTO_UDP)
					net_close(&s2e);
				continue;
			}


			if (tty_write_size) {
				//Write to tty
				if (FD_ISSET(s2e.tty_fd, &WRITESET)) {
					if ((ret = tty_write(&s2e, &s2e.net_buffer[tty_write_pos], tty_write_size - tty_write_pos)) < 0) {
						tty_close(&s2e);
						continue;
					}
					tty_write_pos +=ret;
					if(tty_write_pos >= tty_write_size)
						tty_write_size = 0;
				}
			}
			else {
				//Read from network
				if(FD_ISSET(s2e.sock_fd, &READSET)) {
					if ((ret = net_read(&s2e, s2e.net_buffer, s2e.net_buffsz )) < 0) {
						net_close(&s2e);
						continue;
					}

					tty_write_size = ret;
					//printf("\n [NET]read bytes = %d", tty_write_size);
					tty_write_pos = 0;
				}
			}


			if(net_write_size <= net_write_pos|| net_write_force) {
				//Write to network
				if (FD_ISSET(s2e.sock_fd, &WRITESET)) {

					if (tty_delim_pos && tty_delim_pos < net_write_pos) {
						if((ret = net_write(&s2e, s2e.tty_buffer, tty_delim_pos)) < 0) {
							perror("\n linuxmain: send error");
							continue;
						}
						//printf("\n sent %d bytes", ret);
						/* Moving the rest of data from ttyBuff to the start position*/

						memmove(ttyBuff, &s2e.tty_buffer[tty_delim_pos], net_write_pos - tty_delim_pos);

						net_write_pos -= tty_delim_pos;
						tty_delim_pos = 0;
						net_write_force = 0;
					}
					else {
						if((ret = net_write(&s2e, s2e.tty_buffer, net_write_pos)) < 0) {
							perror("\n linuxmain: send error");
							continue;
						}
						net_write_pos = 0;
						tty_delim_pos = 0;
						net_write_force = 0;
					}

				}
			}

			if (net_write_pos < s2e.net_buffsz) {
				//Read from tty
				if (FD_ISSET(s2e.tty_fd, &READSET)) {
					if ((ret = tty_read(&s2e, &s2e.tty_buffer[net_write_pos], net_write_size - net_write_pos)) < 0) {
						tty_close(&s2e);
						continue;
					}
					net_write_pos += ret;
				//	printf("[TTY] read byte s= %d", ret);
				}

			}

			/* Check delimiter in ttyBuff */
			if (s2e.tty_delim_len && tty_delim_pos < net_write_pos) {
				for (int i = tty_delim_pos; i < net_write_pos; i++) {
					if(s2e.tty_delim_code[tty_delim_len++] == s2e.tty_buffer[i]) {
						//printf("\n Delim found at %d, delim code is %d @%d", ttyBuff[i], s2e.tty_delim_code[tty_delim_len - 1], i);
					//	fflush(stdout);
						//exit(1);
						net_write_force = 1;
						tty_delim_len = 0;
						break;
					}
					else
						tty_delim_len = 0;
				}
			}



				timeout.tv_sec = 5; //resetting timer

        }

        printf("\n Exit");
        return 0;
}

void int_sigHandler() {
	printf("\n Interrupt issued. Closing all processes");
	net_close(&s2e);
	tty_close(&s2e);
	printf("\n Exiting ....");
	exit(1);

}
