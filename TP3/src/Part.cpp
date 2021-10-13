#include "Common.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <new>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>

int sd, rc, i;
struct hostent *h;
struct sockaddr_in cliAddr, remoteServAddr;
char msg[MAX_REQUEST_MSG];

bool WriteFile() {
  std::ofstream file;
  file.open("resultado.txt", std::ios_base::app);
  std::string now = Common::APICommands::GetTimeNow();
  file << "[" << getpid() << "] " << now << "\n";
  file.close();
  return true;
}

bool SendMessage(std::string msgToSend, struct sockaddr_in remoteServAddr) {
  char * msgToSendReady = const_cast<char*>(msgToSend.c_str());
  rc = sendto(sd, msgToSendReady, MAX_REQUEST_MSG, 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
  if (rc<0) {
    printf("%s Cannot send data to '%s' (IP : %s) \n", "[ERROR]", h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
    return false;
  }
  printf("%s Sent %s to '%s' (IP : %s) \n", "[INFO]", msgToSendReady, h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
  return true;
}

bool SetSocketConnection() {
  /* get server IP address (no check if input is IP address or DNS name */
  h = gethostbyname(LOCAL_SERVER_HOST);
  if (h==NULL) {
    printf("%s Unknown host: %s \n", "[ERROR]", LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if (sd<0) {
    printf("%s Cannot open socket on %s\n", "[ERROR]", LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if (rc<0) {
    printf("%s Cannot bind port on %s\n", "[ERROR]", LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

  int repeats, waitSeconds;
  char * pEnd;
  if (argc < 3 || !(repeats = strtol(argv[1],&pEnd,10)) ||  !repeats) {
    printf("Enter a valid number n > 0 as a parameter\n");
    exit(EXIT_FAILURE);
  }
  waitSeconds = strtol(argv[2],&pEnd,10);

  if (SetSocketConnection()) {
    exit(EXIT_FAILURE);
  }

  for(int i=0;i<repeats;i++) {
    std::string msgToSend = Common::APICommands::GenerateMessage(REQUEST, (int)getpid());
    if (!SendMessage(msgToSend, remoteServAddr)) {
      close(sd);
      exit(EXIT_FAILURE);
    }
    int cliLen = sizeof(cliAddr);
    int n = recvfrom(sd, msg, MAX_REQUEST_MSG, 0, (struct sockaddr *) &cliAddr, &cliLen);
    std::string stringMsg;
    stringMsg.assign(msg, MAX_REQUEST_MSG * sizeof(char));
    int type = Common::APICommands::GetTypeFromMessage(stringMsg);
    int pid = Common::APICommands::GetPIDFromMessage(stringMsg);
    if (type == GRANT) {
      WriteFile();
    }

    sleep(waitSeconds);

    msgToSend = Common::APICommands::GenerateMessage(RELEASE, (int)getpid());
    if (!SendMessage(msgToSend, remoteServAddr)) {
      close(sd);
      exit(EXIT_FAILURE);
    }
  }
  close(sd);
  exit(EXIT_SUCCESS);
}