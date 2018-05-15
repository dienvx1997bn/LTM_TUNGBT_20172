#pragma once
#include "place.h"

#define DATA_BUFSIZE 2048
#define RECEIVE 0
#define SEND 1

#define NUMB_USERS_MAX 255
#define NUMB_SESS_MAX 255
#define BUFF_SIZE_RESULT 3
#define FILE_NAME "account.txt"
int userIndex = 0;	//number of user have in database
int sessIndex = -1;	//number of session

#define USER 1
#define PASS 2
#define LOUT 3
#define ADDP 4
#define LIST 5
#define LIFR 6
#define TAGF 7
#define NOTI 8
#define UNKN 9
#define MSG_DATA_LENGTH 1000

//construct message Receive
struct message {
	int msgType;
	int length;
	char data[MSG_DATA_LENGTH];
};

//sessions are connecting
struct session {
	char userID[255];	//user id
	SOCKET connSock;	//socket
	sockaddr_in clientAddr;	//client address
	int sessionStatus = 0;	//0-UNIDENT, 1-UNAUTH, 2-AUTH
							//int isOnline = 0;	//1-user is alrealy connected else 0
							//int isConnected = 0;	//1 if session connected before, else 0
	int isConnected = 0;
}sess[NUMB_SESS_MAX];

//construct user data
struct user {
	char userID[255];
	char passWord[255];
	int status;			//0- block, 1- active
}user[NUMB_USERS_MAX];

//data of current userID 
struct currentUser {
	struct user data;
	int numError = 0;
}currentUser[NUMB_SESS_MAX];

//
//int checkSessionConnected(SOCKET connSock);
//int checkAvailUserID(char userID[]);
//void deleteCurrentSession(int idx);
//void getCurrentUser(int idx, char userID[]);
//void deleteCurrentUser(int idx);
//void updateUser(int idx);
//int checkMsgType(int msgType);
//void changeStatusOfSession(int idx, int status);
//void extractInformation(char buff[], message *msg);
//void readWord(FILE *file, char *word);
//void readFile(char *fileName);
//void updateData(char *fileName);
//int findIndex(SOCKET s);

//if this client was connected before then return 1 else return 0
int checkSessionConnected(SOCKET connSock) {
	int i;
	for (i = 0; i <= sessIndex; i++) {
		if (connSock == sess[i].connSock && sess[i].isConnected == 1) {
			return 1;
		}
	}
	return 0;
}

//check userID have in database or not
//IN userID
//return 1 if have userID in database
//else return 0
int checkAvailUserID(char userID[]) {
	int i;
	for (i = 0; i < userIndex; i++) {
		if (strcmp(userID, user[i].userID) == 0) {
			return 1; //if have
		}
	}
	return 0;	//no have
}

//delete data of session[idx], which was disconnected
//IN index of session
void deleteCurrentSession(int idx) {
	strcpy_s(sess[idx].userID, "");
	sess[idx].connSock = 0;
	sess[idx].sessionStatus = 0;
	sess[idx].isConnected = 0;
}

//get information about current user in database and save to currentUser[idx]
void getCurrentUser(int idx, char userID[]) {
	int i;
	for (i = 0; i < userIndex; i++) {
		if (strcmp(userID, user[i].userID) == 0) {
			currentUser[idx].data = user[i];
			break;
		}
	}
}

//delete data of current user which is disconnected 
void deleteCurrentUser(int idx) {
	currentUser[idx].numError = 0;
	strcpy_s(currentUser[idx].data.userID, "");
	strcpy_s(currentUser[idx].data.passWord, "");
	currentUser[idx].data.status = 0;
}
// update information of user 
void updateUser(int idx) {
	int i;
	for (i = 0; i < userIndex; i++) {
		if (strcmp(currentUser[idx].data.userID, user[i].userID) == 0) {
			user[i] = currentUser[idx].data;
			return;
		}
	}
}

