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

	PERLS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep 'perl' | wc -w

	SUDOES=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'sudo' | wc -w
	SUS=/home/"$ITEM"/.bash_history | awk '{print $1}' | grep -w 'su' | wc -w
	

	GDBS=/home/"$ITEM"/.bash_history | awk | wc -w
	


	EDITS=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	COMPILES=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	BROWSERS=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	NETWORKS=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	ENVIRONS=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	KILLERS=/home/"$ITEM"/.bash_history | grep 'perl' | wc -w
	
	echo -n /home/"$ITEM"/.bash_history | grep 'perl' | wc -w >> userstats.dat
	echo -n " " >> userstats.dat
done
