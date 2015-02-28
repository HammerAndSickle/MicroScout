#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PROC_CATEGORY 5
#define FILE_CATEGORY 5			//파일 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define MAX_CATEGORY 100		//프로세스 중요도나 파일 중요도의 심사 부문은 합해도 최대 100개까지만 가능.
#define MAXIMUM_PROCS 32768		//프로세스는 최대 32768개이다.

#define PROC_OFFSET 0			//가중치 값 중 프로세스 값의 시작점 인덱스
#define FILE_OFFSET PROC_CATEGORY	//가중치 값 중 파일 값의 시작점 인덱스

#define USE_SOFT_LIMIT 0
#define USE_HARD_LIMIT 1	//리소스 제한값 함수의 옵션으로 사용할 상수.

#define SECTOR1 0		//부문별로 점수 인덱스를 상수로 정의한 것.
#define SECTOR2 1
#define SECTOR3 2
#define SECTOR4 3
#define SECTOR5 4

#define NO_VALUE -1		//프로세스의 몇몇 정보를 얻지 못하면 -1을 저장하게 된다.

//Proc의 정보들을 저장하는 데이터구조다.
//값을 찾을 수 없을 경우 -1이 저장되도록 하였다.
//테스트 결과, CPUusage, VSZ, RSS에만 -1이 저장되는 것을 확인, 값을 찾지 못하는 경우를 CPUusage, VSZ, RSS에만 가정한다.
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
	float scores[PROC_CATEGORY];
	float scoreSum;		//sum of score
	
} ProcStats;


//sector는 점수를 저장하는 배열의 인덱스로 쓰인다.
void eval1_nice(ProcStats* procs, int num, int sector);
void eval2_FD(ProcStats* procs, int num, int option, int sector);
void eval3_CPU_USAGE(ProcStats* procs, int num, int sector);
void eval4_pids_comp(ProcStats* procs, int num, int sector);
void eval5_memory(ProcStats* procs, int num, int sector);


//nice 값을 중요도로 환산한다.
void eval1_nice(ProcStats* procs, int num, int sector)
{
	int idx;
	int val;

	for(idx = 0; idx < num; idx++)
	{
		val = -(((procs + idx)->niceVal) -19);	//-19 후 *(-1)
		(procs + idx)->scores[sector] = ((float)(val)/40)*100;	//val 값을 float화하여 (100/40)을 곱한다.
	}

	//ex) nice가 0일 경우, 우선도 값이 19이므로, 19 * (100/40)은 47.5 가 된다.
}

//FDSize 값을 중요도로 환산한다.
void eval2_FD(ProcStats* procs, int num, int option, int sector)
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
		(procs + idx)->scores[sector] = (((float)((procs + idx)->FDSize))/max)*100;	//float으로 나타내도록 한다.
	}
	

}

//CPU 사용률을 점수로 환산한다.
void eval3_CPU_USAGE(ProcStats* procs, int num, int sector)
{
	int idx;
	
	for(idx = 0; idx < num; idx++)
	{
		//값을 찾지 못해 -1이 저장되어있을 경우,
		if( (procs + idx)->CPUusage == NO_VALUE )
			(procs + idx)->scores[sector] = 0.0;
		else
			(procs + idx)->scores[sector] = (procs + idx)->CPUusage;
	}


}

//PID들을 비교하여 중요도를 조사해 본다.
void eval4_pids_comp(ProcStats* procs, int num, int sector)
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

		(procs + idx)->scores[sector] = score;
	}
}

//memory VSZ, RSS값들을 이용해 중요도를 조사해 본다.
void eval5_memory(ProcStats* procs, int num, int sector)
{
	int idx;

	//RSS(실제 사용 메모리)는 VSZ(가상 메모리) 값까지 상승 가능하다.
	//효율적으로 사용하는 정도를 조사.

	for(idx = 0; idx < num; idx++)
	{
		//VSZ나 RSS가-1일 경우는 20점을 부여한다.
		if( (procs + idx)->VSZ == -1 || (procs + idx)->RSS == -1 )
			(procs + idx)->scores[sector] = 20.0;

		//그 외의 경우, RSS/VSZ값을 효율 점수로 지정.
		else (procs + idx)->scores[sector] = ((float)((procs + idx)->RSS) / (float)((procs + idx)->VSZ)) * 100;
	}
}
