/*
 * s2e_essentials.c
 *
 *  Created on: 25-Jun-2020
 *      Author: root
 */


/*
 * s2e_ess.c

 *
 *  Created on: 08-Jun-2020
 *      Author:Mahesh R Devadiga
 */


/* Revision 1.5  @25-06-2020 16:14*/

#include "s2e_ess.h"

int net_open(struct s2e_conf *conf) {

	int fd = -1;
	int fd2 = 1;
	int type = (conf->net_proto == NET_PROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	socklen_t sock_len;
	int len=1;
	struct sockaddr_in saddr;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(conf->net_port);
	//saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sock_len = sizeof(struct sockaddr_in);

	if (conf->net_proto == NET_PROTO_TCP) {
		if (conf->net_mode == NET_MODE_SERVER) {
			saddr.sin_addr.s_addr = htonl(INADDR_ANY);
			if ((fd = socket(AF_INET, type, 0)) < 0 ){
				perror("net_open: Socket failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket openened succesfully"));

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&len, sizeof(len)) < 0) {
				perror("\n net_open: Reuse failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket reuse successfull"));

			if((bind(fd, (struct sockaddr *)&saddr, sock_len)) < 0) {
				perror("\n net_open: Bind Error");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket Bind sucesfull"));


			if (listen(fd ,1) < 0) {
				perror("\n net_open: Listen failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: socket listen successfull"));
			if ((fd2 = accept(fd, (struct sockaddr *)&saddr, &sock_len)) < 0) {
				perror("\n net_open: Accept Error");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Client accept succesfull"));
			fflush(stdout);
			close(fd);
			conf->sock_fd = fd2;
			return fd2;
		}
		else {

			printdbg(("\n Remote IP:%s", conf->net_remote_ip));
			saddr.sin_addr.s_addr = inet_addr(conf->net_remote_ip);
			if ((fd = socket(AF_INET, type, 0)) < 0 ){
				perror("\n net_open: Socket failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket openened succesfully"));

			if ((fd2 = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr))) < 0) {
				perror("net_open: Connect Failed!");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Connect succesfull"));

			conf->sock_fd = fd;
			return fd;
		}
	}
	else {
		if (conf->net_mode == NET_MODE_SERVER) {
			saddr.sin_addr.s_addr = htonl(INADDR_ANY);
			if ((fd = socket(AF_INET, type, 0)) < 0 ){
				perror("net_open: Socket failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket openened succesfully"));

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&len, sizeof(len)) < 0) {
				perror("\n net_open: Reuse failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket reuse successfull"));

			if((bind(fd, (struct sockaddr *)&saddr, sock_len)) < 0) {
				perror("\n net_open: Bind Error");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket Bind sucesfull"));
			conf->sock_fd = fd;
			return fd;
		}
		else {
			//saddr.sin_addr.s_addr = inet_addr(conf->net_remote_ip);
			conf->cliaddr.sin_family = AF_INET;
			conf->cliaddr.sin_addr.s_addr = inet_addr(conf->net_remote_ip);
			conf->cliaddr.sin_port = htons(conf->net_port);
			if ((fd = socket(AF_INET, type, 0)) < 0 ){
				perror("net_open: Socket failed");
				close(fd);
				return -1;
			}
			printdbg(("\n net_open: Socket openened succesfully"));
			conf->sock_fd = fd;
			return fd;
		}
	}
}

int net_close(struct s2e_conf *conf) {
	if (conf->sock_fd <= 0)
		close(conf->sock_fd);
		conf->sock_fd = -1;
		printdbg(("\n net_close: close successfull"));
		return 0;

}

int net_read(struct s2e_conf *conf, char *buffer, int size) {
	int sock;
	int ret;
	socklen_t len;
	//struct sockaddr_in cliaddr; //for UDP
	sock = conf->sock_fd;
	/*conf->cliaddr.sin_family = AF_INET;
	conf->cliaddr.sin_addr.s_addr = inet_addr(conf->net_remote_ip);
	conf->cliaddr.sin_port = htons(conf->net_port);*/
	len = sizeof(conf->cliaddr);
	if (conf->net_mode == NET_PROTO_TCP) {
		if ((ret = recv(sock, buffer, size, 0)) < 0) {
			perror("\n net_read: read failed");
			return -1;
		}
			printdbg(("\n net_read: read successfull [%d bytes]", ret));
			return ret;
	}
	else {
		//printdbg(("\n [UDP] Not implemented"));
		if ((ret = recvfrom(sock, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&conf->cliaddr, &len)) < 0) {
			perror("\n net_read: read failed");
			return -1;
		}
		printdbg(("\n net_read: read successfull [%d bytes]", ret));
		return ret;

	}


}

