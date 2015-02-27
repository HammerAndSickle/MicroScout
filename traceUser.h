#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_CATEGORY 10		//유저의 행적을 조사할 항목
#define MAX_USERS 1024			//유저의 최대 수
#define MAX_LENGTH 32			//유저의 이름 최대 바이트

typedef struct UserStats
{
	char name[MAX_LENGTH];			//유저의 이름
	int trails[USER_CATEGORY];		//유저의 행적 조사값 배열
} UserStats;


void printTrails();
int getUserStats(UserStats* users);


void printTrails()
{
	int count;		//num of users
	UserStats* users = (UserStats*)malloc(sizeof(UserStats)*MAX_USERS);	//유저 정보 구조체 배열
	int idx;
	
	count = getUserStats(users);	//유저들의 행적값들을 파일에서 불러들인다.
	free(users);

	for(idx = 0; idx < count; idx++)
	{
		printf("%s : %d %d %d %d %d %d %d %d %d %d\n", (users + idx)->name, (users + idx)->trails[0], (users + idx)->trails[1], (users + idx)->trails[2], (users + idx)->trails[3], (users + idx)->trails[4], (users + idx)->trails[5], (users + idx)->trails[6], (users + idx)->trails[7], (users + idx)->trails[8], (users + idx)->trails[9]);
	}
}


//유저들의 정보를 파일에서 읽어들이고, 총 유저 수를 반환한다.
int getUserStats(UserStats* users)
{
	FILE* tmpF;
	char* tempFname = "userstats.dat";	//임시 파일명
	char tempName[MAX_LENGTH];		//잠깐 유저 이름을 저장할 문자열
	int idx = 0;
	int i;

	tmpF = fopen(tempFname, "r");

	while(!feof(tmpF))	//파일 끝까지 읽어들인다.
	{
		fscanf(tmpF, "%s ", tempName);		

		for(i = 0; i < USER_CATEGORY; i++)
		{
			fscanf(tmpF, "%d ", &((users + idx)->trails[i]));
		}

		strcpy((users + idx)->name, tempName);
		idx++;
	}

	fclose(tmpF);

	//정보를 모두 옮겼으니 기록 파일은 삭제
	if(remove(tempFname) == -1) perror("remove");

	return idx;
}





