#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


//---사용할 상수
#define PROC_CATEGORY 5			//프로세스 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define FILE_CATEGORY 5			//파일 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define MAX_CATEGORY 100		//프로세스 중요도나 파일 중요도의 심사 부문은 합해도 최대 100개까지만 가능.


#define PROC_OFFSET 0			//가중치 값 중 프로세스 값들의 시작점
#define FILE_OFFSET PROC_CATEGORY	//가중치 값 중 파일 값들의 시작점

//Process 중요도를 저장하는 데이터구조다.
//ex)프로세스 3444의 중요도는 99.
typedef struct ProcImportance
{
	char Name[255];
	long int importanceValue;
} ProcImportance;

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
		fprintf(f, "%d %d %d %d %d ", 18, 19, 20, 21, 22);
		fprintf(f, "%d %d %d %d %d ", 16, 18, 20, 22, 24);
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

//구해낸 항목들의 중요도 값에 임의 파일에 지정한 가중치를 반영시킨다.
void calculateProcImportance(ProcImportance* ImportanceArr, int categories)
{
	int i;
	for(i = 0; i < categories; i++)
	{
		//---차후 구현
	}
		
}

