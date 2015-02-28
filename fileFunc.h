#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define PROC_CATEGORY 5			//���μ��� �߿䵵 �˻�� 5���� �ɻ� �ι��� ���� ����Ѵ�.
#define FILE_CATEGORY 5			//���� �߿䵵 �˻�� 5���� �ɻ� �ι��� ���� ����Ѵ�.
#define MAX_CATEGORY 100		//���μ��� �߿䵵�� ���� �߿䵵�� �ɻ� �ι��� ���ص� �ִ� 100�������� ����.
#define MAXIMUM_PATH_LEN 1024	//���� �����θ��� �ִ� 1024����Ʈ ���ڿ�.

#define PROC_OFFSET 0			//���μ��� ����ġ �� ������ �ε���
#define FILE_OFFSET PROC_CATEGORY	//���� ����ġ �� ������ �ε���

#define SECTOR1 0		//�ι����� ���� �ε����� ����� ������ ��.
#define SECTOR2 1
#define SECTOR3 2
#define SECTOR4 3
#define SECTOR5 4

//==eval1 �Լ��� ���� ���
#define MODE_START_SCORE 200

//==eval2 �Լ��� ���� ���
#define BLKS_START_SCORE 50
#define BLKS_WEIGHT 15			//��� ���� ������

//==eval3 �Լ��� ���� ���
#define HARDLINK_START_SCORE 30
#define HARDLINK_WEIGHT 2		//�ϵ帵ũ ���� ������

//==eval4 �Լ��� ���� �ð� �ٷ�� ����� 
#define MINUTE 60		//��, �ð�, ��, ��, ���� �ʷ� ǥ���� ��
#define HOUR (MINUTE*60)
#define DAY (HOUR*24)
#define WEEK (DAY*7)
#define MONTH (WEEK*4)
#define OVER_WEEK_START_SCORE 10
#define OVER_WEEK_OFFSET 10
#define OVER_DAY_START_SCORE 30
#define OVER_DAY_OFFSET 5
#define OVER_HOUR_START_SCORE 66
#define OVER_HOUR_OFFSET 1

//==eval5 �Լ��� ���� ���
#define ETC_START_SCORE 40
#define ROOT_UID 0			//root uid�� 0�� ���´�.


#define TOP_RANKINGS 25

//File�� �������� �����ϴ� �����ͱ�����.
typedef struct FileStats
{
	char name[MAXIMUM_PATH_LEN];	//������ ������ �̸�
	int devNode;			//������ ����̽� ���
	int mode;			//������ ����
	int hardlinks;			//������ �ϵ� ��ũ ����
	uid_t uid;			//������ ������
	gid_t gid;			//������ ������ �׷�
	int size;			//������ ����Ʈ ũ��
	int blksize;			//������ ��� ũ��
	int blocks;			//������ �Ҵ� ��� ����
	long int atime;			//������ ���� ��� �ð�
	
	float scores[FILE_CATEGORY];	//������ �߿䵵 ������
	float scoreSum;
} FileStats;


//sector�� ������ �����ϴ� �迭�� �ε����� ���δ�.
void eval1_stat(FileStats* files, int num, int sector);
void eval2_blks(FileStats* files, int num, int sector);
void eval3_hardlinks(FileStats* files, int num, int sector);
void eval4_access_time(FileStats* files, int num, int sector);
void eval5_etc(FileStats* files, int num, int sector);


//���� ���� �߿䵵�� ȯ���Ѵ�.
void eval1_stat(FileStats* files, int num, int sector)
{
	int idx;
	int modeVal;		//mode_t value
	int score;		//score value

	//������ ����-�׷��� ����-�Ϲݻ���� ����
	//���Ѱ��� �������� �߿��� ���Ϸ� ����.
	

	//ó���� 200������ ������ ������, ������ �������� ������ ��������.
	//rwxrwxrwx��� ��� �� 0���� �Ǹ�, _________��� 200�� �״�� ���´�.
	//�׷��� sticky bit, setuid, setgid(��� ������ ���� ����) ��Ʈ�� �ִٸ� ������ �߰��Ѵ�.
	
	for(idx = 0; idx < num; idx++)
	{
		score = MODE_START_SCORE;	//200������ ����
		modeVal = (files + idx)->mode;

		//���� ������ ��� : �ִ� 100�� ����
		if(modeVal & S_IXOTH) score -=50;
		if(modeVal & S_IXGRP) score -=30;
		if(modeVal & S_IXUSR) score -=20;

		//���� ������ ��� : �ִ� 70�� ����
		if(modeVal & S_IWOTH) score -=40;
		if(modeVal & S_IWGRP) score -=20;
		if(modeVal & S_IWUSR) score -=10;

		//�б� ������ ��� : �ִ� 30�� ����
		if(modeVal & S_IROTH) score -=15;	
		if(modeVal & S_IRGRP) score -=10;
		if(modeVal & S_IRUSR) score -=5;
		
		//setuid permission
		if(modeVal & 04000) score += 30;

		//setgid permission
		if(modeVal & 02000) score += 20;

		//sticky bit permission
		if(modeVal & 01000) score += 50;

		(files + idx)->scores[sector] = (((float)score) / 300 ) * 100;
		
	}
}

