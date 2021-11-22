#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include "Packet.h"

const int retry_times = 3;

void requestIdentification(int client_socket, struct sockaddr_in server_address, int technology, int subscriber_no, int segment_number);
int sendPacket(int client_socket, struct DataPacket req_packet, struct sockaddr_in server_address, char *buffer);
void receivePacket(int client_socket, struct sockaddr_in server_address, char *req_buffer, int packet_length);

void requestIdentification(int client_socket, struct sockaddr_in server_address, int technology, int subscriber_no, int segment_number)
{
  char buffer[1024];
  struct DataPacket req_packet = generateReqPacket(technology, subscriber_no, segment_number);
  int packet_length = sendPacket(client_socket, req_packet, server_address, buffer);
  receivePacket(client_socket, server_address, buffer, packet_length);
  printf("\n\n");
};

int sendPacket(int client_socket, struct DataPacket req_packet, struct sockaddr_in server_address, char *buffer)
{

  int packet_length = generatePacketBufferToSend(req_packet, buffer);
  sendto(client_socket, buffer, packet_length, 0, (struct sockaddr *)&server_address, sizeof server_address);
  return packet_length;
}

void receivePacket(int client_socket, struct sockaddr_in server_address, char *req_buffer, int packet_length)
{
  struct sockaddr sender;
  struct DataPacket data_packet;
  char ack_buffer[1024];

  socklen_t sendsize = sizeof(sender);
  memset(&sender, 0, sizeof(sender));
  bind(client_socket, (struct sockaddr *)&sender, sendsize);

  int count = 0;

  for (;;)
  {
    if (count < retry_times)
    {
      int recvlen = recvfrom(client_socket, ack_buffer, sizeof(ack_buffer), 0, &sender, &sendsize);

      if (recvlen >= 0) // Message Received
      {
        data_packet = parsePacketFromBuffer(ack_buffer);
        printStatus(data_packet.status);
        break;
      }
      else
      {

        printf("%d.   .....\n", count + 1);
        sendto(client_socket, req_buffer, packet_length, 0, (struct sockaddr *)&server_address, sizeof server_address);
        count++;
      }
    }
    else
    {
      printf("No response from server! \n");
      break;
    }
  }
}
