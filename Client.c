#include "Client.h"
#include "Tool.h"
struct Request
{
    int technology;
    int subscriber_no;
    int segment_number;
};

struct Request requests[] = {
    {.technology = 4, .subscriber_no = 4085546805, .segment_number = 1},
    {.technology = 1, .subscriber_no = 6693020691, .segment_number = 2},
    {.technology = 3, .subscriber_no = 4086668821, .segment_number = 3},
    {.technology = 5, .subscriber_no = 4086808821, .segment_number = 4},
};

int main()
{
    // Init socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // Init Server address
    struct sockaddr_in server_address = GetServerAddress(PORT_NO);

    // Set timer options on socket
    struct timeval timeout = {3, 0};
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        error("Error");
    }

    for (int i = 0; i < 4; i++)
        requestIdentification(client_socket, server_address, requests[i].technology, requests[i].subscriber_no, requests[i].segment_number);

    return 0;
}
