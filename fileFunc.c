//중요도를 계산하는 알고리즘을 구현한 코드

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FILE_CATEGORY 5
#define MAXIMUM_PATH_LEN 1024

//File의 정보들을 저장하는 데이터구조다.
typedef struct FileStats
{
	char name[MAXIMUM_PATH_LEN];	//파일의 절대경로 이름
	int mode;			//파일의 권한
	int hardlinks;			//파일의 하드 링크 개수
	int uid;			//파일의 소유자
	int size;			//파일의 바이트 크기
	int blksize;			//파일의 블록 크기
	int blocks;			//파일의 할당 블록 개수
	long int atime;			//파일의 최종 사용 시간
	long int scores[FILE_CATEGORY];	//파일의 중요도 점수들
} FileStats;

void getStats(FileStats* files, int count);


// 파일의 개수를 받아, 임시 dat 파일에 기록된 파일 정보들을 사용해
// 중요도 값 수치들을 얻어낸다.
void getScores(int count)
{
	FileStats* files = (FileStats*)malloc(sizeof(FileStats)*count);	//파일의 정보들이 담기는 구조체 배열

	//파일 정보를 files 구조체 배열에 기록
	getStats(files, count);

	//funct1();
	//funct2();
	//funct3();
	//funct4();
	//funct5();
	

	free(files);
	
}

//filestats.dat에 기록되어 있는 파일의 정보들을 얻어낸다.
void getStats(FileStats* files, int count)
{
	FILE* tmpF;				//정보를 기록할 임시 파일
	char* tempFname = "filestats.dat";	//정보를 기록할 임시 파일의 이름
	char tempName[MAXIMUM_PATH_LEN];	//잠깐 파일명을 저장할 곳
	int idx;

	tmpF = fopen(tempFname, "r");

	//파일과 그 정보들을 모두 구조체 배열에 저장한다.
	for(idx = 0; idx < count; idx++)
	{
		fscanf(tmpF, "%s %o %d %d %d %d %d %u\n", tempName, &((files + idx)->mode), 
 &((files + idx)->hardlinks), &((files + idx)->uid), &((files + idx)->size),
&((files + idx)->blksize),&((files + idx)->blocks),&((files + idx)->atime));
		strcpy((files + idx)->name, tempName);
	}



	fclose(tmpF);

}


