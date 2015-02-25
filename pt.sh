#! /bin/bash

# 주의! 이 스크립트는 권한 모드에 따라 실행이 불가능할 수도 있음.
# 실행되지 않으면 실행 권한을 높게 부여해 --x 플래그를 만든다.

# ps aux를 이용해 pid를 얻어온다.
PIDLIST=`ps -aux | awk '{print $2}'`

# ps aux를 이용해 %CPU를 배열에 저장한다.
CPULIST=($(ps -aux | awk '{print $3}'))

# idx : PIDLIST의 맨 앞의 "PID"나 CPULIST의 맨 앞의 "%CPU"를 걸러내기 위함.
idx=0


# 테스트삼아 정보들을 터미널에  출력한다. -> 테스트 완료.
# (필요한 정보들을 임의의 파일에 저장하여 프로그램에서 읽어들이도록 할 것이다.)
for ITEM in $PIDLIST
do
	if [ $idx == 0 ]
	then
		idx=1
	elif [ ! -d /proc/"$ITEM" ] || [ ! -f /proc/"$ITEM"/stat ] || [ ! -f /proc/"$ITEM"/status ]
	then
		idx=$((idx+1))
		continue 
	else
		
		# /proc에 프로세스 폴더가 존재하고, 관련 파일들도 존재하는 프로세스만 기록
		idx=$((idx+1))
		PARENTPID=`cat /proc/$ITEM/status | grep 'PPid' | awk '{print $2}'`
		SESSIONID=`cat /proc/$ITEM/stat | awk '{print $6}'`
		PRGRPID=`cat /proc/$ITEM/stat | awk '{print $5}'`
		FDSIZE=`cat /proc/$ITEM/status | grep 'FDSize' | awk '{print $2}'`
		VSZVALUE=`cat /proc/$ITEM/status | grep 'VmSize' | awk '{print $2}'`
		RSSVALUE=`cat /proc/$ITEM/status | grep 'VmRSS' | awk '{print $2}'`
		NICE=`cat /proc/$ITEM/stat | awk '{print $19}'`
		

		echo -n $ITEM >> procstats.dat		#1번째 : PID 기록
		echo -n " " >> procstats.dat
		
		#------------
		
		if [ "${CPULIST[idx]}" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n ${CPULIST[idx]}	>> procstats.dat	#2번째 : %CPU 기록
			echo -n " " >> procstats.dat
		fi

		#-------------
		
		if [ "$PARENTPID" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $PARENTPID >> procstats.dat	#3번째 : PPID 기록
			echo -n " " >> procstats.dat
		fi

		#------------
		
		if [ "$PRGRPID" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $PRGRPID >> procstats.dat	#4번째 : PGID 기록
			echo -n " " >> procstats.dat
		fi


		#------------
		
		if [ "$SESSIONID" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $SESSIONID >> procstats.dat	#5번째 : SID 기록
			echo -n " " >> procstats.dat
		fi


		#------------
		
		if [ "$VSZVALUE" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $VSZVALUE >> procstats.dat	#6번째 : VSZ 기록
			echo -n " " >> procstats.dat
		fi

		#------------
			
		if [ "$RSSVALUE" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $RSSVALUE >> procstats.dat	#7번째 : RSS 기록
			echo -n " " >> procstats.dat
		fi

		#---------------
		
		if [ "$FDSIZE" == "" ]
		then
			echo -n "-1" >> procstats.dat
			echo -n " " >> procstats.dat
		else
			echo -n $FDSIZE	>>procstats.dat	#8번째 : FDSIZE 기록
			echo -n " " >> procstats.dat
		fi

		
		#--------------

		if [ "$NICE" == "" ]
		then
			echo "-1" >> procstats.dat
		else
			echo $NICE >> procstats.dat		#9번째 : NICE 기록
		fi

		#--------------
	

	fi

done


# procstats.dat에 필요한 정보들이 쓰여지도록 함.
# 고려할 점) %CPU를 어떻게 읽어들일 것인가? float로? char로? char*로?  -> float으로 결정함.
# 만일 char로 읽겠다면, 소수로 나타내어지므로 '.' 문자가 있기 때문에 유의해야 한다.
