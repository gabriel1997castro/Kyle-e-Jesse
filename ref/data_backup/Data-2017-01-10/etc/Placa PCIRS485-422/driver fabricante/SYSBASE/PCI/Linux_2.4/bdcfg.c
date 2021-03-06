
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


#define TIOCGNUMOFPORT	0x545F

void MKNOD(char *filename, int major, int minor);

int main(int argc, char *argv[])
{

	int fd,i;
	unsigned int ports_num;
	char list[128];
	char name[128];
	
	MKNOD("/dev/ttyMPCON",54,0);
	
	if( (fd = open("/dev/ttyMPCON",O_RDWR)) < 0 ){
		printf("Can't open /dev/ttyMPCON\n");
		printf("Please check your board in slot..\n");
		return -1;
	}

	ports_num = ioctl(fd,TIOCGNUMOFPORT);
	close(fd);

	unlink("/dev/ttyMPCON");

	if ( ports_num <= 0 ){
		printf("No Multiports/PCI board found..!!\n");
		return -1;
	}
	
	if ( ports_num > 128){
		printf("Invalid Multiport operatation.\n");
		return -1;
	}

	unlink("Node");

	fd = open("Node", O_CREAT | O_RDWR | O_APPEND);

	write(fd,"a\n",2);

	write(fd,"## ttyMPx... made by Multiports/PCI installation ##\n", 52);
	for(i=0; i<ports_num; i++){
		sprintf(name,"/dev/ttyMP%d",i);
		MKNOD(name,54,i);
		sprintf(list,"mp%d:2345:off:/sbin/agetty ttyMP%d 9600 vt100\n",i,i);
		if ( i < 10 )
			write(fd,list,44);
		else
			write(fd,list,46);
	}

	write(fd,".\nw\nq\n",6);
	close(fd);

	_exit(0);
			
}

void MKNOD(char *filename, int major, int minor)
{
	unsigned short newmode;

	newmode = 0766 & ~umask(0);

	unlink(filename);

	if(mknod((char *)filename, newmode | S_IFCHR, makedev(major,minor)))
	{
		printf("MKNOD : Can't make device %s\n", filename);
		exit(1);
	}
}
