#include <stdio.h>
#include <unistd.h>

//프로세스들을 탐색, 중요도를 정리하는 기능.
void searchProc()
{
	
	//실행할 sh파일의 이름.(아직 미정)
	char scriptName = "-----.sh";
	

	//**************쉘 스크립트 파일이 있는지 확인하는 코드를 만들도록 한다. 없으면 예외 처리를 수행.


	//쉘 스크립트 파일을 실행하는 exec 계열 함수.
	//현재는 쉘 스크립트 파일이 같은 폴더에 있는 것으로 가정. 
	execl("ps.sh", "ps.sh", (char*)0);


	//exec 함수는 프로세스를 실행 후, 그 다음 코드는 건너뛴다.
	//이 문제를 해결할 방법을 후에 알아보도록 한다.(fork()를 동원하는 방법도 있다.)
	printf("Lesser important processes...");

}
