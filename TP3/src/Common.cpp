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

#include "Common.h"

namespace Common {

	//Log messages
	std::string APICommands::GetTimeNow(){
		return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	}

	std::string APICommands::LogRequest(int id) {
		std::string timeNow = GetTimeNow();
		return "[" + timeNow + "][INFO] " + std::to_string(id) + " REQUEST.\n";
	}

	std::string APICommands::LogGrant(int id) {
		std::string timeNow = GetTimeNow();
		return "[" + timeNow + "][INFO] " + std::to_string(id) + " GRANTED.\n";
	}

	std::string APICommands::LogRelease(int id) {
		std::string timeNow = GetTimeNow();
		return "["+ timeNow + "][INFO] " + std::to_string(id) + " RELEASED.\n";
	}

	int APICommands::GetPIDFromMessage(std::string msg) {
		int pid = stoi(msg.substr(3,10));
		return pid;
	}

	int APICommands::GetTypeFromMessage(std::string msg) {
		int type = stoi(msg.substr(0,2));
		return type;
	}

	std::string APICommands::LogMessage(char * msgReceived, struct sockaddr_in cliAddr) {
		std::string msg = msgReceived;
		return " Received " + msg + " from " + inet_ntoa(cliAddr.sin_addr) + ":UDP " + std::to_string(ntohs(cliAddr.sin_port)) + "\n";
	}

	// Generate message from type and PID values
	std::string APICommands::GenerateMessage(int t, int id){
		std::string typeString = std::to_string(t);
		if(t < 10){
			typeString = '0' + typeString;
		}
		std::string idString = std::to_string(id);
		int zeroPadLeft = 10 - idString.size();
		std::string msg = std::string(zeroPadLeft, '0');
		msg = typeString + '|' + msg + idString;
		return msg.data();
	}
}