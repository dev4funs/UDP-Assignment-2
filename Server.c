#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Packet.h"

int getNumberOfSubscribers();
void loadVerificationDatabase(struct subscriber_status_details *subscriber_details);

int main()
{

    struct identification_packet id_packet;
    int num_subscribers = getNumberOfSubscribers();
    struct subscriber_status_details subscriber_details[num_subscribers];

    loadVerificationDatabase(subscriber_details);

    for (int i = 0; i < num_subscribers; i++)
    {
        printf("Subscriber: %u, Technology %d, Paid: %d\n", subscriber_details[i].subscriber_no, subscriber_details[i].technology, subscriber_details[i].paid);
    }

    int server_socket;
    char buffer[1024];
    struct sockaddr_in serverAddr;

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    /*---- Configure server address struct ----*/
    serverAddr = GetServerAddress(PORT_NO);

    /*---- Bind the address struct to the socket ----*/
    bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    struct sockaddr sender;
    socklen_t sendsize = sizeof(sender);
    memset(&sender, 0, sizeof(sender));

    // Listening for packets from client indefinitely
    while (1)
    {

        recvfrom(server_socket, buffer, sizeof(buffer), 0, &sender, &sendsize);

        printf("Received packet %s\n", buffer);

        int response = decodePacket(buffer, &id_packet);

        // Verify the subscriber against subscriber_details
        int i = 0;
        int subscriberFound = 0;
        while (i < num_subscribers)
        {

            // Check if subscriber found and if technology matches
            if (id_packet.payload.source_subscriber_no == subscriber_details[i].subscriber_no && subscriber_details[i].technology == id_packet.payload.technology)
            {

                // Verify if paid or not
                id_packet.status = (subscriber_details[i].paid == 1) ? ACCESS_OK : NOT_PAID;
                subscriberFound = 1;
                break;
            }
            i++;
        }

        // If subscriber not found or if subscriber found but technology does not match
        if (subscriberFound == 0)
        {
            id_packet.status = NOT_EXIST;
        }

        printStatus(id_packet.status);

        int packet_length = buildPacket(id_packet, buffer);

        // Send ACK or REJECT packet to client.
        sendto(server_socket, buffer, packet_length, 0, (struct sockaddr *)&sender, sendsize);

        printf("\n\n");
    }

    return 0;
}

int getNumberOfSubscribers()
{
    FILE *fp;
    fp = fopen("./Verification_Database.txt", "r");
    char buffer[100];
    fgets(buffer, 100, fp); //Ignore first line with headings

    char subscriber_number[100];
    char technology[10];
    int paid;
    int lines = 0;

    while (fscanf(fp, "%s %s %d\n", subscriber_number, technology, &paid) == 3)
    {
        lines++;
    }
    fclose(fp);

    return lines;
}

void loadVerificationDatabase(struct subscriber_status_details *subscriber_details)
{

    FILE *fp;
    fp = fopen("./Verification_Database.txt", "r");
    char buffer[100];
    fgets(buffer, 100, fp); //Ignore first line with headings

    char subscriber_number[100];
    char technology[10];
    int paid;

    int i = 0;

    while (fscanf(fp, "%s %s %d\n", subscriber_number, technology, &paid) == 3)
    {
        // printf("Subscriber: %s, Technology %s, Paid: %d\n", subscriber_number, technology, paid);
        char subscriber_number_temp[11];

        // Convert string 123-456-7890 to string 1234567890
        strncpy(subscriber_number_temp, subscriber_number, 3);
        strncpy(subscriber_number_temp + 3, subscriber_number + 4, 3);
        strncpy(subscriber_number_temp + 6, subscriber_number + 8, 4);

        // printf("Subscriber int: %s %u\n", subscriber_number_temp, atoi(subscriber_number_temp));

        subscriber_details[i].subscriber_no = atoi(subscriber_number_temp);
        subscriber_details[i].technology = atoi(technology);
        subscriber_details[i].paid = paid;

        i++;
    }
    fclose(fp);
}