#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define USER_CATEGORY 10		//유저의 행적을 조사할 항목
#define MAX_USERS 1024			//유저의 최대 수

typedef struct UserStats
{
	int trails[USER_CATEGORY];		//유저의 행적 조사값 배열
} UserStats;


void printTrails();
int getUserStats(UserStats* users);


void printTrails()
{
	int count;		//num of users
	UserStats* users = (UserStats*)malloc(sizeof(UserStats)*MAX_USERS);	//유저 정보 구조체 배열
	
	count = getUserStats(users);	//유저들의 행적값들을 파일에서 불러들인다.
	free(users);
}


//유저들의 정보를 파일에서 읽어들이고, 총 유저 수를 반환한다.
int getUserStats(UserStats* users)
{
	FILE* tmpF;
	char* tempFname = "userstats.dat";	//임시 파일명
	int idx = 0;
	int arridx;

	tmpF = fopen(tempFname, "r");

	while(!feof(tmpF))	//파일 끝까지 읽어들인다.
	{
		for(i = 0; i < USER_CATEGORY; i++)
		{
			fscanf(tmpF. "%d ", &((user + idx)->trails[i]));
		|
		idx++;
	}

	fclose(tmpF);

	return idx;
}





