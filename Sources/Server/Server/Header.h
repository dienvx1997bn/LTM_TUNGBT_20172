#pragma once
#include "FileIO.h"


#define DATA_BUFSIZE 4000

#define NUMB_USER_MAX 500
#define NUMB_SESS_MAX 500
#define NUM_LOCATION_MAX 100
#define NUM_FAVORITE_PLACE_MAX 100
#define NAME_LENGTH 255
#define FILE_ACC "account.txt"
#define FILE_LOCATION "location.txt"
int userIndex = 0;	//number of user have in database
int sessIndex = 0;	//number of session
int numLocation = 0;


#define USER 1
#define PASS 2
#define LOUT 3
#define ADDP 4
#define LIST 5
#define LIFR 6
#define TAGF 7
#define NOTI 8
#define UNKN 9

//construct message 
struct message {
	int msgType;
	int length;
	char data[DATA_BUFSIZE - 8];
};

//sessions are connecting
struct session {
	char userID[NAME_LENGTH];	//user id
	SOCKET connSock;	//socket
	sockaddr_in clientAddr;	//client address
	int sessionStatus = 0;	//0-UNIDENT, 1-UNAUTH, 2-AUTH
							//int isOnline = 0;	//1-user is alrealy connected else 0
							//int isConnected = 0;	//1 if session connected before, else 0
	int isConnected = 0;
}sess[NUMB_SESS_MAX];

//construct user data
struct user {
	char userID[NAME_LENGTH];
	char passWord[NAME_LENGTH];
	int status;			//0- block, 1- active
}user[NUMB_USER_MAX];

//data of current userID 
struct currentUser {
	struct user data;
	int numError = 0;
	int isOnline = 0;
}currentUser[NUMB_SESS_MAX];

//chua thong tin dia diem yeu thich cua tat ca user trong csdl
struct location {
	char name[NAME_LENGTH];
	float latitude;
	float longitude;
	char userID[NAME_LENGTH];
}locat[NUM_LOCATION_MAX];

// cau truc thong diep de user them dia diem yeu thich
struct place {
	float longitude;
	float latitude;
	char name[NAME_LENGTH];
};

struct msgListFriend {
	char name[NAME_LENGTH];
};

struct ListTag {
	struct place place;
	char recvUser[NAME_LENGTH];
}listTag[NUMB_USER_MAX];

void increaseNumlocation();

void readAccount(char *fileName) {
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
			if (word[0] == EOF)	//end file, then break
			{
				break;
			}
			strcpy_s(user[userIndex].userID, word);
			//read passWord
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			strcpy_s(user[userIndex].passWord, word);

			//read status
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			user[userIndex].status = atoi(word);
			userIndex += 1;

		} while (true);

		//close file
		fclose(file);
	}
}

// update data 
void updateAccountData(char *fileName) {

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
	fputc('\n', file);
	fclose(file);	//close file
}


//read favorite location
void readFavoriteLocation(char *fileName) {
	FILE *file;
	//open file to read text
	fopen_s(&file, fileName, "r");
	char word[255];

	if (file == NULL) {
		MessageBox(NULL, L"error", L"error open file", MB_OK);
	}
	else {
		do {	//loop 
				//read location name
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			strcpy_s(locat[numLocation].name, word);
			if (word[0] == EOF)	//end file, then break
			{
				break;
			}

			//read lat
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			locat[numLocation].latitude = atof(word);

			//read long
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			locat[numLocation].longitude = atof(word);

			//read userID who like 
			readWord(file, word);
			if (word[0] == '\0') {
				continue;
			}
			strcpy_s(locat[numLocation].userID, word);
			if (word[0] == EOF)
			{
				break;
			}

			increaseNumlocation();

			if (word[0] == EOF)
			{
				break;
			}
		} while (true);

		//close file
		fclose(file);
	}
}

