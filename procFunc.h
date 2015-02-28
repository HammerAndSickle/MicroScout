#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PROC_CATEGORY 5
#define FILE_CATEGORY 5			//���� �߿䵵 �˻�� 5���� �ɻ� �ι��� ���� ����Ѵ�.
#define MAX_CATEGORY 100		//���μ��� �߿䵵�� ���� �߿䵵�� �ɻ� �ι��� ���ص� �ִ� 100�������� ����.
#define MAXIMUM_PROCS 32768		//���μ����� �ִ� 32768���̴�.

#define PROC_OFFSET 0			//����ġ �� �� ���μ��� ���� ������ �ε���
#define FILE_OFFSET PROC_CATEGORY	//����ġ �� �� ���� ���� ������ �ε���

#define USE_SOFT_LIMIT 0
#define USE_HARD_LIMIT 1	//���ҽ� ���Ѱ� �Լ��� �ɼ����� ����� ���.

#define SECTOR1 0		//�ι����� ���� �ε����� ����� ������ ��.
#define SECTOR2 1
#define SECTOR3 2
#define SECTOR4 3
#define SECTOR5 4

#define NO_VALUE -1		//���μ����� ��� ������ ���� ���ϸ� -1�� �����ϰ� �ȴ�.

//Proc�� �������� �����ϴ� �����ͱ�����.
//���� ã�� �� ���� ��� -1�� ����ǵ��� �Ͽ���.
//�׽�Ʈ ���, CPUusage, VSZ, RSS���� -1�� ����Ǵ� ���� Ȯ��, ���� ã�� ���ϴ� ��츦 CPUusage, VSZ, RSS���� �����Ѵ�.
typedef struct ProcStats
{
	pid_t pid;		//PID
	pid_t ppid;		//�θ� PID
	pid_t pgid;		//�׷� ID
	pid_t sid;		//���� ID
	float CPUusage;		//CPU ����
	int VSZ;		//���� �޸� ���
	int RSS;		//��� ���� �޸�
	int FDSize;		//���� ����
	int niceVal;		//�켱���� ��

	//�� �ι����� �߿䵵 ���� ����
	float scores[PROC_CATEGORY];
	float scoreSum;		//sum of score
	
} ProcStats;


//sector�� ������ �����ϴ� �迭�� �ε����� ���δ�.
void eval1_nice(ProcStats* procs, int num, int sector);
void eval2_FD(ProcStats* procs, int num, int option, int sector);
void eval3_CPU_USAGE(ProcStats* procs, int num, int sector);
void eval4_pids_comp(ProcStats* procs, int num, int sector);
void eval5_memory(ProcStats* procs, int num, int sector);


//nice ���� �߿䵵�� ȯ���Ѵ�.
void eval1_nice(ProcStats* procs, int num, int sector)
{
	int idx;
	int val;

	for(idx = 0; idx < num; idx++)
	{
		val = -(((procs + idx)->niceVal) -19);	//-19 �� *(-1)
		(procs + idx)->scores[sector] = ((float)(val)/40)*100;	//val ���� floatȭ�Ͽ� (100/40)�� ���Ѵ�.
	}

	//ex) nice�� 0�� ���, �켱�� ���� 19�̹Ƿ�, 19 * (100/40)�� 47.5 �� �ȴ�.
}

//FDSize ���� �߿䵵�� ȯ���Ѵ�.
void eval2_FD(ProcStats* procs, int num, int option, int sector)
{
	struct rlimit rlim;			//���ҽ� ���Ѱ��� ��� ���� ����ü
	unsigned long max;			//maximum limit value
	int idx;

	getrlimit(RLIMIT_NOFILE, &rlim);	//Ŀ�ο��� ���ҽ� ���Ѱ��� ���´�.

	switch(option)
	{
		case USE_SOFT_LIMIT:
			max = rlim.rlim_cur;
			break;
		case USE_HARD_LIMIT:
			max = rlim.rlim_max;
			break;
		default:
			perror("option");
			break;
	}
			
	//�׽�Ʈ�� ����Ʈ�� ���
	//printf("FILE MAX: %lu, MAX : %lu\n", rlim.rlim_cur, rlim.rlim_max);
	
	for(idx = 0; idx < num; idx++)
	{
		(procs + idx)->scores[sector] = (((float)((procs + idx)->FDSize))/max)*100;	//float���� ��Ÿ������ �Ѵ�.
	}
	

}

//CPU ������ ������ ȯ���Ѵ�.
void eval3_CPU_USAGE(ProcStats* procs, int num, int sector)
{
	int idx;
	
	for(idx = 0; idx < num; idx++)
	{
		//���� ã�� ���� -1�� ����Ǿ����� ���,
		if( (procs + idx)->CPUusage == NO_VALUE )
			(procs + idx)->scores[sector] = 0.0;
		else
			(procs + idx)->scores[sector] = (procs + idx)->CPUusage;
	}


}

//PID���� ���Ͽ� �߿䵵�� ������ ����.
void eval4_pids_comp(ProcStats* procs, int num, int sector)
{
	int idx;
	float score;

	for(idx = 0; idx < num; idx++)
	{
		score = 20.0;	//�⺻ ������ 20���̴�.
	
		//���α׷� �׷��� �����ΰ�?
		if( (procs + idx)->pid == (procs + idx)->pgid )
			score += 30.0;

		//������ �����ΰ�?
		if( (procs + idx)->pgid == (procs + idx)->sid )
			score += 40.0;

		//�θ� init�̰ų�, �� ���μ��� ��ü�� init�ΰ�?
		if( (procs + idx)->ppid == 0 || (procs + idx)->ppid == 1 )
			score += 10.0;

		(procs + idx)->scores[sector] = score;
	}
}

//memory VSZ, RSS������ �̿��� �߿䵵�� ������ ����.
void eval5_memory(ProcStats* procs, int num, int sector)
{
	int idx;

	//RSS(���� ��� �޸�)�� VSZ(���� �޸�) ������ ��� �����ϴ�.
	//ȿ�������� ����ϴ� ������ ����.

	for(idx = 0; idx < num; idx++)
	{
		//VSZ�� RSS��-1�� ���� 20���� �ο��Ѵ�.
		if( (procs + idx)->VSZ == -1 || (procs + idx)->RSS == -1 )
			(procs + idx)->scores[sector] = 20.0;

		//�� ���� ���, RSS/VSZ���� ȿ�� ������ ����.
		else (procs + idx)->scores[sector] = ((float)((procs + idx)->RSS) / (float)((procs + idx)->VSZ)) * 100;
	}
}
