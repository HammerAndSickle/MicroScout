#! /bin/bash

# 주의! 이 스크립트는 권한 모드에 따라 실행이 불가능할 수도 있음.
# 실행되지 않으면 권한에 --x 플래그를 넣는다.

# /etc/passwd에서 유저들의 name만을 찾아온다.

USERLIST=`cat /etc/passwd | grep /bin/bash | cut -f 1 -d : | awk '{print $1}'`


# userstats.dat 파일이 있다면 파일을 지우도록 한 후, 새로 생성한다.
if [ -f userstats.dat ]; then
	rm userstats.dat
fi

# idx : user 중, 루트의 정보는 탐색하지 않는다. (첫 유저는 루트이다)
idx=0

# 유저들이 사용한 명령어들을 체크하여 빈도수를 파일에 저장한다.
for ITEM in $USERLIST
do
	if [ $idx == 0 ]	#첫 시행은 root이므로 제외한다.
	then
		idx=1
	else

		if [ ! -f /home/"$ITEM"/.bash_history ] 	#기록 파일이 없으면 모두 0으로 작성한다.
		then
			echo -n $ITEM >> userstats.dat
			echo -n " 0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo -n "0 " >> userstats.dat
			echo "0" >> userstats.dat
		else
			echo -n $ITEM >> userstats.dat		#이름을 출력
			echo -n " " >> userstats.dat

			#1---PERLS 빈도
			PERLS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'perl' | wc -w)

			#2---SUDO, SU 빈도
			SUDOES=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'sudo' | wc -w)
			SUS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'su' | wc -w)
			SUSUM=$(($SUDOES+$SUS))
	
			#3---디버거 빈도
			GDBS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gdb' | wc -w)

			#4---에디터 빈도
			GEDITS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gedit' | wc -w)
			VIS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'vi' | wc -w)
			EDITSUM=$(($GEDITS + $VIS))
		
			#5---컴파일러 빈도
			GCCS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'gcc' | wc -w)
			GPPS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'g++' | wc -w)
			JAVACS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'javac' | wc -w)
			COMPSUM=$(($GCCS + $GPPS + $JAVACS))
		
			#6---브라우저 빈도
			FF=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'firefox' | wc -w)
			CR=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'chrome' | wc -w)
			BROWSUM=$(($FF + $CR))
		
			#7---네트워크 빈도
			IFCFS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'ifconfig' | wc -w)
			ROUTS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'route' | wc -w)
			NETSTATS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'netstat' | wc -w)
			NETWORKSUM=$(($IFCFS + $ROUTS + $NETSTATS))
		
			#8---환경변수 빈도
			EXPORS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'export' | wc -w)
			ENVIRONS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'env' | wc -w)
			ENVS=$(($EXPORS + $ENVIRONS))
		
			#9---Kill 빈도
			KILLERS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'kill' | wc -w)
	
			#10---who 빈도
			FORGOTS=$(cat /home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'who' | wc -w)
	
	
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
	
			echo $FORGOTS >> userstats.dat
		fi
	fi
done