void updateLoationData(char *fileName) {
	
	FILE *file;
	fopen_s(&file, fileName, "w+");		//open file to rewrite
	for (int i = 0; i < numLocation; i++) {

		fputs(locat[i].name, file);
		fputc(' ', file);

		fprintf(file, "%f", locat[i].latitude);
		fputc(' ', file);

		fprintf(file, "%f", locat[i].longitude);
		fputc(' ', file);

		fputs(locat[i].userID, file);

		fputc('\n', file);

	}
	fclose(file);	//close file
}


int newIndex() {
	int i;
	for (i = 0; i <= sessIndex; i++)
		if (sess[i].connSock == 0) {
		return i;
		}

	sessIndex = sessIndex + 1;
	return sessIndex;
}

void addNewSession(int idx, SOCKET connSock) {
	sess[idx].connSock = connSock;
}

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

int checkUserOnline(char userID[]) {
	int i;
	for (i = 0; i < userIndex; i++) {
		if (strcmp(userID, currentUser[i].data.userID) == 0) {
			if(currentUser[i].isOnline == 1)
				return 1; //if have
		}
	}
	return 0;	//no have
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
	currentUser[idx].isOnline = 0;
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
	else return UNKN;
}

// change status of current session
//IN index of session
void changeStatusOfSession(int idx, int status) {
	sess[idx].sessionStatus = status;
}

//copy data from buff to struct message
void extractData(char buff[], message *msg) {
	/*
	msg->msgType = buff[0];
	msg->length = buff[4];
	memcpy(&msg->data, &buff[8], sizeof(message) - 8);
	*/

	char *data; // data la phan du lieu 	

	strtok_s(buff, " ", &data);

	char *msgType = buff;
	
	msg->length = strlen(data);
	strcpy_s(msg->data, data);
	if (data == NULL) {
		strcpy_s(msg->data, "\n");
	}
	int i = 0;
	while (msgType[i]) {
		msgType[i] = tolower(msgType[i]);
		i++;
	}

	if (strcmp(msgType, "user") == 0) msg->msgType = USER;
	else if (strcmp(msgType, "pass") == 0) msg->msgType = PASS;
	else if (strcmp(msgType, "lout") == 0) msg->msgType = LOUT;
	else if (strcmp(msgType, "addp") == 0) msg->msgType = ADDP;
	else if (strcmp(msgType, "list") == 0) msg->msgType = LIST;
	else if (strcmp(msgType, "lifr") == 0) msg->msgType = LIFR;
	else if (strcmp(msgType, "tagf") == 0) msg->msgType = TAGF;
	else msg->msgType = UNKN;
}

int extractPlaceData(place *place, char data[]) {
	char *placeName;
	char *lat;
	char *lng;
	char *err;

	char  delims[] = "|\t\n";
	int count = 0;
	char* context = NULL;



	placeName = strtok_s(data, delims, &context);
	if (placeName == NULL) return 0;
	strcpy_s(place->name, placeName);

	lat = strtok_s(NULL, delims, &context);
	if (lat == NULL) return 0;
	place->latitude = atof(lat);

	lng = strtok_s(NULL, delims, &context);
	if (lng == NULL) return 0;
	place->longitude = atof(lng);

	err = strtok_s(NULL, delims, &context);
	if (err != NULL) return 0;

	return 1;
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

			memcpy(out, "+01\0", 3);
		}
		else if (currentUser[idx].data.status == 0)//user status == 1 : block
		{
			//delete data
			deleteCurrentUser(idx);
			deleteCurrentSession(idx);

			memcpy(out, "-11\0", 3);
		}

	}
	else if (checkAvailUserID(data) == 0)	//user is not avail
	{
		memcpy(out, "-21\0", 3);
	}

	return 1;
}

