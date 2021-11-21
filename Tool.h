#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/**
 * This function shows the error message and stop the program
 * Input params are char* msg
 * No out put
 **/
void error(const char *msg)
{
  perror(msg);
  exit(1);
}