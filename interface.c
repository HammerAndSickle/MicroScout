#include <stdio.h>
#include <unistd.h>

#define PROCESS_SELECT 49
#define FILE_SELECT 50
#define USER_SELECT 51

//인터페이스 화면으로, 처음 타이틀 화면을 구현한다.
void Interface()
{
	char cmd;

	while(1)
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n==================================");
		printf("\n=======    MICRO SCOUT    ========");
		printf("\n==================================");

		printf("\n\n2015, by CHA DONG MIN\n");

		//구현할 메뉴 목록
		printf("\n\n---------------------------------");
		printf("\n|1. Trace Processes Importance    |");
		printf("\n|2. Trace File Importance         |");
		printf("\n|3. Trace Users' Activity         |");
		printf("\n---------------------------------");

		printf("\n\nPress Q to quit\n\n");

		cmd = getchar();
		getchar();
		//엔터 키(10)를 제거하는 부분이다. 버퍼에 엔터 키가 들어가면 아래 조건문에서 오류가 발생한다.

		printf("\n\n\n\n\n\n\n\n\n\n\n\n");

		switch(cmd)
		{
		case PROCESS_SELECT:
			searchProc();
			cmd = 'q';
			break;
		case FILE_SELECT:
			searchFile();
			cmd = 'q';
			break;
		case USER_SELECT:
			traceUser();
			cmd = 'q';
			break;
		}

		if(cmd == 'q' || cmd == 'Q') break;

	}

}
