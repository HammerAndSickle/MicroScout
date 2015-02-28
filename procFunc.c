//중요도를 계산하는 알고리즘을 구현한 코드

#include "procFunc.h"

//선언된 함수들. eval계열의 함수에 있는 매개변수인 sector는 주어진 값에 따라 score의 인덱스로 쓰인다.

void getProcScores();
int getProcStats(ProcStats* procs);
void getProcWeights(int* arr);
void getProcRanking(ProcStats* procs, int* weights, int count);
void sumProcScore(ProcStats* proc, int* weights);
int compareProcscore(const void* a, const void* b);

//임시 dat 파일에 기록된 프로세스 정보들을 사용해 중요도 값을 계산한다.
void getProcScores()
{
	int count; 					//얻어낸 프로세스의 총 개수를 저장한다.
	//int idx;

	int* weights;			//중요도 가중치를 저장할 정수 배열

	ProcStats* procs = (ProcStats*)malloc(sizeof(ProcStats)*MAXIMUM_PROCS); //프로세스의 정보들이 담기는 구조체 배열

	printf("============ GETTING PROCS SORTED..... ==============\n\n\n");

	//아래 메소드에서 프로세스 정보를 구조체 배열에 기록한다.
	count = getProcStats(procs);
	
	eval1_nice(procs, count, SECTOR1);			//nice 값을 이용해 점수 계산
	eval2_FD(procs, count, USE_HARD_LIMIT, SECTOR2);	//FDSize 값을 이용해 점수 계산
	eval3_CPU_USAGE(procs, count, SECTOR3);			//CPU 사용률을 이용해 점수 계산
	eval4_pids_comp(procs, count, SECTOR4);			//PID 값들을 이용해 점수 계산
	eval5_memory(procs, count, SECTOR5);			//memory 관련 값들을 이용해 점수 계산
	


	/**-----------------------테스트용 출력 코드
	for(idx = 0; idx < count; idx++)
	{
		printf("%d : %f %f %f %f %f\n", (procs + idx)->pid, (procs + idx)->scores[0], (procs + idx)->scores[1], (procs + idx)->scores[2], (procs + idx)->scores[3], (procs + idx)->scores[4]);
	}
	------------------*/
	
	//중요도 배열
	weights = (int*)malloc(sizeof(int)*PROC_CATEGORY);

	
	//-----------------중요도 값들을 파일에서 읽어들이는 함수()
	getProcWeights(weights);
	
	//---------------final calculation
	getProcRanking(procs, weights, count);
	
	printf("\n\n=============== FINISHED =================\n\n\n");


	free(procs);
	free(weights);

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

	//정보를 모두 옮겼으니 기록 파일을 삭제한다.
	if(remove(tempFname) == -1) perror("remove");

	return idx;
}

//일단 모든 가중치는 파일 weight.db에 저장되고, 여기의 값을 모두 temp에 옮긴 뒤, 프로세스 중요도 탐색 작업에 필요한 가중치들만을 골라서 arr에 가져오도록 한다.
//
//arr :		작업에 필요한 가중치 배열이다. 이는 그 작업에서 선언, 할당되어서 사용된다.
//		예를 들어 '프로세스 중요도 탐색' 작업을 수행한다면 그에 필요한 가중치들을 가져간다.
//		작업마다 필요한 가중치 개수는 위에서 상수로 지정되어있다.
void getProcWeights(int* arr)
{
	char* fname = "weight.db";		//가중치 값을 저장하는 파일
	FILE *f;
	int i;
	int temp[MAX_CATEGORY]; 		//임시 정수 배열로, 최대 100개까지의 값을 저장
	int offset;				//offset은 매개변수 opt의 값에 따라 달라진다.
		

	if((f = fopen(fname, "r")) == NULL)	//파일이 없는 경우 디폴트 값 생성.
	{
		f = fopen(fname, "w");
		fprintf(f, "%d %d %d %d %d ", 10, 12, 10, 8, 12);	//process weight
		fprintf(f, "%d %d %d %d %d ", 10, 10, 10, 10, 10);	//file weiht
		fclose(f);
	}
	
	
	//프로세스 중요도 심사항목이 5개, 파일 중요도 심사항목이 5개라고 가정하면
	//데이터 파일에 저장된 가중치는 10개뿐이다.
	for(i = 0; i < PROC_CATEGORY + FILE_CATEGORY; i++)
	{
		fscanf(f, "%d", &temp[i]);
	}

	//이제 매개변수로 들어온 배열 arr에 가중치 값들을 집어넣는다.
	//프로세스의 가중치에 해당되는 값들을 불러온다.
	for(i = 0; i < PROC_CATEGORY; i++)
	{
		*(arr + i) = temp[PROC_OFFSET + i];
	}
		

	fclose(f);
}

//가중치를 반영한, 최종적인 점수, ranking 정렬 후 출력
void getProcRanking(ProcStats* procs, int* weights, int count)
{
	int idx;

	for(idx = 0; idx < count; idx++)
	{
		sumProcScore( (procs + idx), weights);
	}

	qsort(procs, count, sizeof(ProcStats), compareProcscore);	//정렬

	for(idx = 0; idx < TOP_RANKINGS; idx++)
	{
		//만일 랭킹 최대 개수보다 현재 프로세스들이 적을 경우
		if(idx == count) break;

		printf("%d.  %d   %f\n", (idx+1), (procs + idx)->pid, (procs + idx)->scoreSum);
	}

}

//가중치를 반영한 점수들의 합을 구해 구조체에 저장한다.
void sumProcScore(ProcStats* proc, int* weights)
{
	int idx;
	float sum = 0.0;
	
	for(idx = 0; idx < PROC_CATEGORY; idx++)
	{
		sum = sum + ( proc->scores[idx] * (*(weights + idx)) ); 
	}

	proc->scoreSum = sum;
}

//compare process' scores for sorting
int compareProcscore(const void* a, const void* b)
{
	float score1, score2;
	FileStats* file1;
	FileStats* file2;

	file1 = ((FileStats*)a);
	file2 = ((FileStats*)b);
	
	score1 = file1->scoreSum;
	score2 = file2->scoreSum;

	if(score1 > score2) return -1;

	else if(score1 < score2) return 1;

	else return 0;
}
