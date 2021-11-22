#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Packet.h"

int getNumberOfSubscribers();
void readVerificationDatabaseFromTxt(struct SubscriptionInfo *subscriber_info);

int getNumberOfSubscribers()
{
  FILE *fp;
  char buffer[100];

  fp = fopen("./Verification_Database.txt", "r");
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

void readVerificationDatabaseFromTxt(struct SubscriptionInfo *subscriber_info)
{

  FILE *fp;
  fp = fopen("./Verification_Database.txt", "r");
  char buffer[100];
  fgets(buffer, 100, fp); //Ignore first line with headings

  char subscriber_number[20];
  char technology[10];
  int paid;

  int i = 0;

  while (fscanf(fp, "%s %s %d\n", subscriber_number, technology, &paid) == 3)
  {
    printf("Subscriber: %s, Technology %s, Paid: %d\n", subscriber_number, technology, paid);
    char subscriber_number_temp[11];

    // Convert string 123-456-7890 to string 1234567890
    strncpy(subscriber_number_temp, subscriber_number, 3);
    strncpy(subscriber_number_temp + 3, subscriber_number + 4, 3);
    strncpy(subscriber_number_temp + 6, subscriber_number + 8, 4);

    subscriber_info[i].subscriber_no = atoi(subscriber_number_temp);
    subscriber_info[i].technology = atoi(technology);
    subscriber_info[i].paid = paid;

    i++;
  }
  fclose(fp);
}

// Verify the subscriber against subscriber_info
unsigned short verify(int num_subscribers, struct SubscriptionInfo *subscriber_info, struct DataPacket data_packet)
{
  for (int i = 0; i < num_subscribers; i++)
  {

    if (data_packet.payload.subscriber_no == subscriber_info[i].subscriber_no && subscriber_info[i].technology == data_packet.payload.technology)
    {
      return (subscriber_info[i].paid == 1) ? ACCESS_OK : NOT_PAID;
    }
  }
  return NOT_EXIST;
}