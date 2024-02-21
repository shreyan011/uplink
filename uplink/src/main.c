#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>   // File Control Definitions
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions */
#include <errno.h>   /* ERROR Number Definitions */
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 6060
#define MAX 1024
#define JSON_DEVICE_SHADOW_MESSAGES_BUFF_LEN 2024

uint8_t json_device_shadow_message_buff_gau8[JSON_DEVICE_SHADOW_MESSAGES_BUFF_LEN] = {0,};
char device_shadow_json[] =
    "{"
    "\"stream\": \"device_shadow\","
    "\"sequence\": %d"
    "\"timestamp\": %llu,"
    "\"Status\": %s,"
    "}\n";

int sockfd = 0;
int uplink_client_init_s32(void)
{
    int connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.248");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
        return -1;
    }
    else
    {
        printf("connected to the server..\n");
        return 0;
    }
}

uint8_t json_pack_device_shadow_message()
{

    uint64_t timestamp = (uint64_t)time(NULL);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t s = tv.tv_sec;
    double ms = (tv.tv_usec) / 1.0e6;

    //	printf("time stamp while packing json:%ld, %llu, %ld\n", (long)timestamp, (long)s, (long)ms); double timestamp1 = (s + ms)*1000;
    double timestamp1 = (s + ms) * 1000;
    uint64_t time_stamp = (uint64_t)timestamp1;
    static uint32_t seq_id = 0;
    seq_id++;
    snprintf(json_device_shadow_message_buff_gau8, sizeof(json_device_shadow_message_buff_gau8), device_shadow_json, time_stamp,
             seq_id, "hello_world");
    //printf("The packed JSON is : %s\n", json_can_message_buff_gau8);
    printf("The packed JSON is : %s\n",  json_device_shadow_message_buff_gau8);
    return 0;
}

void uplink_push_vp()
{
    int i;
    int nbytes;
    struct ifreq ifr;

   // while (1)
    //{

        json_pack_device_shadow_message();
        int ret = 0;
        //ret = write(sockfd, json_pack_device_shadow_message, strlen(json_pack_device_shadow_message));
        ret = write(sockfd, json_device_shadow_message_buff_gau8, strlen(json_device_shadow_message_buff_gau8));
        printf("Ret: %d\n",ret);

        if (ret < 0)
        {
            exit(0);
        }
    //}
}

int main()
{

    int i = 0;
    //int ret;

    // Connecting to Uplink Socket Server
    uplink_client_init_s32();
    uplink_push_vp();

    return 0;
}