//get message type
//return 1-user, 2-pass, 3-logout else return 0
int checkMsgType(int msgType) {
	if(msgType >=0 && msgType < UNKN)
		return msgType;
	else return -1;
}

// change status of current session
//IN index of session
void changeStatusOfSession(int idx, int status) {
	sess[idx].sessionStatus = status;
}

//copy data from buff to struct message
void extractData(char buff[], message *msg) {
	//casting data
	//memcpy(msg, buff, sizeof(message));
	
	msg->msgType = buff[0];
	msg->length = buff[4];
	memcpy(&msg->data, &buff[8], sizeof(message) - 16);
}

//read a word from file
//OUT word
void readWord(FILE *file, char *word) {
	int idx = 0;
	char ch;

	word[idx] = '\0';
	//read a word
	do {
		ch = fgetc(file);	//read a char from file
		if (ch == '\n' || ch == EOF || ch == ' ') {
			if (ch == EOF) word[0] = EOF;
			break;
		}
		word[idx] = ch;
		idx++;
	} while (true);

	word[idx] = '\0';	//end of string 
}

//read user data from file
void readFile(char *fileName) {
	FILE *file;
	//open file to read text
	fopen_s(&file, fileName, "r");
	char word[255];

	if (file == NULL) {
		MessageBox(NULL, L"error", L"error open file", MB_OK);
	}
	else {
		do {	//loop 
				//read userID
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			strcpy_s(user[userIndex].userID, word);
			if (word[0] == EOF)	//end file, then break
			{
				break;
			}
			//read passWord
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			strcpy_s(user[userIndex].passWord, word);
			if (word[0] == EOF)
			{
				break;
			}
			//read status
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			user[userIndex].status = atoi(word);
			userIndex += 1;
			if (word[0] == EOF)
			{
				break;
			}
		} while (true);

		//close file
		fclose(file);
	}
}

// update data 
void updateData(char *fileName) {
	FILE *file;
	fopen_s(&file, fileName, "w+");		//open file to rewrite
	for (int i = 0; i < userIndex; i++) {
		//put userID
		fputs(user[i].userID, file);
		fputc(' ', file);
		//put pass
		fputs(user[i].passWord, file);
		fputc(' ', file);
		//put status
		fprintf(file, "%d", user[i].status);
		if (i != userIndex - 1)
			fputc('\n', file);
		else if (i == userIndex - 1)		//all data was
			break;
	}
	fclose(file);	//close file
}

int findIndex(SOCKET s) {
	int i;
	for (i = 0; i <= sessIndex; i++) {
		if (sess[i].connSock == s) {
			return i;
		}
	}
	return -1;
}

// process if msgType is userID message
// idx index of this session
// return 1 if no error, else return 0
int checkUserID(int idx, char *data, char *out) {

	if (checkAvailUserID(data) == 1)//have user in database
	{

		//get information of current user
		getCurrentUser(idx, data);

		currentUser[idx].numError = 0;	//reset numError = 0;

										//copy userID to session[idx] to know userID is used by a client
		strcpy_s(sess[idx].userID, currentUser[idx].data.userID);

		//check status
		if (currentUser[idx].data.status == 1) {
			changeStatusOfSession(idx, 1);	//change status to nex step is UNAUTH

			memcpy(out, "+01\0", 4);
		}
		else if (currentUser[idx].data.status == 0)//user status == 1 : block
		{
			//delete data
			deleteCurrentUser(idx);
			deleteCurrentSession(idx);

			memcpy(out, "-11\0", 4);
		}

	}
	else if (checkAvailUserID(data) == 0)	//user is not avail
	{
		memcpy(out, "-21\0", 4);
	}

	return 1;
}

