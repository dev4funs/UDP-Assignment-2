#include <stdio.h>

#define START_PACKET_ID 0xFFFF
#define END_PACKET_ID 0xFFFF

#define ACC_PER 0xFFF8
#define NOT_PAID 0xFFF9
#define NOT_EXIST 0xFFFA
#define ACCESS_OK 0xFFFB

struct _payload {
    char technology;
    unsigned int source_subscriber_no;
};

struct identification_packet {
    unsigned short start_packet_id;
    char client_id;
    unsigned short status;
    char segment_no;
    char length;
    struct _payload payload;
    unsigned short end_packet_id;
};

struct subscriber_status_details {
    unsigned int subscriber_no;
    char technology;
    char paid;
};

/**
 * This function builds the buffer to be sent using the data in the req_packet 
 * Input params are req_packet and buffer
 * Output is the length of the data in the buffer
 **/
int buildPacket(struct identification_packet req_packet, char* buffer) {

    int buffer_length = 0;

    memcpy(&buffer[buffer_length], &req_packet.start_packet_id, 2);
    buffer_length += 2;

    buffer[buffer_length] = req_packet.client_id;
    buffer_length += 1;

    memcpy(&buffer[buffer_length], &req_packet.status, 2);
    buffer_length += 2;

    buffer[buffer_length] = req_packet.segment_no;
    buffer_length += 1;

    buffer[buffer_length] = req_packet.length;
    buffer_length += 1;

    buffer[buffer_length] = req_packet.payload.technology;
    buffer_length += 1;

    memcpy(&buffer[buffer_length], &req_packet.payload.source_subscriber_no, 4);
    buffer_length += 4;

    memcpy(&buffer[buffer_length], &req_packet.end_packet_id, 2);
    buffer_length += 2;

    buffer[buffer_length + 1] = '\0';

    return buffer_length;
}

/**
 * This method decodes data in the buffer into the identification packet
 * Input params are buffer and id_packet
 * Output is an int - 
 **/
int decodePacket(char* buffer, struct identification_packet* id_packet) {

    int buffer_length = 0;

    memcpy(&(id_packet->start_packet_id), buffer + buffer_length, 2);
    buffer_length += 2;

    id_packet->client_id = buffer[buffer_length];
    buffer_length += 1;
    
    memcpy(&(id_packet->status), buffer + buffer_length, 2);
    buffer_length += 2;

    id_packet->segment_no = buffer[buffer_length];
    buffer_length += 1;

    id_packet->length = buffer[buffer_length];
    buffer_length += 1;

    id_packet->payload.technology = buffer[buffer_length];
    buffer_length += 1;

    memcpy(&(id_packet->payload.source_subscriber_no), buffer + buffer_length, 4);
    buffer_length += 4;

    memcpy(&(id_packet->end_packet_id), buffer + buffer_length, 2);
    buffer_length += 2;
    
    return buffer_length;
}

void printStatus(unsigned short status) {

    switch(status) {
        case 0xFFF9: printf("REJECT %x - NOT_PAID\n", NOT_PAID); break;
        case 0xFFFA: printf("REJECT %x - NOT_EXIST\n", NOT_EXIST); break;
        case 0xFFFB: printf("ACCESS PERMISSION GRANTED %x - ACCESS_OK\n", ACCESS_OK); break;
    }
}