//�� ������ �� ��, �뷮���� �ϼ����� ���� �߿䵵�� ȯ���Ѵ�.
void eval2_blks(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = BLKS_START_SCORE;		//50������ ����
		
		//block�� 0����� �⺻ 50���� �״�� ����.
		//BLKS_WEIGHT�� 15���̴�. �� ������  �ּ� 35��, �ִ� 65��.
		//blksize * blocks > size�ϼ��� ���Ͽ� ������ ���� ������ ���� ������ ���� �ο�.
		if((files + idx)->blocks != 0)
		{
			if( ( (files + idx)->blksize * (files + idx)->blocks ) >= (files + idx)->size )
				score +=BLKS_WEIGHT;

			else	score -=BLKS_WEIGHT;
			
		}

		(files + idx)->scores[sector] = (float)score;
		

	}

}

//�ϵ� ��ũ ������ �߿䵵�� ȯ���Ѵ�.
void eval3_hardlinks(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = HARDLINK_START_SCORE;	//30������ ����
		
		//������ ������ ��������ŭ �ϵ帵ũ ������ ���Ͽ� ���Ѵ�.
		score += HARDLINK_WEIGHT * (files + idx)->hardlinks;

		//���� 100���� ���� ���, 100������ ������Ų��.
		if(score > 100) score = 100;

		(files + idx)->scores[sector] = (float)score;
	}

}

//���� ���� �ð��� �߿䵵�� ȯ���Ѵ�.
void eval4_access_time(FileStats* files, int num, int sector)
{
	int idx;
	time_t t;		//���� �ð��� ������ time_t
	long gap;		//(���� �ð� - ������ atime) ���̰�
	int score;

	//time_t�� ���(1970/1/1)���κ��� �帥 '��(sec)'���� ��
	
	//�߿��� ������ ��Ե� ���� �����ϰ� �� ���ɼ��� ���ٰ� ����.

	for(idx = 0; idx < num; idx++)
	{
		score = 0;		//initial value : 0

		gap = ((long)time(&t)) - ((files + idx)->atime);

		
	
		//������ �� �� ���� ���� ��� -> �״�� 0
		if(gap >= MONTH) {}
		
		//������ �� 1�� �̻��� ��� -> 10~30
		else if(gap >= WEEK)
		{
			//(gap/WEEK)�� ������ 1~3
			score = OVER_WEEK_START_SCORE;			//start=10
			score += (gap/WEEK -1)*OVER_WEEK_OFFSET;	//offset=10
		}

		//������ �� �Ϸ� �̻��� ��� -> 35~60
		else if(gap >= DAY)
		{	
			//(gap/DAY)�� ������ 1~6
			score = OVER_DAY_START_SCORE;			//start=30
			score += (gap/DAY)*OVER_DAY_OFFSET;		//offset=5
		}

		//������ �� �� �ð� �̻��� ��� -> 67~90
		else if(gap >= HOUR)
		{	
			//(gap/HOUR)�� ������ 1 �̻�, 24 �̸�
			score = OVER_HOUR_START_SCORE; 			//start=66;
			score += (((gap)/HOUR)*OVER_HOUR_OFFSET);		//offset=1
		}
	
		//�� ��, ������ �� �� �ð� �̳� -> 100
		else
		{		
			score = 100;
		}

		(files + idx)->scores[sector] = (float)score;

	}

}


//�����ڳ� ����̽� ��� ������ ���, �߿䵵�� ����.
void eval5_etc(FileStats* files, int num, int sector)
{
	int idx;
	int score;

	for(idx = 0; idx < num; idx++)
	{
		score = ETC_START_SCORE;
		
		//�����ڰ� root�� ��� ���� ����
		if( (files + idx)->uid == ROOT_UID ) score += 20;

		//������ �׷쵵 root�� ��� ���� ����
		if( (files + idx)->gid == ROOT_UID ) score += 20;

		//�� ������ �������� ����̽��� �����ϴ°�?
		//dev_t�� major�� ����ϸ� �� ��ȣ 12��Ʈ ��ȯ
		if( gnu_dev_major((files + idx)->devNode) > 0) score += 20;

		
		(files + idx)->scores[sector] = (float)score;
	}

}


