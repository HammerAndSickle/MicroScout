#include "MScout.h"
#include <string.h>
#include <sys/types.h>

#define MAXIMUM_PROCS 32768

//프로세스들을 탐색, 중요도를 정리하는 기능.
void searchProc()
{
	pid_t pid;			//pid 자료형.(fork의 결과값)
	int ret;			//exec 결과값을 저장
	FILE* fileDes;			//파일 디스크립터
	int status;			//동기화를 위한 정수 변수

	int* weights;			//중요도 가중치를 저장할 정수 배열
	Importance* items;		 //중요도를 저장할 자료 배열

	//프로세스 복제.
	pid = fork();

	if(pid == -1) perror("fork");

	//자식 프로세스가 쉘 스크립트 실행을 맡는다.
	else if(pid == 0)
	{
		printf("child\n");

		//ps.sh는 테스트용 쉘 코드 파일이다.
		ret = execl("ps.sh", "ps.sh", (char*)0);

		if(ret == -1) perror("execv");

		//exec 함수는 프로세스를 실행 후, 그 다음 코드는 건너뛴다.
		//현재 프로세스를 다른 프로세스로 대체하는 것이기 때문.
		//(fork를 쓴 이유이다.)
		
		//자식을 여기서 끝내면 부모가 그제서야 움직이도록 동기화하였다.
		exit(1);
	}

	//이 부분은 부모 프로세스가 계속 진행할 부분이다.
	//'동기화'를 구현하여 자식 프로세스를 먼저 진행시키고 끝낸 후,  부모를 진행.
	else if(pid > 0)
	{

		//여기선 부모 프로세스이니, pid에는 자식 프로세스 pid이 저장되어있다.
		//자식이 exit 할때까지 기다리게 된다.
		waitpid(pid, &status, 0);
		
		weights = (int*)malloc(sizeof(int)*PROC_CATEGORY);	//가중치 배열을 할당.
		items = (Importance*)malloc(sizeof(Importance)*MAXIMUM_PROCS); //중요도 자료구조 배열을 할당.

		//-----------------중요도 값들을 파일에서 읽어들이는 함수() - MScout.h
		getWeights(weights, SEARCH_PROC);		
		
		printf("parent\n");
		
		// 자식 프로세스에서(exec 부분) 실행되는 sh 파일에서  임시 데이터 파일을 만들어 여기서 읽는다
		// 현재는 아직 파일을 만들지 않았다.
		fileDes = fopen("tempReport.dat", "r");
		if(!fileDes) perror("fopen");

		//------------------데이터 파일들의 값을 읽어들이는 함수() - SearchProc.h
		//------------------데이터 파일을 토대로 중요도를 계산하는 함수() - SearchProc.h
		
		free(weights);
		free(items);

		fclose(fileDes);
	}

}
