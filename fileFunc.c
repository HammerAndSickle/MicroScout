//중요도를 계산하는 알고리즘을 구현한 코드

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define FILE_CATEGORY 5
#define MAXIMUM_PATH_LEN 1024	//파일 절대경로명은 최대 1024바이트 문자열.

#define SECTOR1 0		//부문별로 점수 인덱스를 상수로 정의한 것.
#define SECTOR2 1
#define SECTOR3 2
#define SECTOR4 3
#define SECTOR5 4

//==eval1 함수를 위한 상수
#define MODE_START_SCORE 200

//==eval2 함수를 위한 상수
#define BLKS_START_SCORE 50
#define BLKS_WEIGHT 15			//블록 구멍 가산점

//==eval3 함수를 위한 상수
#define HARDLINK_START_SCORE 30
#define HARDLINK_WEIGHT 2		//하드링크 개수 가산점

//==eval4 함수를 위한 시간 다루기 상수들 
#define MINUTE 60		//분, 시간, 일, 주, 달을 초로 표시한 값
#define HOUR (MINUTE*60)
#define DAY (HOUR*24)
#define WEEK (DAY*7)
#define MONTH (WEEK*4)
#define OVER_WEEK_START_SCORE 10
#define OVER_WEEK_OFFSET 10
#define OVER_DAY_START_SCORE 30
#define OVER_DAY_OFFSET 5
#define OVER_HOUR_START_SCORE 66
#define OVER_HOUR_OFFSET 1

//==eval5 함수를 위한 상수
#define ETC_START_SCORE 40
#define ROOT_UID 0			//root uid는 0을 갖는다.

//File의 정보들을 저장하는 데이터구조다.
typedef struct FileStats
{
	char name[MAXIMUM_PATH_LEN];	//파일의 절대경로 이름
	int devNode;			//파일의 디바이스 노드
	int mode;			//파일의 권한
	int hardlinks;			//파일의 하드 링크 개수
	uid_t uid;			//파일의 소유자
	gid_t gid;			//파일의 소유자 그룹
	int size;			//파일의 바이트 크기
	int blksize;			//파일의 블록 크기
	int blocks;			//파일의 할당 블록 개수
	long int atime;			//파일의 최종 사용 시간
	
	float scores[FILE_CATEGORY];	//파일의 중요도 점수들
} FileStats;

//선언된 함수들. eval계열의 함수에 있는 매개변수인 sector는 주어진 값에 따라 score의 인덱스로 쓰인다.

void getFileScores(int count);
void getFileStats(FileStats* files, int count);
void eval1_stat(FileStats* files, int num, int sector);
void eval2_blks(FileStats* files, int num, int sector);
void eval3_hardlinks(FileStats* files, int num, int sector);
void eval4_access_time(FileStats* files, int num, int sector);
void eval5_etc(FileStats* files, int num, int sector);


// 파일의 개수를 받아, 임시 dat 파일에 기록된 파일 정보들을 사용해
// 중요도 값 수치들을 얻어낸다.
void getFileScores(int count)
{
	FileStats* files = (FileStats*)malloc(sizeof(FileStats)*count);	//파일의 정보들이 담기는 구조체 배열
	int idx;

	//파일 정보를 files 구조체 배열에 기록
	getFileStats(files, count);

	eval1_stat(files, count, SECTOR1);		//권한값을 이용해 점수 계산
	eval2_blks(files, count, SECTOR2);		//블록과 용량으로 점수 계산
	eval3_hardlinks(files, count, SECTOR3);		//하드 링크 개수로 점수 계산
	eval4_access_time(files, count, SECTOR4);	//접근 시간으로 점수 계산
	eval5_etc(files, count, SECTOR5);		//소유자, 디바이스 장치 정보를 이용해 점수 계산



	//--------------------테스트용 출력 코드
	for(idx = 0; idx < count; idx++)
	{
		printf("%s : %f %f %f %f %f\n", (files + idx)->name, (files + idx)->scores[0], (files + idx)->scores[1], (files + idx)->scores[2], (files + idx)->scores[3], (files + idx)->scores[4]);
	}

	free(files);
	
}

//filestats.dat에 기록되어 있는 파일의 정보들을 얻어낸다.
void getFileStats(FileStats* files, int count)
{
	FILE* tmpF;				//정보를 불러올 임시 파일
	char* tempFname = "filestats.dat";	//정보를 불러올 임시 파일의 이름
	char tempName[MAXIMUM_PATH_LEN];	//잠깐 파일명을 저장할 곳
	int idx;

	tmpF = fopen(tempFname, "r");

	//파일과 그 정보들을 모두 구조체 배열에 저장한다.
	for(idx = 0; idx < count; idx++)
	{
		fscanf(tmpF, "%s %d %o %d %d %d %d %d %d %u\n", tempName, &((files + idx)->devNode), &((files + idx)->mode), 
 &((files + idx)->hardlinks), &((files + idx)->uid), &((files + idx)->gid), &((files + idx)->size),
&((files + idx)->blksize),&((files + idx)->blocks),&((files + idx)->atime));
		strcpy((files + idx)->name, tempName);
	}



	fclose(tmpF);

}

