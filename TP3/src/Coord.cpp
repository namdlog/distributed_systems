#include "Common.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
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
#include <thread>
#include <mutex>
#include <fstream>

// Global variables
int N, ClientIds;
int* RequestsCounter;
bool Go, first;
std::mutex mtx;
std::ofstream file;

std::deque<int> Requests;
std::map<int, int> PidToClientId, ClientIdToPid;
std::map<unsigned short, int> PortToPid;
std::map<int, sockaddr_in> PidToAddr;

int sd, rc, n, cliLen;
struct hostent *h;
struct sockaddr_in cliAddr, servAddr;
char msg[MAX_REQUEST_MSG];

// Global Methods
void ShowQeueuRequests() {
  if(Requests.size() == 0){
    printf("Empty queue :( \n");
  }
  mtx.lock();
  for(int i=0;i<Requests.size();i++){
    printf("[%d] %d REQUEST.\n",i, Requests[i]);
  }
  mtx.unlock();
}

void ShowCounter() {
  for(int i=0;i<N;i++) {
    printf("[Process %d] %d \n", ClientIdToPid[i], RequestsCounter[i]);
  }
}

bool TerminalManager(){
  int op;
  while(1) {
    printf("--- MENU ---\nEnter a command:\n - 0 : Show requests queue\n - 1 : Show counter\n - 2 : Exit execution\n------------\n");
    scanf("%d",&op);
    if(op == 0){
      ShowQeueuRequests();
    } else if (op == 1){
      ShowCounter();
    } else if (op == 2){
      break;
    }
  }
  return 0;
}

bool SendMessage(std::string msgToSend, struct sockaddr_in remoteServAddr) {
  h = gethostbyname(LOCAL_SERVER_HOST);
  if (h==NULL) {
    printf("%s Unknown host: %s \n", "[ERROR]", LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }

  char * msgToSendReady = const_cast<char*>(msgToSend.c_str());
  rc = sendto(sd, msgToSendReady, MAX_REQUEST_MSG, 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
  if (rc<0) {
    printf("%s Cannot send data to '%s' (IP : %s) \n", "[ERROR]", h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
    return false;
  }

  file << "[" + Common::APICommands::GetTimeNow() + "]" + "[INFO] Sent " + msgToSend + " to '" + h->h_name + "' (IP : " + inet_ntoa(*(struct in_addr *)h->h_addr_list[0]) + ")\n";
  file.flush();
  return true;
}

bool SendGrant(int pid) {
  std::string grantMessage = Common::APICommands::GenerateMessage(GRANT, (int)getpid());
  
  // Process not found
  if(PidToAddr.find(pid) == PidToAddr.end()){
    return false;
  }

  // Map the PID to a ClientId value to show requests counter
  SendMessage(grantMessage, PidToAddr[pid]);
  if (PidToClientId.find(pid) == PidToClientId.end()){
    PidToClientId[pid] = ClientIds;
    ClientIdToPid[ClientIds++] = pid;
  }
  int clientId = PidToClientId[pid];
  RequestsCounter[clientId]++;
  
  // Log grant message
  file << Common::APICommands::LogGrant(pid);
  file.flush();
  return true;
}

void ManageQeueu() {
  while(1){
    while(!Go) { }
    if(!Requests.empty()){

      // Critical zone
      mtx.lock();
      int pidToGrant = Requests.front();
      Requests.pop_front();
      mtx.unlock();

      SendGrant(pidToGrant);
      Go = false;
    } else {
      Go = true;
    }
  }
}

bool AddRequest(int pid) {
  file << Common::APICommands::LogRequest(pid);
  file.flush();
  if (!Requests.empty()) {
    Requests.push_back(pid);
  } else {
    Requests.push_back(pid);
    // Let the first process start the queue management
    if(!first) {
      Go = true;
      first = true;
    }
  }
  return true;
}

bool HandleMessage(int type, int pid) {
  if (type == REQUEST) {
      return AddRequest(pid);
    } else if (type == GRANT){

      //Process not coord. cannot send a Grant
      file << "[" + Common::APICommands::GetTimeNow() + "][ERROR] Received a GRANT message from " + std::to_string(pid) + "\n";
      file.flush();
      return false;
    } else if (type == RELEASE){
      file << Common::APICommands::LogRelease(pid);
      file.flush();
      Go = true;
    } else {
      // Unhandled message
      printf("%s Unknown type %d message from %d.\n", "[ERROR]", type, pid);
      return false;
    }
    return false;
}

bool OpenConnection() {

  /* socket creation */
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd<0) {
    printf("%s: cannot open socket \n",LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }

  int enable = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    printf("setsockopt(SO_REUSEADDR) failed");
  }

  /* bind local server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(REMOTE_SERVER_PORT);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if (rc<0) {
    printf("%s Cannot bind port number %d on %s\n", "[ERROR]", REMOTE_SERVER_PORT, LOCAL_SERVER_HOST);
    return EXIT_FAILURE;
  }

  /* init buffer */
  /* server infinite loop */
  while(1) {

    memset(msg, 0x0, MAX_REQUEST_MSG);

    cliLen = sizeof(cliAddr);
    n = recvfrom(sd, msg, MAX_REQUEST_MSG, 0, (struct sockaddr *) &cliAddr, &cliLen);
    if(n<0) {
      printf("%s Cannot receive data on \n", "[ERROR]", LOCAL_SERVER_HOST);
      continue;
    }

    std::string stringMsg;
    stringMsg.assign(msg, MAX_REQUEST_MSG * sizeof(char));
    int type = Common::APICommands::GetTypeFromMessage(stringMsg);
    int pid = Common::APICommands::GetPIDFromMessage(stringMsg);
    PortToPid[cliAddr.sin_port] = pid;
    PidToAddr[pid] = cliAddr;
    HandleMessage(type, pid);
  }
  close(sd);
  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[]) {
  // Read N
  printf("Please enter the number of process:");
  std::cin >> N;
  RequestsCounter = new (std::nothrow) int[N];

  // Try to allocate memory for the counter
  if (RequestsCounter == nullptr) {
    printf("%s Failed to allocate memory :(\n", "[ERROR]");
    exit(EXIT_FAILURE);
  }

  // Initialize counter values
  for (int i=0;i<N;i++) {
    RequestsCounter[i] = 0;
  }

  // Start log file
  file.open("log.txt",std::ios_base::app);

  // Set threads, run and join them
  std::thread connectionThread(OpenConnection);
  std::thread queueThread(ManageQeueu);
  std::thread terminalThread(TerminalManager);

  connectionThread.join();
  queueThread.join();
  terminalThread.join();

  // Delete pointers and finish execution
  delete RequestsCounter;
  exit(EXIT_SUCCESS);
}