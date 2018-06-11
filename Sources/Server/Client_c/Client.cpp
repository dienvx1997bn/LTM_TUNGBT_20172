#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<winsock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

#define DATA_BUFSIZE 4000
#define NAME_LENGTH 255

#define USER 1
#define PASS 2
#define LOUT 3
#define ADDP 4
#define LIST 5
#define LIFR 6
#define TAGF 7
#define NOTI 8
#define UNKN 9
#define MSG_DATA_LENGTH 2000

int sessionStatus;	//0-UNIDENT, 1-UNAUTH, 2-AUTH

//construct message 
struct message {
	int msgType;
	int length;
	char data[DATA_BUFSIZE - 8];
}msg;

struct place {
	float longitude;
	float latitude;
	char name[NAME_LENGTH];
}place;

//wrapper function send()
int Send(SOCKET s, char *buff, int size, int flag) {
	int n;

	n = send(s, buff, size, flag);
	if (n == SOCKET_ERROR) {
		printf("Send message error: %d\n", WSAGetLastError());
	}
	return n;
}

//wrapper function recv()
int Receive(SOCKET s, char *buff, int size, int flag) {
	int n;

	n = recv(s, buff, size, flag);
	if (n == SOCKET_ERROR) {
		printf("Receive message error: %d\n", WSAGetLastError());
	}
	return n;
}

// type data
// return 1 if OK, 0 if have error
int enterData() {
	printf("type:\t");

	char word[DATA_BUFSIZE];
	gets_s(word);
	if (word[0] == '\0') {
		return 0;
	}
	char *data;
	strtok_s(word, " ", &data);
	
	char *msgType = word;
	
	if (strcmp(msgType, "user") == 0) msg.msgType = USER;
	else if (strcmp(msgType, "pass") == 0) msg.msgType = PASS;
	else if (strcmp(msgType, "lout") == 0) msg.msgType = LOUT;
	else if (strcmp(msgType, "addp") == 0) msg.msgType = ADDP;
	else if (strcmp(msgType, "list") == 0) msg.msgType = LIST;
	else if (strcmp(msgType, "lifr") == 0) msg.msgType = LIFR;
	else msg.msgType = UNKN;

	strcpy_s(msg.data, data);
	msg.length = strlen(data);
	return 1;
}

//check if have error or not
//return 1 if have error, 0 if OK
int isError(char *buff) {
	if (buff[0] == '+') {
		return 0;
	}
	else if (buff[0] == '-')
	{
		return 1;
	}
}

//show error details
void errorDetail(char *buff) {
	if (strcmp(buff, "-10") == 0)
	{
		printf("message type can not identified\n");
	}
	else if (strcmp(buff, "-20") == 0) {
		printf("wrong sequence\n");
	}
	else if (strcmp(buff, "-11") == 0) {
		printf("user is blocked\n");
	}
	else if (strcmp(buff, "-21") == 0)
	{
		printf("user is not avail\n");
	}
	else if (strcmp(buff, "-31") == 0)
	{
		printf("user was already connected\n");
	}
	else if (strcmp(buff, "-41") == 0)
	{
		printf("client connected before\n");
	}
	else if (strcmp(buff, "-12") == 0)
	{
		printf("password is wrong\n");
	}
	else if (strcmp(buff, "-22") == 0)
	{
		printf("enter password fail more than 3 times, user will be blocked\n");
	}
	else if (strcmp(buff, "-13") == 0)
	{
		printf("Account isn't logged in\n");
	}
	else if (strcmp(buff, "-14") == 0)
	{
		printf("This place was in your favorite places list\n");
	}
	else
	{
		printf("Error\n");
	}

}


void changeStatusSession(int num) {
	sessionStatus = num % 3;
}


int main(int argc, char **argv) {
	if (argc < 3) {
		printf("not enough aguments\n");
		_getch();
		return 0;
	}
	unsigned short port_number;			/* Port number to use */

										//step 1: Initiate Winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Version is not supported\n");
	}

	//step 2: Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// (optional) Set time-out for receiving
	int tv = 10000; //Time-out interval: 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//step 3: Specify server address
	port_number = atoi(argv[2]);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_number);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	//step 4: Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error! Cannot connect server. %d\n", WSAGetLastError());
		_getch();
		return 0;
	}

	printf("Connected server! \n");
	//connect to server
	int ret;
	static char buff[DATA_BUFSIZE];
	char *data;

	//intit msg_type 
	sessionStatus = 0;
	strcpy(place.name, "my house");
	place.longitude = 12.34;
	place.latitude = 56.789;
	//loop
	while (true)
	{
		
		data = (char*)malloc(sizeof(message));
		if (enterData() == 0) {
			printf("not enough arguments\n");
			continue;
		}

		if (msg.msgType == ADDP) {
			msg.length = strlen(place.name);
			strcpy_s(msg.data, (char *)&place);
			printf("name %s lat %f long %f\n", place.name, place.latitude, place.longitude);
		}
		if (msg.msgType == LIST) {
			msg.length = 3;
			strcpy_s(msg.data, "abc");
		}



		//convert data type
		memcpy(data, &msg, sizeof(message));

		//printf("message\t %d %d %s\n", msg.msgType, msg.length, msg.data);

		//send messagee
		ret = Send(client, data, sizeof(message), 0);
		if (ret < 0) {
			return 0;
		}
		//release memory
		free(data);
		//printf("buff %s ", buff);

		//receive message
		ret = Receive(client, &buff[0], DATA_BUFSIZE, 0);
		if (ret < 0) {
			return 0;
		}
		message msgRep;
		memcpy(&msgRep, buff, DATA_BUFSIZE);
		printf("msgRep:type %d length %d dataResult %s\n", msgRep.msgType, msgRep.length, msgRep.data);
		if (msgRep.msgType == LIST) {
			struct place temp[10];
			memcpy(temp, msgRep.data, msgRep.length);
			int num = msgRep.length / sizeof(place);
			int i;
			for (i = 0; i < num; i++) {
				printf("place   %s %f %f\n", temp[i].name, temp[i].latitude, temp[i].longitude);
			}
		}
		else if (msg.msgType == LIFR) {
			char name[100][NAME_LENGTH];
			memcpy(name, msgRep.data, msgRep.length);
			int num = msgRep.length / NAME_LENGTH;
			int i;
			for (i = 0; i < num; i++) {
				printf("name --%s \n", name[i]);
			}
		}
		//printf("ret = %d   ",ret);
		//buff[ret] = '\0';
		//if (ret > 3) {
		//	if (msg.msgType == LIST) {
		//		struct place temp[10];
		//		memcpy(temp, buff, ret);
		//		int num = ret / sizeof(place);
		//		int i;
		//		for (i = 0; i < num; i++) {
		//			printf("place   %s %f %f\n", temp[i].name, temp[i].latitude, temp[i].longitude);
		//		}
		//	}
		//	if (msg.msgType == LIFR) {
		//		char name[100][NAME_LENGTH];
		//		memcpy(name, buff, ret);
		//		int num = ret / NAME_LENGTH;
		//		int i;
		//		for (i = 0; i < num; i++) {
		//			printf("name --%s \n", name[i]);
		//		}
		//	}
		//}
		//else {
		//	printf("receive %s\n", buff);
		//} 
		////buff[3] = '\0';

		
		if (strcmp(msgRep.data, "+03") == 0) break;
		if (isError(msgRep.data) == 1) {
			errorDetail(msgRep.data);
		}
		//memset(&buff[0], 0, DATA_BUFSIZE);

	}

	//step 6: Close socket
	closesocket(client);

	//step 7: Terminate Winsock
	WSACleanup();
	return 0;
}