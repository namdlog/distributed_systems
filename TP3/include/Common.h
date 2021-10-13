#include <arpa/inet.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
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
#include <sys/types.h>
#include <unistd.h>

// API TYPE MESSAGES
#define MAX_REQUEST_MSG 0xE
#define REQUEST         0x1
#define GRANT           0x2
#define RELEASE         0x3

// SOCKET INFO
#define REMOTE_SERVER_PORT 8084
#define LOCAL_SERVER_HOST "127.0.0.1"

namespace Common {

  class APICommands {
  public:
    static std::string GetTimeNow();
    static std::string LogRequest(int x);
    static std::string LogGrant(int x);
    static std::string LogRelease(int x);
    static std::string LogMessage(char * msgReceived, struct sockaddr_in cliAddr);
    static int  GetPIDFromMessage(std::string msg);
    static int  GetTypeFromMessage(std::string msg);
    static std::string GenerateMessage(int t, int id);
  };

}