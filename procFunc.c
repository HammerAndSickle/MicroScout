//중요도를 계산하는 알고리즘을 구현한 코드

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define PROC_CATEGORY 5
#define MAXIMUM_PROCS 32768	//프로세스는 최대 32768개이다.

//Proc의 정보들을 저장하는 데이터구조다.
typedef struct ProcStats
{
	pid_t pid;		//PID
	pid_t ppid;		//부모 PID
	pid_t pgid;		//그룹 ID
	pid_t sid;		//세션 ID
	float CPUusage;		//CPU 사용률
	int VSZ;		//가상 메모리 사용
	int RSS;		//사용 실제 메모리
	int FDSize;		//슬롯 개수
	int niceVal;		//우선순위 값
	
} ProcStats;

int getProcStats(ProcStats* procs);

//임시 dat 파일에 기록된 프로세스 정보들을 사용해 중요도 값을 계산한다.
void getProcScores()
{
	int count;

	ProcStats* procs = (ProcStats*)malloc(sizeof(ProcStats)*MAXIMUM_PROCS); //프로세스의 정보들이 담기는 구조체 배열

	//아래 메소드에서 프로세스 정보를 구조체 배열에 기록한다.
	count = getProcStats(procs);

	//funct1();
	//funct2();
	//funct3();
	//funct4();
	//funct5();
	
	free(procs);

}

//정보가 기록된 파일에서 프로세스와 그 정보들을 읽어들이고, 프로세스의 총 개수를 반환한다.
int getProcStats(ProcStats* procs)
{
	FILE* tmpF;				//정보를 불러올 임시 파일
	char* tempFname = "procstats.dat";	//정보를 불러올 임시 파일의 이름
	int idx = 0;

	tmpF = fopen(tempFname, "r");

	//프로세스와 그 정보들을 모두 구조체 배열에 저장한다.
	while(!feof(tmpF))
	{
		fscanf(tmpF, "%d %f %d %d %d %d %d %d %d\n", &((procs + idx)->pid), &((procs + idx)->CPUusage), &((procs + idx)->ppid), &((procs + idx)->pgid), &((procs + idx)->sid), &((procs + idx)->VSZ), &((procs + idx)->RSS), &((procs + idx)->FDSize), &((procs + idx)->niceVal));	
		idx++;
	}

	fclose(tmpF);

	return idx;
}
