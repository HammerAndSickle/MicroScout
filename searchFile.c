#include "searchFile.h"
#include <sys/stat.h>

void searchFile()
{

	int* weights;				//중요도 가중치를 저장할 정수 배열
	char* path;				//경로를 저장할 문자열
	int fileCount;				//조사할 파일의 개수
	//int i;				//테스트를 위한 iteration

	weights = (int*)malloc(sizeof(int)*FILE_CATEGORY);
	path = (char*)malloc(sizeof(char)*MAXIMUM_PATH_LEN);
	
	//---------------가중치를 읽어오는 함수
	getFileWeights(weights);

	//경로를 입력받아 에 저장하는 함수
	getPath(path);

	//입력한 경로에 해당하는 디렉터리를 열고 파일 정보들을 기록하는 함수
	//이 함수는 파일의 개수를 반환한다.
	fileCount = openDirAndReport(path);

	/**************************파일 입/출력 테스트 부분
	for(i = 0; i < FILE_CATEGORY; i++)
	{
		printf("for test\n");
		printf("%d ", *(weights + i));
	}
	printf("test ended\n");
	*********************************/


	//기록한 파일 정보를 받아 중요도 값들을 계산한다.
	getScores(fileCount);

	free(weights);
	free(path);

}
