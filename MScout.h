#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


//---사용할 상수

#define PROC_CATEGORY 5			//프로세스 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define FILE_CATEGORY 5			//파일 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define MAX_CATEGORY 100		//프로세스 중요도나 파일 중요도의 심사 부문은 합해도 최대 100개까지만 가능.

#define SEARCH_PROC 1			//아래 가중치 함수를 위한 옵션 매개변수.
#define SEARCH_FILE 2

//중요도를 저장하는 데이터구조다.
//ex)프로세스 3444의 중요도는 99.
//
typedef struct Importance
{
	char Name[255];
	long int importanceValue;
} Importance;


//필요한 가중치를 가져온다. parameter opt에 무얼 넣느냐에 따라 프로세스 가중치 혹은 파일 가중치를 가져온다.
//
//일단 모든 가중치는 파일 weight.db에 저장되고, 여기의 값을 모두 temp에 옮긴 뒤, 사용자가 Micro Scout 에서 선택한
//작업에 필요한 가중치들만을 골라서 arr에 가져오도록 한다.
//
//arr :		작업에 필요한 가중치 배열이다. 이는 그 작업에서 선언, 할당되어서 사용된다.
//		예를 들어 '프로세스 중요도 탐색' 작업을 수행한다면 그에 필요한 가중치들을 가져간다.
//		작업마다 필요한 가중치 개수는 위에서 상수로 지정되어있다.
//opt :		사용자가 Micro Scout에서 선택한 작업의 식별번호. 위에서 상수로 지정되어있다.
void getWeights(int* arr, int opt)
{
	char* fname = "weight.db";		//가중치 값을 저장하는 파일
	FILE *f;
	int i;
	int temp[MAX_CATEGORY]; 		//임시 정수 배열로, 최대 100개까지의 값을 저장
	int offset;				//offset은 매개변수 opt의 값에 따라 달라진다.
	int categories;				//categories는 매개변수 의 값에 따라 달라진다.

	//offset과 categories 값을 에 따라 정한다.
	//offset :	temp 배열의 시작점. temp 배열은 파일의 모든 값을 저장 중이므로
	//		 여기서 어떤 값을 골라 arr에 저장할지 결정한다.
	//categories :	항목의 개수. 프로세스 중요도 검사일 경우, 항목은 5개이다(위에서 함수로 정의하였다.)
	//		for 반복문의 반복 횟수로 쓰일 것이다.
	//		
	switch(opt)
	{
	case SEARCH_PROC:
		offset = 0;
		categories = PROC_CATEGORY;
		break;
	case SEARCH_FILE:
		offset = PROC_CATEGORY;
		categories = FILE_CATEGORY;
		break;
	}

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
	//옵션에 따라서 어디의 값들을 주는지가 다르다.(process or file?)
	for(i = 0; i < categories; i++)
	{
		*(arr + i) = temp[offset + i];
	}
		

	fclose(f);
}

//구해낸 항목들의 중요도 값에 임의 파일에 지정한 가중치를 반영시킨다.
void calculateImportance(Importance* ImportanceArr, int categories)
{
	int i;
	for(i = 0; i < categories; i++)
	{
		//---차후 구현
	}
		
}


