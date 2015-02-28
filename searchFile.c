#include "searchFile.h"
#include <sys/stat.h>

void searchFile()
{

	char* path;				//경로를 저장할 문자열
	int fileCount;				//조사할 파일의 개수
	//int i;				//테스트를 위한 iteration

	path = (char*)malloc(sizeof(char)*MAXIMUM_PATH_LEN);
	

	//경로를 입력받아 에 저장하는 함수
	getPath(path);

	//입력한 경로에 해당하는 디렉터리를 열고 파일 정보들을 기록하는 함수
	//이 함수는 파일의 개수를 반환한다.
	fileCount = openDirAndReport(path);

	//기록한 파일 정보를 받아 중요도 값들을 계산한다.
	getFileScores(fileCount);

	free(weights);
	free(path);

}
