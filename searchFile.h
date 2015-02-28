#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


//---사용할 상수
#define PROC_CATEGORY 5			//프로세스 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define FILE_CATEGORY 5			//파일 중요도 검사는 5개의 심사 부문을 거쳐 계산한다.
#define MAX_CATEGORY 100		//프로세스 중요도나 파일 중요도의 심사 부문은 합해도 최대 100개까지만 가능.
#define MAX_FILES 2048			//조사할 파일의 최대 개수

#define PROC_OFFSET 0			//가중치 값 중 프로세스 값들의 시작점
#define FILE_OFFSET PROC_CATEGORY	//가중치 값 중 파일 값들의 시작점

#define MAXIMUM_PATH_LEN 1024		//경로 문자열의 최대 길이(바이트)




//구해낸 항목들의 중요도 값에 임의 파일에 지정한 가중치를 반영시킨다.
//void calculateFileImportance(int categories)
//{
//	int i;
//	for(i = 0; i < categories; i++)
//	{
//		//---차후 구현
//	}
//		
//}

//탐색할 경로를 입력받는다.
void getPath(char* path)
{
	printf("\'.\' == current directory, \'..\' == upper directory\n");
	printf("Type path -> ");
	fgets(path, MAXIMUM_PATH_LEN, stdin);
	
	//개행문자를 삭제한다.
	*(path + (strlen(path) - 1)) = '\0';

	/******************************테스트로 사용했던 부분들
	if(strcmp(path, ".") == 0)	printf(".\n");
	
	if(strcmp(path, "..") == 0)	printf("..\n");

	printf("%s\n", path);
	*******************************************************/
	
}

//얻은 경로를 이용해 디렉터리를 열고 파일에 접근, 중요한 정보를 기록한다.
int openDirAndReport(char* path)
{
	DIR* dirp;				//디렉터리 구조체
	struct dirent *dentry;			//파일과 아이노드를 저장하는 파일 구조체의 일종
	FILE* tmpF;				//정보를 기록할 임시 파일
	char* tempFname = "filestats.dat";	//정보를 기록할 임시 파일의 이름
	struct stat fileInfo;			//파일 정보를 담는 stat 구조체
	char tempPath[MAXIMUM_PATH_LEN];	//디렉터리명을 연결하여 절대경로 파일 이름을 저장 
	int count = 0;				//파일 개수를 카운트

	//opendir로 입력받은 경로 디렉터리를 연다.
	if((dirp = opendir(path)) == NULL) perror("opendir");

	//임시 파일을 연다.
	tmpF = fopen(tempFname, "w");
	
	while(dentry = readdir(dirp))
	{
		//위 임시 파일과 .나 ..를 제외한 파일명들을 와 path/와 연결하여 전체 경로명을 얻어낸다.
		if(strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") && strcmp(dentry->d_name, "filestats.dat") != 0)
		{
		count++;

		if(count > MAX_FILES) perror("file overflow");

		strcpy(tempPath, path);
		strcat(tempPath, "/");
		strcat(tempPath, dentry->d_name);
		//printf("%s\n", tempPath);

		if(stat(tempPath, &fileInfo) == -1) perror("stst");
	
		/******************테스트 : 필요한 정보들을 출력해본다.*****************
		printf("파일명 :%s\n", tempPath); 			//파일명
		printf("st_dev : %d\n", fileInfo.st_dev);		//디바이스 노드
		printf("st_mode : %o\n", fileInfo.st_mode);		//권한
		printf("st_nlink : %d\n", fileInfo.st_nlink);		//하드링크 개수
		printf("st_uid : %d\n", fileInfo.st_uid); 		//소유자
		printf("st_gid : %d\n", fileInfo.st_gid);		//소유자 그룹
		printf("st_size : %d\n", fileInfo.st_size);		//바이트 크기
		printf("st_blksize : %d\n", fileInfo.st_blksize);	//블록 크기
		printf("st_blocks : %d\n", fileInfo.st_blocks);		//할당 블록 개수
		printf("st_atim : %u\n", fileInfo.st_atim);		//마지막 접근 시간
		*********************************************************************/

		//필요한 정보들을 임시 파일에 저장한다.
		fprintf(tmpF, "%s %d %o %d %d %d %d %d %d %u\n", tempPath, fileInfo.st_dev, fileInfo.st_mode, fileInfo.st_nlink, fileInfo.st_uid, fileInfo.st_gid, fileInfo.st_size, fileInfo.st_blksize, fileInfo.st_blocks, fileInfo.st_atim);
		
		
		}
	}

	fclose(tmpF);
	//임시 파일인 tmpF는 코드 수행 후 삭제되도록 만들 예정이다. 

	closedir(dirp);

	return count;
}
