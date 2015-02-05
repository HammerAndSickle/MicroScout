#include <stdio.h>
#include <unistd.h>

//사용자들의 접근 로그를 탐색할 것이다.
void searchProc()
{
	//사용자 접속 로그 파일 경로
	//이 파일을 이용할지, 혹은 lastlog 커맨드를 이용해 sh 파일을 이용할지는 아직 정하지 않음.
	char wtmpPath[] = "/var/log/wtmp";
	char lastlogPath[] = "/var/log/lastlog";

	printf("Lesser important processes...");

}
