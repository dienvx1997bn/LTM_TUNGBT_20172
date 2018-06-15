
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include "Header.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#define RECEIVE 0
#define SEND 1


// Structure definition
typedef struct {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuff;
	CHAR buffer[DATA_BUFSIZE];
	int bufLen;
	int recvBytes;
	int sentBytes;
	int operation;
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

typedef struct {
	SOCKET socket;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;


unsigned __stdcall serverWorkerThread(LPVOID CompletionPortID);
int  process(SOCKET connSock, int idx, char buff[], Message *msgRep);



int main(int argc, char **argv) { 

	SOCKADDR_IN serverAddr;
	SOCKET listenSock, acceptSock;
	HANDLE completionPort;
	SYSTEM_INFO systemInfo;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	DWORD transferredBytes;
	DWORD flags;
	WSADATA wsaData;
	unsigned short port_number;			/* Port number to use */

	if (WSAStartup((2, 2), &wsaData) != 0) {
		printf("WSAStartup() failed with error %d\n", GetLastError());
		return 1;
	}

	readAccount(FILE_ACC);
	readFavoriteLocation(FILE_LOCATION);

	printf("Server started!\n");


	// Step 1: Setup an I/O completion port
	if ((completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
		printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		return 1;
	}

	// Step 2: Determine how many processors are on the system
	GetSystemInfo(&systemInfo);

	// Step 3: Create worker threads based on the number of processors available on the
	// system. Create two worker threads for each processor	
	for (int i = 0; i < (int)systemInfo.dwNumberOfProcessors * 2; i++) {
		//Create a server worker thread and pass the completion port to the thread
		if (_beginthreadex(0, 0, serverWorkerThread, (void*)completionPort, 0, 0) == 0) {
			printf("Create thread failed with error %d\n", GetLastError());
			return 1;
		}
	
	}

	// Step 4: Create a listening socket
	if ((listenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		printf("WSASocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	port_number = atoi(argv[1]);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port_number);
	if (bind(listenSock, (PSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	// Prepare socket for listening
	if (listen(listenSock, 20) == SOCKET_ERROR) {
		printf("listen() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	sockaddr clientAddr;

	while (1) {
		// Step 5: Accept connections
		if ((acceptSock = WSAAccept(listenSock, &clientAddr, NULL, NULL, 0)) == SOCKET_ERROR) {
			printf("WSAAccept() failed with error %d\n", WSAGetLastError());
			return 1;
		}
		// Step 6: Create a socket information structure to associate with the socket
		if ((perHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}

		// Step 7: Associate the accepted socket with the original completion port
		printf("Socket number %d got connected...\n", acceptSock);
		perHandleData->socket = acceptSock;

		addNewSession(newIndex(), acceptSock);

		if (CreateIoCompletionPort((HANDLE)acceptSock, completionPort, (DWORD)perHandleData, 0) == NULL) {
			printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		return 1;
		}

		// Step 8: Create per I/O socket information structure to associate with the WSARecv call
		if ((perIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}

		ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
		perIoData->sentBytes = 0;
		perIoData->recvBytes = 0;
		perIoData->dataBuff.len = DATA_BUFSIZE;
		perIoData->dataBuff.buf = perIoData->buffer;
		perIoData->operation = RECEIVE;
		flags = 0;

		if (WSARecv(acceptSock, &(perIoData->dataBuff), 1, &transferredBytes, &flags, &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				return 1;
			}
		}
	}
	//_getch();
	return 0;
}

unsigned __stdcall serverWorkerThread(LPVOID completionPortID)
{
	HANDLE completionPort = (HANDLE)completionPortID;
	DWORD transferredBytes;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	DWORD flags;

	Message msgRep;
	char result[DATA_BUFSIZE];
	msgRep.length = 0;
	

	while (TRUE) {
		int resultGetQueuedCompletionStatus = GetQueuedCompletionStatus(completionPort, &transferredBytes,
			(LPDWORD)&perHandleData, (LPOVERLAPPED *)&perIoData, INFINITE);
		int idx = findIndex(perHandleData->socket);
		if (resultGetQueuedCompletionStatus == 0) {
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			deleteCurrentSession(idx);
			deleteCurrentUser(idx);
			//return 0;
			continue;
		}

		// Check to see if an error has occurred on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket
		if (transferredBytes == 0) {
			printf("Closing socket %d\n", perHandleData->socket);
			deleteCurrentSession(idx);
			deleteCurrentUser(idx);

			if (closesocket(perHandleData->socket) == SOCKET_ERROR) {
				printf("closesocket() failed with error %d\n", WSAGetLastError());
				//return 0;
				continue;
			}
			GlobalFree(perHandleData);
			GlobalFree(perIoData);
			continue;
		}
		// Check to see if the operation field equals RECEIVE. If this is so, then
		// this means a WSARecv call just completed so update the recvBytes field
		// with the transferredBytes value from the completed WSARecv() call
		if (perIoData->operation == RECEIVE) {
			perIoData->recvBytes = transferredBytes;
			perIoData->sentBytes = 0;
			perIoData->operation = SEND;

			perIoData->buffer[perIoData->recvBytes] = 0;

			process(perHandleData->socket, idx, perIoData->buffer, &msgRep);

			//msgRep.data[msgRep.length] = 0;
			//printf("lengthresult %d\n", lengthResult);
			//lengthResult = strlen(result);
			
			//memcpy(result, &msgRep, DATA_BUFSIZE);
			//memcpy(result, &msgRep.data, msgRep.length);
			memset(result, 0, DATA_BUFSIZE);
			memcpy(result, msgRep.data, DATA_BUFSIZE);
			msgRep.data[msgRep.length] = 0;
			printf("\socket %d  length %d result \"%s\"\n", perHandleData->socket, msgRep.length, result);

		}
		else if (perIoData->operation == SEND) {
			perIoData->sentBytes += transferredBytes;
		}

		if (perIoData->sentBytes < msgRep.length) {
			// Post another WSASend() request.
			// Since WSASend() is not guaranteed to send all of the bytes requested,
			// continue posting WSASend() calls until all received bytes are sent.
			ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
			perIoData->dataBuff.buf = result + perIoData->sentBytes;
			perIoData->dataBuff.len = msgRep.length - perIoData->sentBytes;
			perIoData->operation = SEND;

			if (WSASend(perHandleData->socket,
				&(perIoData->dataBuff),
				1,
				&transferredBytes,
				0,
				&(perIoData->overlapped),
				NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					printf("WSASend() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			}
			printf("transfer bytes %d\n", transferredBytes); //co dong nay thi k loi nhan du lieu tren client
		}
		else {
			// No more bytes to send post another WSARecv() request
			perIoData->recvBytes = 0;
			perIoData->operation = RECEIVE;
			flags = 0;
			ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
			perIoData->dataBuff.len = DATA_BUFSIZE;
			perIoData->dataBuff.buf = perIoData->buffer;
			if (WSARecv(perHandleData->socket,
				&(perIoData->dataBuff),
				1,
				&transferredBytes,
				&flags,
				&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					printf("WSARecv() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			}

		}

	}
}



//main process
int updateLoationDataLock = 0;
int  process(SOCKET connSock, int idx, char buff[], Message *msgRep) {

	Message msg;
	memset(&msg, 0, sizeof(Message));
	char out[DATA_BUFSIZE];
	int length;

	
	extractData(buff, &msg);
	
	if (msg.length > DATA_BUFSIZE) {

		makeResult("-10", 3, msg, msgRep);
		return 0;
	}

	printf("receive from socket %d :  type %d length %d data %s\n", connSock, msg.msgType, msg.length, msg.data);

	if (checkMsgType(msg.msgType) == UNKN) {			//message can not be defined

		makeResult("-10", 3, msg, msgRep);

		return 0;
	}

	if (sess[idx].sessionStatus == 0) {					//session 0
		if (checkMsgType(msg.msgType) == USER) {
			if (checkUserOnline(msg.data) == 1) {		//if user was online

				makeResult("-31", 3, msg, msgRep);

				return 0;
			}
			else {										//user haven't been logged in yet
				addNewSession(idx, connSock);
				checkUserID(idx, msg.data, out);		

				makeResult(out, 3, msg, msgRep);

				return 0;
			}
		}
		else {											//message can not be defined

			makeResult("-20", 3, msg, msgRep);
			return 0;
		}

	}
	else if (sess[idx].sessionStatus == 1) {			//session 1
		if (checkMsgType(msg.msgType) == PASS) {
			if (checkUserOnline(currentUser[idx].data.userID) == 1) {

				makeResult("-31", 3, msg, msgRep);

				deleteCurrentSession(idx);
				deleteCurrentUser(idx);
				return 0;

			}

			checkPass(idx, msg.data, out);

			makeResult(out, 3, msg, msgRep);

			return 0;
		}
		else {											//message can not be defined

			makeResult("-20", 3, msg, msgRep);

			return 0;
		}
	}	
	else if (sess[idx].sessionStatus == 2) {			//session 1
		if (checkMsgType(msg.msgType == ADDP)) {

			Place place;
			place.latitude = 0;
			place.longitude = 0;

			if (extractPlaceData(&place, msg.data) == 0) {
				makeResult("-10", 3, msg, msgRep);

				return 0;
			}
			

			printf("name %s latitude %f longitude %f\n", place.name, place.latitude, place.longitude);

			if (place.latitude < 180 && place.longitude < 180 && strlen(place.name) < NAME_LENGTH && place.latitude >= 0 && place.longitude >= 0) {
				if (addNewLocation(place.name, place.latitude, place.longitude, sess[idx].userID) == 0) {	//add new favorite lovation

					makeResult("-14", 3, msg, msgRep);

					return 0;
				}

				//update data location
				while (updateLoationDataLock == 1);
				updateLoationDataLock = 1;
				updateLoationData(FILE_LOCATION);
				updateLoationDataLock = 0;

				makeResult("+04", 3, msg, msgRep);

				return 0;
			}
			else {										

				makeResult("-24", 3, msg, msgRep);

				return 0;
			}

		}
		else if (checkMsgType(msg.msgType) == LIST) {
			char result[NUM_FAVORITE_PLACE_MAX][DATA_BUFSIZE];
			int num;
			char dataBuff[DATA_BUFSIZE];

			Location temp[NUM_LOCATION_MAX];
			getListFavoriteLocation(sess[idx].userID, &num, result);

			if (num == 0) {
				makeResult("+05 ", 3,msg, msgRep);
				return 0;
			}
			else {
				Location tempLocation[100];
				Place tempPlace[100];
				for (int i = 0; i < num; i++) {
					memcpy(&tempLocation[i], result[i], sizeof(Location));
					printf("list---- %f %s\n", tempLocation[i].place.latitude, tempLocation[i].place.name);
					memcpy(&tempPlace[i].name, &tempLocation[i].place.name, NAME_LENGTH);
					tempPlace[i].latitude = tempLocation[i].place.latitude;
					tempPlace[i].longitude = tempLocation[i].place.longitude;
				}

				makeResultListFavoriteLocation(out, tempLocation, num);

				makeResult(out, strlen(out), msg, msgRep);

				return 0;
			}
		}
		else if (checkMsgType(msg.msgType) == LIFR) {
			struct MsgListFriend tempData[NUMB_USER_MAX];
			int i, num = 0;
			for (int i = 0; i <= sessIndex; i++) {
				if (currentUser[i].isOnline == 1) {
					memcpy(&tempData[num], sess[i].userID, NAME_LENGTH);
					num++;
				}
			}
			makeResultListFriend(out, tempData, num);
			makeResult(out, strlen(out), msg, msgRep);
			msgRep->length = strlen(out);

			return 0;
		}
		else if (checkMsgType(msg.msgType) == TAGF) {

			MsgTagMessage tempTagMsg;
			MsgTagRequest tempTagReq;

			if (extractTagRequestData(&tempTagReq, msg.data) == 0) {
				makeResult("-10", 3, msg, msgRep);
				return 0;
			}

			if (checkAvailUserID(tempTagReq.recvUser) == 0) {
				makeResult("-27", 3, msg, msgRep);
				return 0;
			}

			memcpy(&tempTagMsg.detail, &tempTagReq, sizeof(MsgTagRequest));
			strcpy(tempTagMsg.sendUser, currentUser[idx].data.userID);

			if (_beginthreadex(0, 0, tagThread, (void*)&tempTagMsg, 0, 0) == 0) {
				makeResult("-17", 3, msg, msgRep);
				return 0;
			}

			makeResult("+07", 3, msg, msgRep);
			return 0;
		}

		else if (checkMsgType(msg.msgType) == LOUT) {
			logOut(idx, msg.data, out);
			deleteCurrentSession(idx);
			deleteCurrentUser(idx);

			makeResult("+03", 3, msg, msgRep);
			return 0;
		}
		else {

			makeResult("-20", 3, msg, msgRep);
			return 0;
		}
	}
	return 0;
}

