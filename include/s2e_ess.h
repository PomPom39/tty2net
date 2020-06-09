/*
 * s2e_ess.h
 *
 *  Created on: 09-Jun-2020
 *      Author: root
 */

#ifndef S2E_ESS_H_
#define S2E_ESS_H_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#define NET_MODE_SERVER 1
#define NET_MODE_CLIENT 2
#define NET_PROTO_UDP 0
#define NET_PROTO_TCP 1


struct s2e_conf {
	char tty_device[16];
	long tty_baudrate;
	int tty_parity;
	int tty_stopbit;
	int net_mode;
	int net_proto;
	int net_port;
	int tty_fd;
	int sock_fd;
	char tty_buffer[256];
	int tty_buffsz;
	char net_buffer[256];
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
