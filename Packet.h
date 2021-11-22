#include <stdio.h>

#define START_PACKET_ID 0xFFFF
#define END_PACKET_ID 0xFFFF
#define ACC_PER 0xFFF8
#define NOT_PAID 0xFFF9
#define NOT_EXIST 0xFFFA
#define ACCESS_OK 0xFFFB

const int PORT_NO = 7891;

struct Payload
{
    char technology;
    unsigned int subscriber_no;
};

struct DataPacket
{
    unsigned short start_packet_id;
    char client_id;
    unsigned short status;
    char segment_no;
    char length;
    struct Payload payload;
    unsigned short end_packet_id;
};

struct SubscriptionInfo
{
    unsigned int subscriber_no;
    char technology;
    char paid;
};

int generatePacketBufferToSend(struct DataPacket data_packet, char *buffer)
{
    int buffer_length = 0;

    memcpy(&buffer[buffer_length], &data_packet.start_packet_id, 2);
    buffer_length += 2;

    buffer[buffer_length] = data_packet.client_id;
    buffer_length += 1;

    memcpy(&buffer[buffer_length], &data_packet.status, 2);
    buffer_length += 2;

    buffer[buffer_length] = data_packet.segment_no;
    buffer_length += 1;

    buffer[buffer_length] = data_packet.length;
    buffer_length += 1;

    buffer[buffer_length] = data_packet.payload.technology;
    buffer_length += 1;

    memcpy(&buffer[buffer_length], &data_packet.payload.subscriber_no, 4);
    buffer_length += 4;

    memcpy(&buffer[buffer_length], &data_packet.end_packet_id, 2);
    buffer_length += 2;

    buffer[buffer_length + 1] = '\0';

    return buffer_length;
}

struct DataPacket parsePacketFromBuffer(char *buffer)
{
    struct DataPacket data_packet;
    int buffer_length = 0;

    memcpy(&(data_packet.start_packet_id), buffer + buffer_length, 2);
    buffer_length += 2;

    data_packet.client_id = buffer[buffer_length];
    buffer_length += 1;

    memcpy(&(data_packet.status), buffer + buffer_length, 2);
    buffer_length += 2;

    data_packet.segment_no = buffer[buffer_length];
    buffer_length += 1;

    data_packet.length = buffer[buffer_length];
    buffer_length += 1;

    data_packet.payload.technology = buffer[buffer_length];
    buffer_length += 1;

    memcpy(&(data_packet.payload.subscriber_no), buffer + buffer_length, 4);
    buffer_length += 4;

    memcpy(&(data_packet.end_packet_id), buffer + buffer_length, 2);
    buffer_length += 2;

    return data_packet;
}

char *description(unsigned short code)
{
    if (0xFFF9 == code)
    {
        return "NOT_PAID";
    }
    if (0xFFFA == code)
    {
        return "NOT_EXIST";
    }
    if (0xFFFB == code)
    {
        return "ACCESS_OK";
    }
    return "Unknow";
}

void printStatus(unsigned short status)
{
    switch (status)
    {
    case ACCESS_OK:
        printf("ACCESS PERMISSION GRANTED %x - %s\n", ACCESS_OK, description(ACCESS_OK));
        break;
    case NOT_PAID:
        printf("REJECT %x - %s\n", NOT_PAID, description(NOT_PAID));
        break;
    case NOT_EXIST:
        printf("REJECT %x - %s\n", NOT_EXIST, description(NOT_EXIST));
        break;
    }
}

struct DataPacket generateReqPacket(int technology, int subscriber_no, int segment_number)
{
    struct DataPacket req_packet;
    req_packet.start_packet_id = START_PACKET_ID;
    req_packet.client_id = 24;
    req_packet.status = ACC_PER;
    req_packet.payload.technology = technology;
    req_packet.payload.subscriber_no = subscriber_no;
    req_packet.segment_no = segment_number;
    req_packet.length = 5;
    req_packet.end_packet_id = END_PACKET_ID;
    return req_packet;
}

struct sockaddr_in getServerAddress(int port_no)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // htons() which converts a port number in host byte order to a port number in network byte order
    address.sin_port = htons(port_no);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    return address;
};