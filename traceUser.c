#include "traceUser.h"
#include <string.h>
#include <sys/types.h>

void traceUser();
void printTrails(); 

//유저들의 동향을 간단히 조사하는 함수
void traceUser()
{
	pid_t pid;			//pid 자료형.(fork의 결과값)
	int ret;			//exec 결과값을 저장
	FILE* fileDes;			//파일 디스크립터
	int status;			//동기화를 위한 정수 변수

	chmod("ut.sh", 0775);		//쉘 스크립트 권한 변경

	//프로세스 복제.
	pid = fork();

	if(pid == -1) perror("fork");

	//자식 프로세스가 쉘 스크립트 실행을 맡는다.
	else if(pid == 0)
	{

		//ut.sh는 유저들의 명령어들을 조사해 간단한 평가를 내린다.
		ret = execl("ut.sh", "ut.sh", (char*)0);

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

		//유저들의 행적들을 간략하게 평가
		printTrails();		
		
	}

}

//유저들의 행적을 출력하는 함수
void printTrails() 
{ 
 	int count;		//num of users 
 	UserStats* users = (UserStats*)malloc(sizeof(UserStats)*MAX_USERS);	//유저 정보 구조체 배열 

	//---------start of function
	traceUserSTART();
 	 
 	count = getUserStats(users);	//유저들의 행적값들을 파일에서 불러들인다.
	eval_print(users, count);	//행적을 조사해 출력
 
	//-------------end of function
	traceUserEND(users);
} 
