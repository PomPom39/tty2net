/*
 * s2e_ess.c
 *
 *  Created on: 09-Jun-2020
 *      Author: root
 */


/*
 * s2e_ess.c

 *
 *  Created on: 08-Jun-2020
 *      Author: root
 */
#include "s2e_ess.h"

int net_open(struct s2e_conf *conf) {

	int fd = -1;
	int fd2 = 1;
	int ret;
	int type = (conf->net_proto == NET_PROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	socklen_t sock_len;
	int len=1;
	struct sockaddr_in saddr;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(conf->net_port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sock_len = sizeof(struct sockaddr_in);

	if ((fd = socket(AF_INET, type, 0)) < 0 ){
		perror("net_open: Socket failed");
		close(fd);
		return -1;
	}
	printf("\n net_open: Socket openened succesfully");

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&len, sizeof(len)) < 0) {
		perror("\n net_open: Reuse failed");
		close(fd);
		return -1;
	}
	printf("\n net_open: Socket reuse successfull");

	if((bind(fd, (struct sockaddr *)&saddr, sock_len)) < 0) {
		perror("\n net_open: Bind Error");
		close(fd);
		return -1;
	}
	printf("\n net_open: Socket Bind sucesfull");

	if (listen(fd ,1) < 0) {
		perror("\n net_open: Listen failed");
		close(fd);
		return -1;
	}
	printf("\n net_open: socket listen successfull");
	if ((fd2 = accept(fd, (struct sockaddr *)&saddr, &sock_len)) < 0) {
		perror("\n net_open: Accept Error");
		close(fd);
		return -1;
	}
	printf("\n net_open: Client accept succesfull");
	fflush(stdout);
	close(fd);
	conf->sock_fd = fd2;

	return fd2;
}

int net_close(struct s2e_conf *conf) {
	if (conf->sock_fd <= 0)
		close(conf->sock_fd);
		conf->sock_fd = -1;
		return 0;

}

int net_read(struct s2e_conf *conf, char *buffer, int size) {
	int sock;
	int ret;
	sock = conf->sock_fd;
	if (conf->net_mode == NET_PROTO_TCP) {
		ret = recv(sock, buffer, size, 0);
		return ret;
	}
	else {
		printf("\n [UDP] Not implemented");
		return 0;
	}

}

int net_write(struct s2e_conf *conf, char *buffer, int size) {
	int sock;
	int ret;

	sock = conf->sock_fd;
	if (conf->net_mode == NET_PROTO_TCP) {
		ret = send(sock, buffer, size, 0);
		return ret;
	}
	else {
		printf("\n [UDP] Not implemented");
		return 0;
	}
}

int tty_open(struct s2e_conf *conf) {
	int fd;
	if ((fd = open(conf->tty_device, O_RDWR)) < 0) {
		perror("\n tty_open: open failed");
		close(fd);
		conf->tty_fd = -1;
		return -1;

	}
	printf("\n tty_open: open succesfull");
	conf->tty_fd = fd;
	return 0;
}

int tty_write(struct s2e_conf *conf, char *buffer, int size) {
	int ret;
	int fd;
	fd = conf->tty_fd;
	if ((ret = write(fd, buffer, size)) < 0) {
		printf("\n tty_write: write failed");
		return -1;
	}
	printf("\n tty_write: write succesfull");
	return ret;
}

tty_read(struct s2e_conf *conf, char *buffer, int size) {
	int ret;
		int fd;
		fd = conf->tty_fd;
		if ((ret = read(fd, buffer, size)) < 0) {
			printf("\n tty_read: read failed");
			return -1;
		}
		printf("\n tty_write: read succesfull");
		return ret;
}

int tty_close(struct s2e_conf *conf) {
	if (conf->tty_fd > 0) {
		close(conf->tty_fd);
	}
	conf->tty_fd = -1;
	printf("\n tty_close: tty close");
	return 0;
}