int net_write(struct s2e_conf *conf, char *buffer, int size) {
	int sock;
	int ret;
	socklen_t len;
	//struct sockaddr_in cliaddr; //for UDP
	/*conf->cliaddr.sin_family = AF_INET;
	conf->cliaddr.sin_addr.s_addr = inet_addr(conf->net_remote_ip);
	conf->cliaddr.sin_port = htons(conf->net_port);*/
	sock = conf->sock_fd;
	len = sizeof(conf->cliaddr);
	if (conf->net_mode == NET_PROTO_TCP) {
		if ((ret = send(sock, buffer, size, 0)) < 0) {
			printf("\n net_write: write failed");
			return -1;
		}
		printdbg(("\n net_write: write successfull [%d bytes]", ret));
		return ret;
	}
	else {
		//printdbg(("\n [UDP] Not implemented"));
			if ((ret = sendto(sock, buffer, size, MSG_CONFIRM, (struct sockaddr *)&conf->cliaddr, len)) < 0) {
			perror("\n net_write: write failed");
			return -1;
		}
		printdbg(("\n net_write: write successfull [%d bytes]", ret));
		return ret;

	}
}

int tty_open(struct s2e_conf *conf) {
	int fd;
	int ret;
	struct termios serialSettings;
	conf->tty_fd = -1;
	if ((fd = open(conf->tty_device, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		perror("\n tty_open: open failed");
		close(fd);
		return -1;

	}
	printdbg(("\n tty_open: open succesfull"));

	/*Get default serial attributes*/
	if ((ret = ioctl(fd, TCGETS, &serialSettings)) < 0) {
		perror("\n tty_open: get default attributes failed");
		close(fd);
		conf->tty_fd = -1;
		return -1;
	}


	serialSettings.c_cflag = (CLOCAL | CREAD);
	serialSettings.c_iflag = (IGNPAR);
	serialSettings.c_oflag = 0;
	/*Setting the Baudrate*/
	switch (conf->tty_baudrate) {
	case 300:		serialSettings.c_cflag |= B300; 					break;
	case 600:		serialSettings.c_cflag |= B600;						break;
	case 1200:		serialSettings.c_cflag |= B1200;					break;
	case 2400:		serialSettings.c_cflag |= B2400;					break;
	case 4800:		serialSettings.c_cflag |= B4800;					break;
	case 9600:		serialSettings.c_cflag |= B9600;					break;
	case 19200:		serialSettings.c_cflag |= B19200;					break;
	case 38400:		serialSettings.c_cflag |= B38400;					break;
	case 57600:		serialSettings.c_cflag |= B57600;					break;
	case 115200:	serialSettings.c_cflag |= B115200;					break;
	case 230400:	serialSettings.c_cflag |= B230400;					break;
	case 460800:	serialSettings.c_cflag |= B460800;					break;
	case 921600:	serialSettings.c_cflag |= B921600;					break;
	default:		serialSettings.c_cflag |= B115200;					break;		/*By default use 115200 as baud rate*/
	}

	/*Setting the parity*/
	switch(conf->tty_parity) {
	case 1:			serialSettings.c_cflag |= (PARENB | PARODD);		break;
	case 2:			serialSettings.c_cflag |= PARENB;					break;
	case 0:
	default:		serialSettings.c_cflag &= ~(PARENB | PARODD);		break;
	}

	/*Configuring stop bit*/
	switch(conf->tty_stopbit) {
	case 2:			serialSettings.c_cflag |= CSTOPB;					break;
	case 1:
	default:		serialSettings.c_cflag &= ~CSTOPB;					break;
	}


	/*Configuring bit length */
	switch(conf->tty_length) {
	case 5:			serialSettings.c_cflag |= CS5;						break;
	case 6:			serialSettings.c_cflag |= CS6;						break;
	case 7:			serialSettings.c_cflag |= CS7;						break;
	case 8:
	default:		serialSettings.c_cflag |= CS8;						break;
	}

	/*Configuring flow control */
	switch(conf->tty_flow) {
	case 1:			serialSettings.c_iflag |= (IXON | IXOFF | IXANY);	break;
	case 2:			serialSettings.c_cflag |= CRTSCTS;					break;
	case 0:
	default:		serialSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
					serialSettings.c_cflag &= ~CRTSCTS;					break;
	}

	/*Desiable raw canonical data, Disable echo, Disable sig inerrupt */
	serialSettings.c_lflag &= !(ICANON | ECHO | ISIG);


	serialSettings.c_cc[VMIN] = 0;			/* read Waits (blocks) for a minimum of 1 byte */
	serialSettings.c_cc[VTIME] = 0;			/* No timeout*/



	if ((ret = ioctl(fd, TCSETS, &serialSettings)) , 0) {
		printdbg(("\n %s: Setting serial attributes failed"));
		fd = -1;
		return -1;
	}
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
	printdbg(("\n tty_write: write successfull [%d bytes]", ret));
	return ret;
}

int tty_read(struct s2e_conf *conf, char *buffer, int size) {
	int ret;
		int fd;
		fd = conf->tty_fd;
		if ((ret = read(fd, buffer, size)) < 0) {
			perror("\n tty_read: read failed");
			return -1;
		}
		printdbg(("\n tty_read: read successfull [%d bytes]", ret));
		return ret;
}

int tty_close(struct s2e_conf *conf) {
	if (conf->tty_fd > 0) {
		close(conf->tty_fd);
	}
	conf->tty_fd = -1;
	printdbg(("\n tty_close: tty close"));
	return 0;
}



