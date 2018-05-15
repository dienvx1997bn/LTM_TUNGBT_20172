#define MSG_DATA_LENGTH 1000


struct place {
	float longitude;
	float latitude;
	char name[MSG_DATA_LENGTH - 16];
};


//  
int checkPlaceInFavoriteList(float longitude, float latitude) {
	return 0;
}

