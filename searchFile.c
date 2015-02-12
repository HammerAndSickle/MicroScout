#include "searchFile.h"
#include <sys/stat.h>

#define MAXIMUM_PATH_LEN 1024

void searchFile()
{

	int* weights;				//중요도 가중치를 저장할 정수 배열
	char* path;				//경로를 저장할 문자열
	//int i;					//테스트를 위한 iteration

	weights = (int*)malloc(sizeof(int)*FILE_CATEGORY);
	path = (char*)malloc(sizeof(char)*MAXIMUM_PATH_LEN);
	
	//---------------가중치를 읽어오는 함수
	getFileWeights(weights);

	/**************************파일 입/출력 테스트 부분
	for(i = 0; i < FILE_CATEGORY; i++)
	{
		printf("for test\n");
		printf("%d ", *(weights + i));
	}
	printf("test ended\n");
	*********************************/

	free(weights);
	free(path);

}
