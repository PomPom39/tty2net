/*
 * s2e_ess.h
 *
 *  Created on: 09-Jun-2020
 *      Author: root
 */

#ifndef S2E_ESS_H_
#define S2E_ESS_H_

#undef DEBUG


#ifdef DEBUG
#define printdbg(x) printf x
#else
#define printdbg(x) asm("nop");
#endif





#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

#define NET_MODE_SERVER 1
#define NET_MODE_CLIENT 2
#define NET_PROTO_UDP 0
#define NET_PROTO_TCP 1


struct s2e_conf {
	char tty_device[16];			/* Device Name*/
	long tty_baudrate;				/* Baudrate*/
	int tty_isBlockingMode;
	int tty_parity;					/* parity <0:none, 1:odd ,2:even> */
	int tty_stopbit;				/* stop bit <1:1 stop bit, 2:2 stop bits> */
	int tty_length;					/* Bit length < 5:5 bits, 6:6 bits, 7:7 bits, 8:8 bits > */
	int tty_flow;					/* Flow Control  < 0: No Flow, 1: Software , 2: Hardware > */
	int tty_tsize;					/* Trigger size if the buffer in tty becomes full */
	char tty_delim_code[3];			/* Delimiter code for transmission trigger */
	char tty_delim_len;				/* Length of delimiter code */
	int net_mode;
	int net_proto;
	int net_port;
	int tty_fd;
	int sock_fd;
	char *tty_buffer;
	int tty_buffsz;
	char *net_buffer;
	int net_buffsz;

};



int net_open(struct s2e_conf *conf);
int net_close(struct s2e_conf *conf);
int net_read(struct s2e_conf *conf, char *buffer, int size);
int net_write(struct s2e_conf *conf, char *buffer, int size);
int tty_open(struct s2e_conf *conf);
int tty_close(struct s2e_conf *conf);
int tty_write(struct s2e_conf *conf, char *buffer, int size);
int tty_read(struct s2e_conf *conf, char *buffer, int size);



#endif /* S2E_ESS_H_ */
