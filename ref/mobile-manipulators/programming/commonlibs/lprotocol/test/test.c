#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include "../lprotocol.h"

int testnumber = 2;

volatile unsigned char datachannel_byte;
volatile unsigned char datachannel_status_datasent = 0;
volatile unsigned char datachannel_close = 0;

void my_usleep(long int delay_us)
{
	struct timespec sleepTime;
	struct timespec remainingSleepTime;
	sleepTime.tv_sec=0;
	sleepTime.tv_nsec=delay_us*1000;
	nanosleep(&sleepTime,&remainingSleepTime);
}

void *master(void *arg)
{
	lprotocoldata_t protocoldata;
	lprotocoldatagram_t protocoldatagram;

	char  data_char[10];
	int   data_int[10];
	float data_float[10];
	int n;
	

	printf("\nMaster initialized\n");
	
	switch(testnumber){
	  case 0:
			lprotocol_init();
			lprotocol_datagram_encoder_init(0, &protocoldata, &protocoldatagram);
			data_char[0] = 12;
			data_char[1] = 92;
			lprotocol_datagram_encoder_insert_data((unsigned char *)&data_char[0], sizeof(char), &protocoldatagram);
			lprotocol_datagram_encoder_insert_data((unsigned char *)&data_char[1], sizeof(char), &protocoldatagram);
			lprotocol_datagram_encoder_end(&protocoldatagram);
	    break;
	  case 1:
			lprotocol_init();
			lprotocol_datagram_encoder_init(1, &protocoldata, &protocoldatagram);
			data_int[0] = 12;
			data_int[1] = 92;
			lprotocol_datagram_encoder_insert_data((unsigned char *)&data_int[0], sizeof(int), &protocoldatagram);
			lprotocol_datagram_encoder_insert_data((unsigned char *)&data_int[1], sizeof(int), &protocoldatagram);
			lprotocol_datagram_encoder_end(&protocoldatagram);
	    break;
	  case 2:
			lprotocol_init();
			lprotocol_datagram_encoder_init(LPROCOTOL_FUNCTION_GETTIME, &protocoldata, &protocoldatagram);
			data_float[0] = 12.0;
			lprotocol_datagram_encoder_insert_data((unsigned char *)&data_float[0], sizeof(float), &protocoldatagram);
			lprotocol_datagram_encoder_end(&protocoldatagram);
	    break;
	  case 3:
	    break;
	  case 4:
	    break;
	}
	
	// send datagram:
	printf("\ndatagrama a ser enviado pelo mestre: ");
	lprotocol_datagram_print(&protocoldatagram);
	
	datachannel_close = 0;
	for(n=0;n<protocoldatagram.datagram_size;++n){
		datachannel_byte = protocoldatagram.datagram[n];
		datachannel_status_datasent = 1;
		do{ 
//			printf("\n master datachannel_status_datasent = %i",datachannel_status_datasent);
			my_usleep(10000);
		}
		while(datachannel_status_datasent);
	}
	datachannel_close = 1;

	printf("\ndatagrama enviado.");

	lprotocol_close();

	return (NULL);
}

void *slave(void *arg)
{
	lprotocoldata_t protocoldata;
	lprotocoldatagram_t protocoldatagram;

	char  data_char[10];
	int   data_int[10];
	float data_float[10];
	
	lprotocol_init();

	printf("\nSlave initialized\n");
	
	// receive datagram:

	lprotocol_datagram_decoder_reset(&protocoldatagram);
	while(!datachannel_close){
		datachannel_status_datasent = 0;
		do{ 
			my_usleep(10000);
		}
		while((!datachannel_status_datasent) & (!datachannel_close));
		
		if((!datachannel_close)){
//			printf("\n slave (1) datagram_size = %i - state = %i - checksum = %i",protocoldatagram.datagram_size,protocoldatagram.state, protocoldatagram.checksum);

			lprotocol_datagram_decoder_process_received_byte(datachannel_byte, &protocoldatagram);

//			printf("\n slave (2) datagram_size = %i - state = %i - checksum = %i",protocoldatagram.datagram_size,protocoldatagram.state, protocoldatagram.checksum);
		}
	}

	printf("\ndatagrama recebido pelo escravo: ");
	
	lprotocol_datagram_print(&protocoldatagram);
	
	switch(testnumber){
	  case 0:
	    break;
	  case 1:
	    break;
	  case 2:
	    break;
	  case 3:
	    break;
	  case 4:
	    break;
	}

	lprotocol_close();

	return (NULL);
}

int main(void)
{
	pthread_t master_thread;
	pthread_t slave_thread;
	pthread_attr_t pthread_custom_attr;

	pthread_attr_init(&pthread_custom_attr);

	pthread_create(&master_thread, &pthread_custom_attr, master, NULL);
	pthread_create(&slave_thread,  &pthread_custom_attr, slave, NULL);

	pthread_join(master_thread,NULL);
	pthread_join(slave_thread,NULL);

	printf("\n"); 
	fflush(stdout); // mostra todos printfs pendentes.
	
	return 0;
}

