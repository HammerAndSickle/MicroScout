#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_PROCS 32768

//프로세스들을 탐색, 중요도를 정리하는 기능.
void searchProc()
{
	pid_t pid;			//pid 자료형.(fork의 결과값)
	int ret;			//exec 결과값을 저장
	FILE* fileDes;			//파일 디스크립터
	int status;			//동기화를 위한 정수 변수
	//int i;			//테스트를 위한 iteration 변수



	chmod("pt.sh", 0775);		//쉘스크립트 권한을 설정

	//프로세스 복제.
	pid = fork();

	if(pid == -1) perror("fork");

	//자식 프로세스가 쉘 스크립트 실행을 맡는다.
	else if(pid == 0)
	{

		//pt.sh는 현재 프로세스들의 얻고자 하는 정보들을 /proc 등의 디렉토리의 파일에서 얻어오며,
		//그 정보들을 임시 파일에 저장할 것이다.
		ret = execl("pt.sh", "pt.sh", (char*)0);

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
		
		
		

		//위 pt.sh을 이용해 프로세스들의 정보를 임의의 파일에 기록했으니 아래 함수에서 그 내용을 읽어
		//중요도를 계산한다. 
		getProcScores();
		

	}

}
