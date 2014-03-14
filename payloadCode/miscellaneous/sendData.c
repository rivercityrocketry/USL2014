/*****************************
GPS TEST CAPE SCRIPT
Created on: Aug 06, 2013
Author: dcarona
******************************/
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "termios.h"
#include "string.h"
#include "unistd.h"
#define TRUE 1
#define FALSE 0
int fd;
char buffer[8];
struct Location
{
  char Time[8];
  char status[2];
  char Lat[10];
  char N_S_indicator[2];
  char Long[11];
  char E_W_indicator[2];
  char Speed[9];
  char angle[7];
  char Date[7];
};

int readLine(char * result);
void parseMSG(char *msg, struct Location *location, int size);
void sendInfoToServer (char *message);
void buildLink (struct Location *location, char * addr);
int main()
{
  int size;
  struct termios opts;
  struct Location location;
  char line[128];
  char address[256];
  int test_send = FALSE;
  /* Attempt to open the serial port */
  if ((fd = open("/dev/ttyO1",O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
    printf("Unable to open serial device, error %d\n",fd);
  }
  /* Configure serial port */
  tcgetattr(fd,&opts);
  opts.c_cflag = (opts.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  opts.c_iflag &= ~IGNBRK; // ignore break signal
  opts.c_lflag = 0; // no signaling chars, no echo,
  // no canonical processing
  opts.c_oflag = 0; // no remapping, no delays
  opts.c_cc[VMIN] = 0; // read doesn't block
  opts.c_cc[VTIME] = 5; // 0.5 seconds read timeout
  opts.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
  opts.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  // enable reading
  opts.c_cflag &= ~(PARENB | PARODD); // shut off parity
  opts.c_cflag |= 0;
  opts.c_cflag &= ~CSTOPB;
  opts.c_cflag &= ~CRTSCTS;
  cfsetispeed(&opts,B4800);
  tcsetattr(fd,TCSANOW,&opts);

  for (;;) {
    //Delay
    if (test_send == TRUE) {
      sleep(1800); // Delay of 30min = 1800s
      test_send = FALSE;
    }
    size = readLine(line);
    if (strstr(line,"$GPRMC") != 0) {
      test_send = TRUE;
      parseMSG(line, &location, size);
      buildLink(&location, address);
      printf("Address:[%s]\n", address);
      sendInfoToServer(address);
      tcflush(fd, TCIFLUSH);
    }
  }
}
/*
 * Reads one complete line from GPS serial port
 */
int readLine(char * result)
{
  int lineFeedFound =FALSE;
  int crfound = FALSE;
  int i = 0;
  char c;
  while ((lineFeedFound==FALSE) && (i < 128)) {
    if (read(fd,&c,1) > 0) {
      result[i++] = c;
      if (c == 10) {
	result[i++]=0;
	lineFeedFound = TRUE;
      }
    }
  }
  return i;
}

/*
 * Separates each message into several fields and afects location struct.
 */
void parseMSG(char *msg, struct Location *location, int size)
{
  //$GPRMC,090446.000,A,3845.5509,N,00906.9152,W,0.00,129.54,080813,,,A*79
  char comma[2] = ",";
  char point[2] = ".";
  char *copy_time, *copy_vel;
  char *token;
  char *token_time, *token_vel;
  float vel;
  //printf("Msg:[%s]\n", msg);
  token = strtok(msg, comma);
  token = strtok(NULL, comma);
  copy_time = token;
  token = strtok(NULL, comma);
  if (*token == 'A') {
    //*********** STATUS *************************//
    memcpy(location->status, token, strlen(token));
    //INSERT 0 IN THE END OF STRING SO SYSTEM CAN DETECT ITS END
    location->status[strlen(token)] = 0;
    //*********** LATITUDE ***********************//
    token = strtok(NULL, comma);
    memcpy(location->Lat, token, strlen(token));
    location->Lat[strlen(token)] = 0;
    //*********** N S INDICATOR ******************//
    token = strtok(NULL, comma);
    memcpy(location->N_S_indicator, token, strlen(token));
    location->N_S_indicator[strlen(token)] = 0;
    //*********** LONGITUDE **********************//
    token = strtok(NULL, comma);
    memcpy(location->Long, token, strlen(token));
    location->Long[strlen(token)] = 0;
    //*********** E W INDICATOR ******************//
    token = strtok(NULL, comma);
    memcpy(location->E_W_indicator, token, strlen(token));
    location->E_W_indicator[strlen(token)] = 0;
    token = strtok(NULL, comma);

    copy_vel = token;
    //*********** ANGLE **************************//
    token = strtok(NULL, comma);
    memcpy(location->angle, token, strlen(token));
    location->angle[strlen(token)] = 0;
    //*********** DATE ***************************//
    token = strtok(NULL, comma);
    memcpy(location->Date, token, strlen(token));
    location->Date[strlen(token)] = 0;
    //*********** TIME ***************************//
    token_time = strtok(copy_time, point);
    memcpy(location->Time, token_time, strlen(token_time));
    location->Time[strlen(token_time)] = 0;
    //*********** VELOCITY ***********************//
    token_vel = strtok(copy_vel, point);
    vel = atof(token_vel);
    vel = vel * 1.852;
    sprintf(location->Speed, "%f", vel);
    location->Speed[strlen(token_vel)] = 0;
  }
}
/*
 * Sends position information to a specified server
 */
void sendInfoToServer (char *message) {
  system(message);
}
/*
 * Builds link to use in the call of server info update according with the 
 specified format
*/
void buildLink (struct Location *location, char * address) {
  strcpy(address, "curl “your link”,");
  strcat(address, location->Lat);
  strcat(address, ",");
  strcat(address, location->N_S_indicator);
  strcat(address, ",");
  strcat(address, location->Long);
  strcat(address, ",");
  strcat(address, location->E_W_indicator);
  strcat(address, ",");
  strcat(address, location->Date);
  strcat(address, ",");
  strcat(address, location->Time);
  strcat(address, ",");
  strcat(address, location->Speed);
  strcat(address, ",");
  strcat(address, "0");
}