//권한 값을 중요도로 환산한다.
void eval1_stat(FileStats* files, int num, int sector)
{
	int idx;
	int modeVal;		//mode_t value
	int score;		//score value

	//소유자 권한-그룹멤버 권한-일반사용자 권한
	//권한값이 작을수록 중요한 파일로 간주.
	

	//처음엔 200점으로 시작해 나가며, 권한이 넓을수록 점수가 낮아진다.
	//rwxrwxrwx라면 모두 깎여 0점이 되며, _________라면 200점 그대로 남는다.
	//그러나 sticky bit, setuid, setgid(잠깐 소유자 권한 가짐) 비트가 있다면 점수를 추가한다.
	
	for(idx = 0; idx < num; idx++)
	{
		score = MODE_START_SCORE;	//200점으로 시작
		modeVal = (files + idx)->mode;

		//실행 가능인 경우 : 최대 100점 감점
		if(modeVal & S_IXOTH) score -=50;
		if(modeVal & S_IXGRP) score -=30;
		if(modeVal & S_IXUSR) score -=20;

		//쓰기 가능인 경우 : 최대 70점 감점
		if(modeVal & S_IWOTH) score -=40;
		if(modeVal & S_IWGRP) score -=20;
		if(modeVal & S_IWUSR) score -=10;

		//읽기 가능인 경우 : 최대 30점 감점
		if(modeVal & S_IROTH) score -=15;	
		if(modeVal & S_IRGRP) score -=10;
		if(modeVal & S_IRUSR) score -=5;
		
		//setuid permission
		if(modeVal & 04000) score += 30;

		//setgid permission
		if(modeVal & 02000) score += 20;

		//sticky bit permission
		if(modeVal & 01000) score += 50;

		(files + idx)->scores[sector] = (((float)score) / 300 ) * 100;
		
	}
}

//블럭 개수와 블럭 수, 용량으로 완성도를 평가해 중요도로 환산한다.
void eval2_blks(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = BLKS_START_SCORE;		//50점으로 시작
		
		//block이 0개라면 기본 50점을 그대로 적용.
		//BLKS_WEIGHT는 15점이다. 총 점수는  최소 35점, 최대 65점.
		//blksize * blocks > size일수록 파일에 구멍이 없는 것으로 보고 점수를 높게 부여.
		if((files + idx)->blocks != 0)
		{
			if( ( (files + idx)->blksize * (files + idx)->blocks ) >= (files + idx)->size )
				score +=BLKS_WEIGHT;

			else	score -=BLKS_WEIGHT;
			
		}

		(files + idx)->scores[sector] = (float)score;
		

	}

}

//하드 링크 개수를 중요도로 환산한다.
void eval3_hardlinks(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = HARDLINK_START_SCORE;	//30점으로 시작
		
		//위에서 정의한 가산점만큼 하드링크 개수를 곱하여 더한다.
		score += HARDLINK_WEIGHT * (files + idx)->hardlinks;

		//만일 100점을 넘을 경우, 100점으로 고정시킨다.
		if(score > 100) score = 100;

		(files + idx)->scores[sector] = (float)score;
	}

}

//최종 접근 시간을 중요도로 환산한다.
void eval4_access_time(FileStats* files, int num, int sector)
{
	int idx;
	time_t t;		//현재 시간을 저장할 time_t
	long gap;		//(현재 시간 - 파일의 atime) 차이값
	int score;

	//time_t는 기원(1970/1/1)으로부터 흐른 '초(sec)'들의 값
	
	//중요한 파일은 어떻게든 자주 접근하게 될 가능성이 높다고 가정.

	for(idx = 0; idx < num; idx++)
	{
		score = 0;		//initial value : 0

		gap = ((long)time(&t)) - ((files + idx)->atime);

		
	
		//접근한 지 한 달이 넘은 경우 -> 그대로 0
		if(gap >= MONTH) {}
		
		//접근한 지 1주 이상인 경우 -> 10~30
		else if(gap >= WEEK)
		{
			//(gap/WEEK)의 범위는 1~3
			score = OVER_WEEK_START_SCORE;			//start=10
			score += (gap/WEEK -1)*OVER_WEEK_OFFSET;	//offset=10
		}

		//접근한 지 하루 이상인 경우 -> 35~60
		else if(gap >= DAY)
		{	
			//(gap/DAY)의 범위는 1~6
			score = OVER_DAY_START_SCORE;			//start=30
			score += (gap/DAY)*OVER_DAY_OFFSET;		//offset=5
		}

		//접근한 지 한 시간 이상인 경우 -> 67~90
		else if(gap >= HOUR)
		{	
			//(gap/HOUR)의 범위는 1 이상, 24 미만
			score = OVER_HOUR_START_SCORE; 			//start=66;
			score += (((gap)/HOUR)*OVER_HOUR_OFFSET);		//offset=1
		}
	
		//그 외, 접근한 지 한 시간 이내 -> 100
		else
		{		
			score = 100;
		}

		(files + idx)->scores[sector] = (float)score;

	}

}


//소유자나 디바이스 노드 정보를 사용, 중요도를 얻어낸다.
void eval5_etc(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = ETC_START_SCORE;
		
		//소유자가 root일 경우 점수 증가
		if( (files + idx)->uid == ROOT_UID ) score += 20;

		//소유자 그룹도 root일 경우 점수 증가
		if( (files + idx)->gid == ROOT_UID ) score += 20;

		//이 파일이 물리적인 디바이스에 존재하는가?
		//dev_t에 major를 사용하면 주 번호 12비트 반환
		if( gnu_dev_major((files + idx)->devNode) > 0) score += 20;

		
		(files + idx)->scores[sector] = (float)score;
	}

}


