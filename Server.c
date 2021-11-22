#include "Server.h"

int main()
{
    struct DataPacket data_packet;
    int num_subscribers = getNumberOfSubscribers();
    struct SubscriptionInfo subscriber_info[num_subscribers];
    readVerificationDatabaseFromTxt(subscriber_info);

    int server_socket;
    char buffer[1024];
    struct sockaddr_in server_address;

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    /*---- Configure server address struct ----*/
    server_address = GetServerAddress(PORT_NO);

    /*---- Bind the address struct to the socket ----*/
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    struct sockaddr sender;
    socklen_t sendsize = sizeof(sender);
    memset(&sender, 0, sizeof(sender));

    while (1)
    {
        int recvlen = recvfrom(server_socket, buffer, sizeof(buffer), 0, &sender, &sendsize);
        if (recvlen >= 0)
        {
            data_packet = parsePacketFromBuffer(buffer);
            printf("\n");
            printf("Received packet %d\n", data_packet.segment_no);
            data_packet.status = verify(num_subscribers, subscriber_info, data_packet);
            printStatus(data_packet.status);
            int packet_length = generatePacketBufferToSend(data_packet, buffer);
            sendto(server_socket, buffer, packet_length, 0, (struct sockaddr *)&sender, sendsize);
            printf("\n");
        }
    }

    return 0;
}
