#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "s2e_ess.h"
#include "s2e_ess.c"

#define TTY_DEVICE "/dev/tnt0"			//using a virtual tty device for testing


int main() {
	int ret;
	struct s2e_t s2e_conf;

	s2e_conf.net_port = 12346;
	s2e_conf.net_proto = NET_PROTO_TCP;
	s2e_conf.net_mode = NET_MODE_SERVER;




	if ((ret = net_open(&s2e_conf)) < 0) {
		printf("\n net_open failed");
		return -1;
	}

	
	return 0;
}