//process if PassWord message
// return 1 if no error, else return 0
// idx index of this session
int updateAccountDataLock = 0;
int checkPass(int idx, char *data, char *out) {

	if (strcmp(currentUser[idx].data.passWord, data) == 0) {				//if password true

		if (sess[idx].sessionStatus == 1) {
			changeStatusOfSession(idx, 2);		//change status to nex step is AUTH

			sess[idx].isConnected = 1;		//session is already connected
			currentUser[idx].isOnline = 1;
			memcpy(out, "+02\0", 3);
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
			while (updateAccountDataLock == 1);
			updateAccountDataLock = 1;
			updateAccountData(FILE_ACC);
			updateAccountDataLock = 0;
			//delete data
			deleteCurrentUser(idx);

			memcpy(out, "-22\0", 3);
		}
		else {
			memcpy(out, "-12\0", 3);
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
		memcpy(out, "+03\0", 3);
	}
	else
	{
		memcpy(out, "-13\0", 3);
	}
	
	return 1;
}

void increaseNumlocation() {
	numLocation += 1;
}


//get list favorite
void getListFavoriteLocation(char userID[],int *num, char result[][DATA_BUFSIZE]) {
	int i = 0;
	int count = 0;
	for (i = 0; i < numLocation; i++) {
		if (strcmp(userID, locat[i].userID) == 0) {
			//printf("num %d %s %f %f %s\n", num, locat[i].name, locat[i].latitude, locat[i].longitude, locat[i].name);
			memcpy(result[count], &locat[i], sizeof(location));
			count++;
		}
	}
	*num = count;
}

//add new favorite location
int addNewLocation(char name[], float latitude, float longitude, char userID[]) {

	int i;
	for (i = 0; i < numLocation; i++) {
		if (locat[i].latitude == latitude && locat[i].longitude == longitude && strcmp(userID, locat[i].userID) == 0) //this place is existed in list
			return 0;
	}
	
	int c = numLocation;
	for (i = 0; i < strlen(name); i++) {
		if (name[i] == ' ') {
			name[i] = '_';
			break;
		}
	}

	strcpy_s(locat[c].name, name);
	locat[c].latitude = latitude;
	locat[c].longitude = longitude;
	strcpy_s(locat[c].userID, userID);
	increaseNumlocation();
	return 1;
};


void addNewTagLocation(char sendUser[], char recvUser[], place place) {
	int i;
	for (i = 0; i < NUMB_USER_MAX; i++) {
		if (listTag[i].recvUser == NULL) {
			//copy data
			memcpy(&listTag[i].recvUser, recvUser, NAME_LENGTH);
			memcpy(&listTag[i].place, &place, sizeof(struct place));
		}
	}
}
void makeResult(char errorCode[],int length,message msg, message *msgRep) {
	msgRep->length = length;
	msgRep->msgType = checkMsgType(msg.msgType);
	memcpy(msgRep->data, errorCode, length);
	msgRep->data[length] = '\0';
}

void makeResultListFriend(char *out, msgListFriend tempData[], int size) {
	out[0] = NULL;
	strcat(out,"+06 ");
	int i = 0;
	for (i = 0; i < size-1; i++) {
		strcat(out, tempData[i].name);
		strcat(out, "|");
	}
	strcat(out, tempData[i].name);
	
}

void makeResultList(char *out, struct location dataLocation[], int size) {
	out[0] = NULL;
	strcat(out, "+05 ");
	char buff[5];
	int i = 0;
	for (i = 0; i < size - 1; i++) {
		strcat(out, dataLocation[i].name);
		strcat(out, "|");
		_itoa(dataLocation[i].latitude, buff, 10);
		strcat(out, buff);
		strcat(out, "|");
		_itoa(dataLocation[i].longitude, buff, 10);
		strcat(out, buff);
		strcat(out, "$");
	}
	strcat(out, dataLocation[i].name);
	strcat(out, "|");
	_itoa(dataLocation[i].latitude, buff, 10);
	strcat(out, buff);
	strcat(out, "|");
	_itoa(dataLocation[i].longitude, buff, 10);
	strcat(out, buff);
	strcat(out, "$");
}


//main process
int updateLoationDataLock = 0;
int  process(SOCKET connSock, int idx, char buff[], message *msgRep) {
	
	message msg;
	char out[DATA_BUFSIZE]; 
	int length;

	extractData(buff, &msg);
	if (msg.length > DATA_BUFSIZE) {
		
		makeResult("-10", 3, msg, msgRep);
		return 0;
	}
	//printf("index %d\n", idx);
	printf("receive from socket %d :  type %d length %d data %s\n", connSock, msg.msgType, msg.length, msg.data);
	
	if (checkMsgType(msg.msgType) == UNKN) {
		
		makeResult("-10", 3, msg, msgRep);

		return 0;
	}

	if (sess[idx].sessionStatus == 0) {
		if (checkMsgType(msg.msgType) == USER ) {
			if (checkUserOnline(msg.data) == 1) {
				
				makeResult("-31", 3, msg, msgRep);

				return 0;
			}
			else {
				sess[idx].connSock = connSock;
				checkUserID(idx, msg.data, out);
				
				makeResult(out, 3, msg, msgRep);

				return 0;
			}
		}
		else {
			
			makeResult("-20", 3, msg, msgRep);

			return 0;
		}

	}
	else if (sess[idx].sessionStatus == 1) {
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
		else {
			
			makeResult("-20", 3, msg, msgRep);

			return 0;
		}
	} 
	else if (sess[idx].sessionStatus == 2) {
		if (checkMsgType(msg.msgType == ADDP)) {
		
			place place;
			place.latitude = 0;
			place.longitude = 0;
			
			//giai nen msg.data o day
			//memcpy(&place, msg.data, sizeof(place));
			if (extractPlaceData(&place, msg.data) == 0) {
				makeResult("-10", 3, msg, msgRep);

				return 0;
			}
			//

			printf("name %s latitude %f longitude %f\n", place.name, place.latitude, place.longitude);

			if (place.latitude < 180 && place.longitude < 180 || strlen(place.name) > NAME_LENGTH || place.latitude == 0 || place.longitude == 0) {
				if (addNewLocation(place.name, place.latitude, place.longitude, sess[idx].userID) == 0) {
					
					makeResult("-14", 3, msg, msgRep);

					return 0;
				}
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
			
			location temp[NUM_LOCATION_MAX];
			getListFavoriteLocation(sess[idx].userID, &num, result);
			location tempLocation[100];
			place tempPlace[100];
			for (int i = 0; i < num; i++) {
				memcpy(&tempLocation[i], result[i], sizeof(location));
				printf("list---- %f %s\n", tempLocation[i].latitude, tempLocation[i].name);
				memcpy(&tempPlace[i].name, &tempLocation[i].name,NAME_LENGTH);
				tempPlace[i].latitude = tempLocation[i].latitude;
				tempPlace[i].longitude = tempLocation[i].longitude;
			}
			
			//memcpy(out, &tempPlace, num * sizeof(place));
			//length = num * sizeof(place);
			makeResultList(out, tempLocation, num);
			
			makeResult(out, strlen(out), msg, msgRep);

			return 0;
		}
		else if (checkMsgType(msg.msgType) == LIFR) {
			struct msgListFriend tempData[NUMB_USER_MAX];
			int i;
			for (int i = 0; i < userIndex; i++) {
				memcpy(&tempData[i], user[i].userID, NAME_LENGTH);
			}
			makeResultListFriend(out, tempData,userIndex);
			makeResult(out, strlen(out), msg, msgRep);
			msgRep->length = strlen(out);

			return 0;
		}
		else if (checkMsgType(msg.msgType) == TAGF) {
			ListTag temp;
			memcpy(&temp, msg.data, sizeof(ListTag));
			printf("tag friend: recvUser %s lat %f long %f\n", temp.recvUser, temp.place.latitude, temp.place.longitude);
			/*
			int i;
			for (i = 0; i < NUMB_USER_MAX; i++) {
				if (listTag[i].recvUser == NULL) {
					memcpy(&listTag[i], &temp, sizeof(ListTag));
				}
			}*/

			/*memcpy(out, "+07\0", 4);
			length = 3;
			msgRep->length = length;
			msgRep->msgType = checkMsgType(msg.msgType);
			memcpy(msgRep->data, out, length);*/
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

