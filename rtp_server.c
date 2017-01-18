#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TS_PACKET_SIZE 188  
#define MTU 1500 

struct rtp_header{
        unsigned char cc:4;
        unsigned char x:1;
        unsigned char p:1;
        unsigned char v:2;

        unsigned char pt:7;
        unsigned char m:1;

        unsigned short sequence_number;
        unsigned int timestamp;
        unsigned int ssrc;
};

void init_rtp_header(struct rtp_header *h){
        h->v = 2;
        h->p = 0;
        h->x = 0;
        h->cc = 0;
        h->m = 0;
        h->pt = 33;
        h->sequence_number = 123;
        h->timestamp = 123;
        h->ssrc = 123;
}

void sequence_number_increase(struct rtp_header *header){
        unsigned short sequence = ntohs(header->sequence_number);
        sequence++;
        header->sequence_number = htons(sequence);
}

int main(){
        // RTP Packet we will send  
    char buf[MTU];
    //char * tsFile="/media/sda1./enigma_stream.ts";
    char * tsFile="./enigma_stream.ts";
    unsigned int count = 0;
    unsigned char remote[16] = {0};
    char databuf[1600] = {0};
    struct sockaddr_in groupSock;
    struct sockaddr_in sa = {0};
    struct ip_mreqn mreqn = {{0}};
    int datalen;


        // Init RTP Header  
        init_rtp_header((struct rtp_header*)buf);
        count = sizeof(struct rtp_header);

        // Init socket  
        int sock = socket(AF_INET, SOCK_DGRAM, 0);

      sprintf(remote, "%u.%u.%u.%u",224,1,1,1);

    if((224&0xf0) == 0xe0){
        int stream_ttl=0;
        setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &stream_ttl, sizeof(stream_ttl));
        mreqn.imr_ifindex = if_nametoindex("eth0");
        setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (void *)&mreqn, sizeof(mreqn));

        /* Initialize the group sockaddr structure with a */
        /* group address of dynamic address and port dynamic port. */
        memset((char *) &groupSock, 0, sizeof(groupSock));
        groupSock.sin_family = AF_INET;
        groupSock.sin_addr.s_addr = inet_addr(remote);
        groupSock.sin_port = htons(1234);
    } else {
        sa.sin_family = AF_INET;
        sa.sin_port = htons(1234);
        inet_pton(AF_INET, remote, &sa.sin_addr);
    }

        // Open TS file  
        FILE *ts_file = fopen(tsFile, "r+");

        if(ts_file == NULL){
            printf("the file is opeaned error!\n");
            return -1;
        }

        while(1){
                fseek(ts_file,0L,SEEK_SET);

                while(!feof(ts_file)){
                        int read_len = fread(buf+count, 1, TS_PACKET_SIZE, ts_file);
                        if(*(buf+count) != 0x47){
                                fprintf(stderr, "Bad sync header!\n");
                                continue;
                        }
                        count += read_len;

                        if (count + TS_PACKET_SIZE > MTU){// We should send
                                sequence_number_increase((struct rtp_header*)buf);
                                //sendto(sock, buf, count, 0, (const struct sockaddr*)&dest_addr, sizeof(dest_addr));
                                sendto(sock, buf, count, 0, (const struct sockaddr*)&groupSock, sizeof(struct sockaddr));

                                count = sizeof(struct rtp_header);
                                usleep(20);
                        }
                }
        }

        fclose(ts_file);

        return 0;
}

