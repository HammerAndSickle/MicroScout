#! /bin/bash

# 주의! 이 스크립트는 권한 모드에 따라 실행이 불가능할 수도 있음.
# 실행되지 않으면 권한에 --x 플래그를 넣는다.

# /etc/passwd에서 유저들의 name만을 찾아온다.
USERLIST=`cat /etc/passwd | grep /bin/bash | cut -f 1 -d :`

# userstats.dat 파일이 있다면 파일을 지우도록 한 후, 새로 생성한다.
if [ -f userstats.dat ]; then
	rm userstats.dat
fi

# 유저들이 사용한 명령어들을 체크하여 빈도수를 파일에 저장한다.
for ITEM in $USERLIST
do
	echo -n $ITEM >> userstats.dat		#이름을 출력
	echo -n " " >> userstats.dat

	#1---PERLS 빈도
	PERLS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'perl' | wc -w

	#2---SUDO, SU 빈도
	SUDOES=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'sudo' | wc -w
	SUS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'su' | wc -w
	SUSUM=$(($SUDOES + $SUS))
	
	#3---디버거 빈도
	GDBS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gdb' | wc -w

	#4---에디터 빈도
	GEDITS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gedit' | wc -w
	VIS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'vi' | wc -w
	EDITSUM=$(($GEDITS + $VIS))
	
	#5---컴파일러 빈도
	GCCS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gcc' | wc -w
	GPPS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'g++' | wc -w
	JAVACS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'javac' | wc -w
	COMPSUM=$(($GCCS + $GPPS + $JAVACS))
	
	#6---브라우저 빈도
	FF=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'firefox' | wc -w
	CR=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'chrome' | wc -w
	BROWSUM=$(($FF + $CR))
	
	#7---네트워크 빈도
	IFCFS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'ifconfig' | wc -w
	ROUTS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'route' | wc -w
	NETSTATS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'netstat' | wc -w
	NETWORKSUM=$(($IFCFS + $ROUTS + $NETSTATS))
	
	#8---환경변수 빈도
	EXPORS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'export' | wc -w
	ENVIRONS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'env' | wc -w
	ENVS=$(($EXPORS + $ENVIRONS))
	
	#9---Kill 빈도
	KILLERS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'kill' | wc -w
	
	#10---who 빈도
	FORGOTS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'who' | wc -w
	
	
	echo -n $PERLS >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $SUSUM >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $GDBS >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $EDITSUM >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $COMPSUM >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $BROWSUM >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $NETWORKSUM >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $ENVS >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $KILLERS >> userstats.dat
	echo -n " " >> userstats.dat
	
	echo -n $FORGOTS >> userstats.dat
	echo -n " " >> userstats.dat

done
