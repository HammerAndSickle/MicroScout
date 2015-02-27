//중요도를 계산하는 알고리즘을 구현한 코드

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PROC_CATEGORY 5
#define MAXIMUM_PROCS 32768	//프로세스는 최대 32768개이다.

#define USE_SOFT_LIMIT 0
#define USE_HARD_LIMIT 1	//리소스 제한값 함수의 옵션으로 사용할 상수.

//Proc의 정보들을 저장하는 데이터구조다.
//값을 찾을 수 없을 경우 -1이 저장되도록 하였다.
//테스트 결과, VSZ, RSS에만 -1이 저장되는 것을 확인, 값을 찾지 못하는 경우를 VSZ, RSS에만 가정한다.
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

	//각 부문별로 중요도 점수 저장
	float score1;
	float score2;
	float score3;
	float score4;
	float score5;
	
} ProcStats;

void getProcScores();
int getProcStats(ProcStats* procs);
void eval1_nice(ProcStats* procs, int num);
void eval2_FD(ProcStats* procs, int num, int option);
void eval3_CPU_USAGE(ProcStats* procs, int num);
void eval4_pids_comp(ProcStats* procs, int num);
void eval5_memory(ProcStats* procs, int num);


//임시 dat 파일에 기록된 프로세스 정보들을 사용해 중요도 값을 계산한다.
void getProcScores()
{
	int count; 					//얻어낸 프로세스의 총 개수를 저장한다.
	//int idx;

	ProcStats* procs = (ProcStats*)malloc(sizeof(ProcStats)*MAXIMUM_PROCS); //프로세스의 정보들이 담기는 구조체 배열

	//아래 메소드에서 프로세스 정보를 구조체 배열에 기록한다.
	count = getProcStats(procs);
	
	eval1_nice(procs, count);			//nice 값을 이용해 점수 계산
	eval2_FD(procs, count, USE_HARD_LIMIT);		//FDSize 값을 이용해 점수 계산
	eval3_CPU_USAGE(procs, count);			//CPU 사용률을 이용해 점수 계산
	eval4_pids_comp(procs, count);			//PID 값들을 이용해 점수 계산
	eval5_memory(procs, count);			//memory 관련 값들을 이용해 점수 계산
	


	//-----------------------테스트용 출력 코드
	/*for(idx = 0; idx < count; idx++)
	{
		printf("%d : %f %f %f %f %f\n", (procs + idx)->pid, (procs + idx)->score1, (procs + idx)->score2, (procs + idx)->score3, (procs + idx)->score4, (procs + idx)->score5);
	}*/

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

//nice 값을 중요도로 환산한다.
void eval1_nice(ProcStats* procs, int num)
{
	int idx;
	int val;

	for(idx = 0; idx < num; idx++)
	{
		val = -(((procs + idx)->niceVal) -19);	//-19 후 *(-1)
		(procs + idx)->score1 = ((float)(val)/40)*100;	//val 값을 float화하여 (100/40)을 곱한다.
	}

	//ex) nice가 0일 경우, 우선도 값이 19이므로, 19 * (100/40)은 47.5 가 된다.
}

//FDSize 값을 중요도로 환산한다.
void eval2_FD(ProcStats* procs, int num, int option)
{
	struct rlimit rlim;			//리소스 제한값을 얻기 위한 구조체
	unsigned long max;			//maximum limit value
	int idx;

	getrlimit(RLIMIT_NOFILE, &rlim);	//커널에서 리소스 제한값을 얻어온다.

	switch(option)
	{
		case USE_SOFT_LIMIT:
			max = rlim.rlim_cur;
			break;
		case USE_HARD_LIMIT:
			max = rlim.rlim_max;
			break;
		default:
			perror("option");
			break;
	}
			
	//테스트용 리미트값 출력
	//printf("FILE MAX: %lu, MAX : %lu\n", rlim.rlim_cur, rlim.rlim_max);
	
	for(idx = 0; idx < num; idx++)
	{
		(procs + idx)->score2 = (((float)((procs + idx)->FDSize))/max)*100;	//float으로 나타내도록 한다.
	}
	

}

//CPU 사용률을 점수로 환산한다.
void eval3_CPU_USAGE(ProcStats* procs, int num)
{
	int idx;
	
	for(idx = 0; idx < num; idx++)
	{
		(procs + idx)->score3 = (procs + idx)->CPUusage;
	}


}

//PID들을 비교하여 중요도를 조사해 본다.
void eval4_pids_comp(ProcStats* procs, int num)
{
	int idx;
	float score;

	for(idx = 0; idx < num; idx++)
	{
		score = 20.0;	//기본 점수는 20점이다.
	
		//프로그램 그룹의 리더인가?
		if( (procs + idx)->pid == (procs + idx)->pgid )
			score += 30.0;

		//세션의 리더인가?
		if( (procs + idx)->pgid == (procs + idx)->sid )
			score += 40.0;

		//부모가 init이거나, 이 프로세스 자체가 init인가?
		if( (procs + idx)->ppid == 0 || (procs + idx)->ppid == 1 )
			score += 10.0;

		(procs + idx)->score4 = score;
	}
}

//memory VSZ, RSS값들을 이용해 중요도를 조사해 본다.
void eval5_memory(ProcStats* procs, int num)
{
	int idx;

	//RSS(실제 사용 메모리)는 VSZ(가상 메모리) 값까지 상승 가능하다.
	//효율적으로 사용하는 정도를 조사.

	for(idx = 0; idx < num; idx++)
	{
		//VSZ나 RSS가-1일 경우는 20점을 부여한다.
		if( (procs + idx)->VSZ == -1 || (procs + idx)->RSS == -1 )
			(procs + idx)->score5 = 20.0;

		//그 외의 경우, RSS/VSZ값을 효율 점수로 지정.
		else (procs + idx)->score5 = ((float)((procs + idx)->RSS) / (float)((procs + idx)->VSZ)) * 100;
	}
}