//process if PassWord message
// return 1 if no error, else return 0
// idx index of this session
int checkPass(int idx, char *data, char *out) {

	if (strcmp(currentUser[idx].data.passWord, data) == 0) {				//if password true

		if (sess[idx].sessionStatus == 1) {
			changeStatusOfSession(idx, 2);		//change status to nex step is AUTH

			sess[idx].isConnected = 1;		//session is already connected

			memcpy(out, "+02\0", 4);
		}
	}
	else ////if password is worng
	{
		currentUser[idx].numError += 1;		//numError ++
		if (currentUser[idx].numError >= 3) {	//enter wrong password more than 3 times
												//change status of user
			currentUser[idx].data.status = 0;	//block account
			sess[idx].sessionStatus = 0;
			updateUser(idx);

			//change database
			updateData(FILE_NAME);

			//delete data
			deleteCurrentUser(idx);

			memcpy(out, "-22\0", 4);
		}
		else {
			memcpy(out, "-12\0", 4);
		}
	}

	return 1;
}

//process if logout message
// return 1 if no error, else return 0
// idx index of this session
int logOut(int idx, char *data, char *out) {

	if (sess[idx].sessionStatus == 2) {
		changeStatusOfSession(idx, 0);
		deleteCurrentUser(idx);
		memcpy(out, "+03\0", 4);
	}
	else
	{
		memcpy(out, "-13\0", 4);
	}
	
	return 1;
}


// Receive message from client and process
// buff data recv
// out data response
int  process(SOCKET connSock, int idx, char buff[], char *out) {

	message msg;

	extractData(buff, &msg);

	printf("receive from socket %d :  type %d length %d data %s\n", connSock, msg.msgType, msg.length, msg.data);
	
	if (sess[idx].sessionStatus == 0) {
		if (checkMsgType(msg.msgType) == USER ) {
			if (checkSessionConnected(connSock) == 1) {
				memcpy(out, "-41\0", 4);
				return 0;
			}
			else {
				sess[idx].connSock = connSock;
				checkUserID(idx, msg.data, out);
				return 0;
			}
		}
		else if (checkMsgType(msg.msgType) == -1) {
			memcpy(out, "-10\0", 4);
			return 0;
		}
		else {
			memcpy(out, "-20\0", 4);
			return 0;
		}

	}
	else if (sess[idx].sessionStatus == 1) {
		if (checkMsgType(msg.msgType) == PASS) {
			checkPass(idx, msg.data, out);
			return 0;
		}
		else if (checkMsgType(msg.msgType) == -1) {
			memcpy(out, "-10\0", 4);
			return 0;
		}
		else {
			memcpy(out, "-20\0", 4);
			return 0;
		}
	} 
	else if (sess[idx].sessionStatus == 2) {
		if (checkMsgType(msg.msgType == ADDP)) {
		
			place place;
			memcpy(&place, msg.data, sizeof(place));
			printf("name %s longitude %f latitude %f\n", place.name, place.longitude, place.latitude);
			if (checkPlaceInFavoriteList(place.longitude, place.latitude) == 1) {
				memcpy(out, "-14\0", 4);
				return 0;
			}
			else if (place.latitude < 180 && place.longitude < 180) {
				memcpy(out, "+04\0", 4);
				return 0;
			}
			else {
				memcpy(out, "-24\0", 4);
				return 0;
			}
			
		}
		else if (checkMsgType(msg.msgType) == LIST) {

		}
		else if (checkMsgType(msg.msgType) == LIFR) {

		}
		else if (checkMsgType(msg.msgType) == TAGF) {

		}

		if (checkMsgType(msg.msgType) == LOUT) {
			logOut(idx, msg.data, out);
			deleteCurrentSession(idx);
			deleteCurrentUser(idx);
			return 0;
		}
		else if (checkMsgType(msg.msgType) == -1) {
			memcpy(out, "-10\0", 4);
			return 0;
		}
		else {
			memcpy(out, "-20\0", 4);
			return 0;
		}
	}
	
	return 0;
}

//set index for each session
int addNewSession() {
	/*int i;
	for (i = 0; i <= sessIndex; i++)
		if (sess[i].connSock == 0) return i;*/

	sessIndex += 1;
	return sessIndex;
}
