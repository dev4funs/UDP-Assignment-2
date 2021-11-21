#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include "Tool.h"
#include "packet.h"

void sendRequestPacket(int client_socket, struct sockaddr_in serverAddress, int technology, int subscriber_no, int segment_number);
void receivePacket(int client_socket, struct sockaddr_in serverAddress, char *req_buffer, int packet_length);

int main()
{
    int client_socket;
    struct sockaddr_in serverAddress;
    int segment_number = 1;

    // Create client socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Initialize Server socket
    serverAddress = GetServerAddress(PORT_NO);

    // Set timer options on socket
    struct timeval timeout = {3, 0};
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        error("Error");
    }

    // Nonexistent subscriber
    sendRequestPacket(client_socket, serverAddress, 1, 3085546805, segment_number);
    // Not paid subscriber
    sendRequestPacket(client_socket, serverAddress, 3, 4086668821, segment_number);
    // Subscriber exitst but technology does not match
    sendRequestPacket(client_socket, serverAddress, 5, 4086808821, segment_number);
    // Paid Subscriber with matching technology
    sendRequestPacket(client_socket, serverAddress, 4, 4085546805, segment_number);
}

void sendRequestPacket(int client_socket, struct sockaddr_in serverAddress, int technology, int subscriber_no, int segment_number)
{

    char buffer[1024];
    struct identification_packet req_packet;

    // Initialize request packet
    req_packet.start_packet_id = START_PACKET_ID;
    req_packet.client_id = 24;
    req_packet.status = ACC_PER;
    req_packet.segment_no = segment_number;
    req_packet.length = 5;
    req_packet.payload.technology = technology;
    req_packet.payload.source_subscriber_no = subscriber_no;
    req_packet.end_packet_id = END_PACKET_ID;

    int packet_length = buildPacket(req_packet, buffer);

    // Send data packet to server
    sendto(client_socket, buffer, packet_length, 0, (struct sockaddr *)&serverAddress, sizeof serverAddress);
    receivePacket(client_socket, serverAddress, buffer, packet_length);
    printf("\n\n");
}

void receivePacket(int client_socket, struct sockaddr_in serverAddress, char *req_buffer, int packet_length)
{
    // Receive ACK or REJECT packet from server
    char ack_buffer[1024];
    struct sockaddr sender;
    struct identification_packet id_packet;

    socklen_t sendsize = sizeof(sender);
    memset(&sender, 0, sizeof(sender));
    bind(client_socket, (struct sockaddr *)&sender, sendsize);

    int retry_counter = 0;

    while (retry_counter <= 3)
    {
        int recvlen = recvfrom(client_socket, ack_buffer, sizeof(ack_buffer), 0, &sender, &sendsize);

        if (recvlen >= 0)
        {
            // Message Received
            decodePacket(ack_buffer, &id_packet);
            printStatus(id_packet.status);
            break;
        }
        else
        {
            // If there is still no response from server after the last try to retransmit
            if (retry_counter == 3)
            {
                printf("Server does not respond\n");
                break;
            }
            retry_counter++;
            printf("Trying to retransmit..... Attempt no. %d\n", retry_counter);
            sendto(client_socket, req_buffer, packet_length, 0, (struct sockaddr *)&serverAddress, sizeof serverAddress);
        }
    }
}
