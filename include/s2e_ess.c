
#include "s2e_ess.h"



int test() {

	printf("\n Hello header");
}


int net_open(struct s2e_t *s2e) {

	int fd = -1, cfd = -1;
	int ret;
	int type = (s2e->net_proto == NET_PROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM; //For now only implementing TCP
	int slen;
	int len = 1;
	struct sockaddr_in saddr;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(s2e->net_port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	slen = sizeof(saddr);



	if ((fd = socket(AF_INET, type, 0)) < 0 ) {
		perror("\n net_open: Socket creation failed");
		return -1;
	}
	//Implemeted only TCP SERVER Mode 
	if (s2e->net_proto == NET_PROTO_TCP && s2e->net_mode == NET_MODE_SERVER) {



		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(char *)&len, sizeof(len)) < 0 ) {
			perror("\n net_open:Resueseaddr failed");
			close(fd);
			return -1;

		}

		//Binding the IP Address
		if (bind(fd, (struct sockaddr *)&saddr, slen) , 0) {
			perror("\n net_open: Bind failed");
			close(fd);
			fd = -1;
			return -1;
		}

		//listenin got the address
		if (listen(fd, 1) < 0) {
			perror("\n net-_pen: Listen Failed");
			close(fd);
			fd = -1;
			return -1;
		}
		fflush(stdout);
		memset(&saddr, 0, sizeof(saddr));
		if ((cfd = accept(fd, (struct sockaddr *)&saddr, &slen)) < 0) {
			perror("\n net_open: Accept failed");
			close(fd);
			fd = -1;
			return -1;
		}

		printf("\n Connection accepted");
		close(fd);
		fd = -1;
		s2e->sock_fd = cfd;

	}
	else {
		printf("\n Not yet implemeted");
	}

	return 0;
}