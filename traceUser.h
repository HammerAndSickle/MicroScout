#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_CATEGORY 10		//유저의 행적을 조사할 항목
#define MAX_USERS 1024			//유저의 최대 수
#define MAX_LENGTH 32			//유저의 이름 최대 바이트

#define PERLS 0				//유저의 행동들을 의미하는 상수이며, 배열 인덱스로도 쓰임.
#define SUDOES 1
#define DEBUGGERS 2
#define EDITORS 3
#define COMPILERS 4
#define BROWSERS 5
#define NETWORKS 6
#define ENVIRONS 7
#define KILLERS 8
#define WHOES 9

#define USER_ERROR -1

typedef struct UserStats
{
	char name[MAX_LENGTH];			//유저의 이름
	int trails[USER_CATEGORY];		//유저의 행적 조사값 배열
} UserStats;


int getUserStats(UserStats* users);
void eval_print(UserStats* users, int num);
int searchMAX(UserStats* user);
void getSentences(UserStats* user, int idx);
void traceUserSTART();
void traceUserEND(UserStats* users);

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

//얻은 정보를 이용해 각 사람의 행적을 간단히 평가
void eval_print(UserStats* users, int num)
{
	int idx;
	int traceIdx;		//가장 빈도가 높은 행동을 의미하는 인덱스를 저장할 것이다.

	//예를 들어, '갈갈이' 라는 유저가 디버거들을 많이 쓰는 유저라면, DEBUGGERS(2)를 저장할 것이다.

	for(idx = 0; idx < num; idx++)
	{
		traceIdx = searchMAX(users + idx);	//가장 높은 빈도수의 행동값을 저장, 모두 0이라면 -1를 반환.		
		getSentences(users + idx, traceIdx);
	}
}

//유저 배열 중, 한 유저를 받아 그 유저의 행동값 USER_CATEGORY개 중 (처음 디폴트 값은 10개) 최대값을 찾는다.
//그 최대값을 갖는 maxIdx의 인덱스를 반환한다. 최대값이 0이면 -1을 반환한다.
int searchMAX(UserStats* user)
{
	int max = 0;
	int idx, maxIdx;		//idx는 반복문 용 iteration 값이고, 최대값이 나오게 되면 그 값의 idx를 maxIdx에 저장한다.
	
	for(idx = 0; idx < USER_CATEGORY; idx++)
	{
		if( (user)->trails[idx] > max)
		{
			max = (user)->trails[idx];
			maxIdx = idx;
		}
	}

	return (max == 0)? USER_ERROR : maxIdx;
}

//얻은 인덱스에 따라 출력되는 문장이 달라진다.
void getSentences(UserStats* user, int idx)
{
	switch(idx)
	{
		case PERLS:
			printf("[%s]는 버퍼 오버플로우 공격을 시도하는 잠재적 범죄자 같다.\n", (user)->name);
			break;
		case SUDOES:
			printf("[%s]는 멀쩡한 자기 아이디는 안 쓰고 자꾸 남의 계정을 쓰려고 한다.\n", (user->name));
			break;
		case DEBUGGERS:
			printf("[%s]는 디버깅을 열심히 하고 있다. 아니면 리버스 엔지니어링을 시도하는 해커일수도 있다..\n", (user->name));
			break;
		case EDITORS:
			printf("[%s]는 에디터 프로그램을 열심히 쓰는 걸 보니 코딩에 몰두하는 듯 하다.\n", (user->name));
			break;
		case COMPILERS:
			printf("[%s]는 컴파일을 자주 한다. 코딩실수를 많이 하는 사용자일지도 모른다.\n", (user)->name);
			break;
		case BROWSERS:
			printf("[%s]는 인터넷 브라우저를 자주 쓴다. 무인도에 가면 답답해 죽을 사람이다.\n", user->name);
			break;
		case NETWORKS:
			printf("[%s]는 네트워크를 자주 다룬다. 라우팅도 아이피도 자주 확인한다.\n", user->name);
			break;
		case ENVIRONS:
			printf("[%s]는 환경변수를 자주 쓴다. 시스템 엔지니어거나 해커 좀도둑인 듯 하다.\n", user->name);
			break;
		case KILLERS:
			printf("[%s]는 프로세스를 자주 죽이는 프로세스 살인마다.\n", user->name);
			break;
		case WHOES:
			printf("[%s]는 자꾸 다른 사용자들에 관심이 많다..\n", user->name);
			break;
		case USER_ERROR:
			printf("[%s]는 접속해놓고 아무 것도 한 적이 없는 것 같다.\n", user->name);
			break;
		default:
			perror("usertrail");
			break;
	}

	printf("\n");
}

//함수의 시작. 문장 출력
void traceUserSTART()
{
	printf("\n\n\n@==================== TRACING USERS... ===================@\n\n\n\n");
}

//함수의 마지막. 메모리 반환 후, 문장 출력
void traceUserEND(UserStats* users)
{
	free(users);

	printf("\n\n\n@======================= FINISHED =========================@\n\n");
}


