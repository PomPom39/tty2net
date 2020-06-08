#ifndef _S2E_ESS_H_
#define _S2E_ESS_H_


#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>



#define NET_MODE_SERVER 1
#define NET_MODE_CLIENT 2


#define NET_PROTO_UDP 0
#define NET_PROTO_TCP 1



struct s2e_t {

	char tty_device[16];
	int net_mode;
	int net_proto;
	int net_port;

	int tty_fd;
	int sock_fd;


	char *tty_buffer;
	int tty_buffsz;
	char *net_buffer;
	int net_buffsz;

	int tty_rcvd;
	int tty_sent;
	int net_rcvd;
	int net_sent;

};




int net_open(struct s2e_t *s2e);
int test();


#endif
