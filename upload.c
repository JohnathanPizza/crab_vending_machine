#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>

int portSetup(int fd){
	struct termios t;
	if(tcgetattr(fd, &t)){
		close(fd);
		return 1;
	}
	t.c_cflag &= ~PARENB;
	t.c_cflag |= CS8;
	t.c_cflag &= ~CRTSCTS;
	t.c_cflag |= CREAD | CLOCAL;
	t.c_lflag &= ~ICANON;
	t.c_lflag &= ~ECHO;
	t.c_lflag &= ~ECHOE;
	t.c_lflag &= ~ECHONL;
	t.c_lflag &= ~ISIG;
	t.c_iflag &= ~(IXON | IXOFF | IXANY);
	t.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
	t.c_oflag &= ~OPOST;
	t.c_oflag &= ~ONLCR;
	t.c_cc[VTIME] = 0 ; // deciseconds
	t.c_cc[VMIN] = 0;
	cfsetispeed(&t, B38400);
	if(tcsetattr(fd, TCSANOW, &t)){
		close(fd);
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[]){
	if(argc != 3){
		puts("need more args");
		return 1;
	}

	int portfd = open(argv[1], O_RDWR);
	if(portfd < 0){
		perror("couldnt open device file");
		return 1;
	}
	if(portSetup(portfd)){
		puts("couldnt configure port correctly");
		return 1;
	}
	
	struct pollfd fda[1] = {{.fd = portfd, .events = POLLOUT}};
	poll(fda, 1, -1); // millis
	ioctl(portfd, TCFLSH, 2);
	poll(fda, 1, -1); // millis

	puts("waiting for device");
	fda[0].events = POLLIN;
	while(1){
		char c = '!';
		write(portfd, &c, 1);
		poll(fda, 1, 10);
		if(fda[0].revents & POLLIN){
			if(read(portfd, &c, 1) && c == '!'){
				break;
			}
		}
	}

	int filefd = open(argv[2], O_RDONLY);
	if(filefd < 0){
		perror("couldnt open data file");
		return 1;
	}

	puts("creating data buffer");
	unsigned char* dataBuffer = malloc(0x8000);
	if(!dataBuffer){
		puts("couldnt allocate space for the data buffer");
		return 1;
	}
	memset(dataBuffer, 0, 0x8000);

	puts("reading into data buffer");
	if(!read(filefd, dataBuffer, 0x8000)){
		puts("error reading into data buffer");
		return 1;
	}
	close(filefd);
	
	for(int pagechunk = 0; pagechunk < 0x20; ++pagechunk){
		printf("writing chunk %d\n", pagechunk);
		write(portfd, dataBuffer + pagechunk * 0x400, 0x400);
		while(1){
			char c;
			poll(fda, 1, 0);
			if(fda[0].revents & POLLIN){
				if(read(portfd, &c, 1) && c == '.'){
					break;
				}
			}
		}
	}

	puts("done");
	free(dataBuffer);
	close(portfd);
	return 0;
}
