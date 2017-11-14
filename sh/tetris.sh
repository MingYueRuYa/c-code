#!/bin/bash

APP_NAME="${0##*[\\/]}"
APP_VERSION="1.0"

#��ɫ����
iSumColor=7			#��ɫ����
cRed=1				#��ɫ
cGreen=2			#��ɫ
cYellow=3			#��ɫ
cBlue=4				#��ɫ
cFuchsia=5			#�Ϻ�ɫ
cCyan=6				#��ɫ(����ɫ)
cWhite=7			#��ɫ

#λ�����С
marginLeft=3			#�߿���߾�
marginTop=2			#�߿��ϱ߾�
((mapLeft=marginLeft+2))	#������߾�
((mapTop=$marginTop+1))		#�����ϱ߾�
mapWidth=10			#���̿��
mapHeight=15			#���̸߶�

#��ɫ����
cBorder=$cGreen
cScore=$cFuchsia
cScoreValue=$cCyan

#�����ź�
#��Ϸʹ���������̣�һ�����ڽ������룬һ��������Ϸ���̺���ʾ����;
#��ǰ�߽��յ��������ҵȰ���ʱ��ͨ������߷���signal�ķ�ʽ֪ͨ���ߡ�
sigRotate=25		#���ϼ�
sigLeft=26
sigRight=27
sigDown=28
sigAllDown=29		#�ո��
sigExit=30

#���鶨�壬7����19����ʽ
#ǰ8λΪ�������꣬��2λΪ����ճ��ֵ�ʱ���λ��
box0_0=(0 0 0 1 1 0 1 1 0 4)

box1_0=(0 1 1 1 2 1 3 1 0 3)
box1_1=(1 0 1 1 1 2 1 3 -1 3)

box2_0=(0 0 1 0 1 1 2 1 0 4)
box2_1=(0 1 0 2 1 0 1 1 0 3)

box3_0=(0 1 1 0 1 1 2 0 0 4)
box3_1=(0 0 0 1 1 1 1 2 0 4)

box4_0=(0 2 1 0 1 1 1 2 0 3)
box4_1=(0 1 1 1 2 1 2 2 0 3)
box4_2=(1 0 1 1 1 2 2 0 -1 3)
box4_3=(0 0 0 1 1 1 2 1 0 4)

box5_0=(0 0 1 0 1 1 1 2 0 3)
box5_1=(0 1 0 2 1 1 2 1 0 3)
box5_2=(1 0 1 1 1 2 2 2 -1 3)
box5_3=(0 1 1 1 2 0 2 1 0 4)

box6_0=(0 1 1 0 1 1 1 2 0 3)
box6_1=(0 1 1 1 1 2 2 1 0 3)
box6_2=(1 0 1 1 1 2 2 1 -1 3)
box6_3=(0 1 1 0 1 1 2 1 0 4)

iSumType=7			#������������
boxStyle=(1 2 2 2 4 4 4)	#���ַ�����ת����ܵ���ʽ��Ŀ

iScoreEachLevel=50	#����һ��������Ҫ�ķ���
#����ʱ����
sig=0			#���յ���signal
iScore=0		#�ܷ�
iLevel=0		#�ٶȼ�
boxNext=()		#��һ������
iboxNextColor=0		#��һ���������ɫ
iboxNextType=0		#��һ�����������
iboxNextStyle=0		#��һ���������ʽ
boxCur=()		#��ǰ�����λ�ö���
iBoxCurColor=0		#��ǰ�������ɫ
iBoxCurType=0		#��ǰ���������
iBoxCurStyle=0		#��ǰ�������ʽ
boxCurX=-1		#��ǰ�����x����λ��
boxCurY=-1		#��ǰ�����y����λ��
map=()			#����ͼ��

#��ʼ�����б�������Ϊ-1, ��ʾû�з���
for ((i = 0; i < mapHeight * mapWidth; i++))
do
	map[$i]=-1
done

#��������Ľ��̵�������
function RunAsKeyReceiver()
{
	local pidDisplayer key aKey sig cESC sTTY

	pidDisplayer=$1
	aKey=(0 0 0)

	cESC=`echo -ne "\033"`
	cSpace=`echo -ne "\040"`

	#�����ն����ԡ���read -s��ȡ�ն˼�ʱ���ն˵����Իᱻ��ʱ�ı䡣
	#�����read -sʱ���򱻲���ɱ�������ܻᵼ���ն˻��ң�
	#��Ҫ�ڳ����˳�ʱ�ָ��ն����ԡ�
	sTTY=`stty -g`

	#��׽�˳��ź�
	trap "MyExit;" INT QUIT
	trap "MyExitNoSub;" $sigExit

	#���ع��
	echo -ne "\033[?25l"

	while :
	do
		#��ȡ���롣ע-s�����ԣ�-n����һ���ַ���������
		read -s -n 1 key

		aKey[0]=${aKey[1]}
		aKey[1]=${aKey[2]}
		aKey[2]=$key
		sig=0

		#�ж������˺��ּ�
		if [[ $key == $cESC && ${aKey[1]} == $cESC ]]
		then
			#ESC��
			MyExit
		elif [[ ${aKey[0]} == $cESC && ${aKey[1]} == "[" ]]
		then
			if [[ $key == "A" ]]; then sig=$sigRotate	#<���ϼ�>
			elif [[ $key == "B" ]]; then sig=$sigDown	#<���¼�>
			elif [[ $key == "D" ]]; then sig=$sigLeft	#<�����>
			elif [[ $key == "C" ]]; then sig=$sigRight	#<���Ҽ�>
			fi
		elif [[ $key == "W" || $key == "w" ]]; then sig=$sigRotate	#W, w
		elif [[ $key == "S" || $key == "s" ]]; then sig=$sigDown	#S, s
		elif [[ $key == "A" || $key == "a" ]]; then sig=$sigLeft	#A, a
		elif [[ $key == "D" || $key == "d" ]]; then sig=$sigRight	#D, d
		elif [[ "[$key]" == "[]" ]]; then sig=$sigAllDown	#�ո��
		elif [[ $key == "Q" || $key == "q" ]]			#Q, q
		then
			MyExit
		fi

		if [[ $sig != 0 ]]
		then
			#����һ���̷�����Ϣ
			kill -$sig $pidDisplayer
		fi
	done
}

#�˳�ǰ�Ļָ�
MyExitNoSub()
{
	local y

	#�ָ��ն�����
	stty $sTTY
	((y = marginTop + mapHeight + 4))

	#��ʾ���
	echo -e "\033[?25h\033[${y};0H"
	exit
}

MyExit()
{
	#֪ͨ��ʾ������Ҫ�˳�
	kill -$sigExit $pidDisplayer

	MyExitNoSub
}

#������ʾ����Ϸ���̵�������
RunAsDisplayer()
{
	local sigThis
	InitDraw

	#���ظ����źŵĴ�����
	trap "sig=$sigRotate;" $sigRotate
	trap "sig=$sigLeft;" $sigLeft
	trap "sig=$sigRight;" $sigRight
	trap "sig=$sigDown;" $sigDown
	trap "sig=$sigAllDown;" $sigAllDown
	trap "ShowExit;" $sigExit

	while :
	do
		#���ݵ�ǰ���ٶȼ�iLevel��ͬ���趨��Ӧ��ѭ���Ĵ���
		for ((i = 0; i < 21 - iLevel; i++))
		do
			sleep 0.02
			sigThis=$sig
			sig=0

			#����sig�����ж��Ƿ���ܵ���Ӧ���ź�
			if ((sigThis == sigRotate)); then BoxRotate;	#��ת
			elif ((sigThis == sigLeft)); then BoxLeft;	#����һ��
			elif ((sigThis == sigRight)); then BoxRight;	#����һ��
			elif ((sigThis == sigDown)); then BoxDown;	#����һ��
			elif ((sigThis == sigAllDown)); then BoxAllDown;	#���䵽��
			fi
		done
		#kill -$sigDown $$
		BoxDown	#����һ��
	done
}

#���Ƶ�ǰ���飬����һ��������0��ʾ������ǰ���飬1��ʾ���Ƶ�ǰ����
DrawCurBox()
{
	local i x y bErase sBox
	bErase=$1
	if (( bErase == 0 ))
	then
		sBox="\040\040"		#�������ո����
	else
		sBox="[]"
		echo -ne "\033[1m\033[3${iBoxCurColor}m\033[4${iBoxCurColor}m"
	fi

	for ((i = 0; i < 8; i += 2))
	do
		((y = mapTop + 1 + ${boxCur[$i]} + boxCurY))
		((x = mapLeft + 1 + 2 * (boxCurX + ${boxCur[$i + 1]})))
		echo -ne "\033[${y};${x}H${sBox}"
	done
	echo -ne "\033[0m"
}

#�ƶ�����
#BoxMove(y, x), �����Ƿ���԰��ƶ��еķ����Ƶ�(y, x)��λ��, ����0�����, 1������
BoxMove()
{
	local i x y xPos yPos
	yPos=$1
	xPos=$2
	for ((i = 0; i < 8; i += 2))
	do
		#�����������������
		((y = yPos + ${boxCur[$i]}))
		((x = xPos + ${boxCur[$i + 1]}))

		if (( y < 0 || y >= mapHeight || x < 0 || x >= mapWidth))
		then
			#ײ��ǽ����
			return 1
		fi
		
		if (( ${map[y * mapWidth + x]} != -1 ))
		then
			#ײ�������Ѿ����ڵķ�����
			return 1
		fi
	done
	return 0;
}

#����������������
Box2Map()
{
	local i j x y line
	#����ǰ�ƶ��еķ����������̶�Ӧ������
	for ((i = 0; i < 8; i += 2))
	do
		#���㷽����������̵�����
		((y = ${boxCur[$i]} + boxCurY))
		((x = ${boxCur[$i + 1]} + boxCurX))
		map[y*mapWidth+x]=$iBoxCurColor	#��������ɫ������ͼ
	done

	line=0
	for ((i = 0; i < mapHeight; i++))
	do
		for ((j = 0; j < mapWidth; j++))
		do
			#����������п�϶������ѭ��
			[[ ${map[i*mapWidth+j]} -eq -1 ]] && break
		done

		[ $j -lt $mapWidth ] && continue
		#˵����ǰ�п���ȥ������ȥ������һ
		(( line++ ))

		#��i�пɱ���������0������i-1��ȫ������һ�У��ӵ�i-1�п�ʼ�ƶ�
		for ((j = i*mapWidth-1; j >= 0; j--))
		do
			((x = j + mapWidth))
			map[$x]=${map[$j]}
		done

		#��Ϊ����һ�У���0���ÿ�
		for ((i = 0; i < mapWidth; i++))
		do
			map[$i]=-1
		done
	done
	
	[ $line -eq 0 ] && return

	#������ȥ������line����������ٶȼ�
	((x = marginLeft + mapWidth * 2 + 7))
	((y = marginTop + 11))
	((iScore += line * 2 - 1))
	#��ʾ�µķ���
	echo -ne "\033[1m\033[3${cScoreValue}m\033[${y};${x}H${iScore}         "
	if ((iScore % iScoreEachLevel < line * 2 - 1))
	then
		if ((iLevel < 20))
		then
			((iLevel++))
			((y = marginTop + 14))
			#��ʾ�µ��ٶȼ�
			echo -ne "\033[3${cScoreValue}m\033[${y};${x}H${iLevel}        "
		fi
	fi
	echo -ne "\033[0m"

	#������ʾ��������
	for ((i = 0; i < mapHeight; i++))
	do
		#�����������Ļ������
		((y = i + mapTop + 1))
		((x = mapLeft + 1))
		echo -ne "\033[${y};${x}H"
		for ((j = 0; j < mapWidth; j++))
		do
			((tmp = i * mapWidth + j))
			if ((${map[$tmp]} == -1))
			then
				echo -ne "  "
			else
				echo -ne "\033[1m\033[3${map[$tmp]}m\033[4${map[$tmp]}m[]\033[0m"
			fi
		done
	done
}

#����һ��
BoxLeft()
{
	local x
	((x = boxCurX - 1))
	if BoxMove $boxCurY $x
	then
		DrawCurBox 0
		((boxCurX = x))
		DrawCurBox 1
	fi
}

#����һ��
BoxRight()
{
	local x
	((x = boxCurX + 1))
	if BoxMove $boxCurY $x
	then
		DrawCurBox 0
		((boxCurX = x))
		DrawCurBox 1
	fi
}

#������һ��
BoxDown()
{
	local y
	((y = boxCurY + 1))	#�µ�y����
	if BoxMove $y $boxCurX	#�����Ƿ��������һ��
	then
		DrawCurBox 0	#���ɵķ���Ĩȥ
		((boxCurY = y))
		DrawCurBox 1	#��ʾ�µ�����󷽿�
	else
		#�ߵ����, �������������
		Box2Map		#����ǰ�ƶ��еķ�����������������
		CreateBox	#�����µķ���
	fi
}

#���䵽��
BoxAllDown()
{
	local y iDown

	#�����ܹ����������
	iDown=0
	(( y = boxCurY + 1 ))
	while BoxMove $y $boxCurX
	do
		(( y++ ))
		(( iDown++ ))
	done

	DrawCurBox 0	#���ɵķ���Ĩȥ
	((boxCurY += iDown))
	DrawCurBox 1	#��ʾ�µ������ķ���
	Box2Map		#����ǰ�ƶ��еķ�����������������
	CreateBox	#�����µķ���
}

#��ת
BoxRotate()
{
	[ ${boxStyle[$iBoxCurType]} -eq 1 ] && return
	((rotateStyle = (iBoxCurStyle + 1) % ${boxStyle[$iBoxCurType]}))
	#����ǰ���鱣�浽boxTmp
	boxTmp=( `eval 'echo ${boxCur[@]}'` )
	boxCur=( `eval 'echo ${box'$iBoxCurType'_'$rotateStyle'[@]}'` )

	if BoxMove $boxCurY $boxCurX	#������ת���Ƿ��пռ�ŵ���
	then
		#Ĩȥ�ɵķ���
		boxCur=( `eval 'echo ${boxTmp[@]}'` )
		DrawCurBox 0

		boxCur=( `eval 'echo ${box'$iBoxCurType'_'$rotateStyle'[@]}'` )
		DrawCurBox 1
		iBoxCurStyle=$rotateStyle
	else
		#������ת�����Ǽ���ʹ���ϵ���ʽ
		boxCur=( `eval 'echo ${boxTmp[@]}'` )
	fi
}

#׼����һ������
PrepareNextBox()
{
	local i x y
	#����ұ�Ԥ��ʾ�ķ���
	if (( ${#boxNext[@]} != 0 )); then
		for ((i = 0; i < 8; i += 2))
		do
			((y = marginTop + 1 + ${boxNext[$i]}))
			((x = marginLeft + 2 * mapWidth + 7 + 2 * ${boxNext[$i + 1]}))
			echo -ne "\033[${y};${x}H\040\040"
		done
	fi

	#�������Ԥ��ʽ����
	(( iBoxNextType = RANDOM % iSumType ))
	(( iBoxNextStyle = RANDOM % ${boxStyle[$iBoxNextType]} ))
	(( iBoxNextColor = RANDOM % $iSumColor + 1 ))

	boxNext=( `eval 'echo ${box'$iBoxNextType'_'$iBoxNextStyle'[@]}'` )


	#��ʾ�ұ�Ԥ��ʾ�ķ���
	echo -ne "\033[1m\033[3${iBoxNextColor}m\033[4${iBoxNextColor}m"
	for ((i = 0; i < 8; i += 2))
	do
		((y = marginTop + 1 + ${boxNext[$i]}))
		((x = marginLeft + 2 * mapWidth + 7 + 2 * ${boxNext[$i + 1]}))
		echo -ne "\033[${y};${x}H[]"
	done

	echo -ne "\033[0m"

}

#��ʾ�·���
CreateBox()
{
	if (( ${#boxCur[@]} == 0 )); then
		#��ǰ���鲻����
		(( iBoxCurType = RANDOM % iSumType ))
		(( iBoxCurStyle = RANDOM % ${boxStyle[$iBoxCurType]} ))
		(( iBoxCurColor = RANDOM % $iSumColor + 1 ))
	else
		#��ǰ�����Ѵ���, ����һ�����鸳����ǰ����
		iBoxCurType=$iBoxNextType;
		iBoxCurStyle=$iBoxNextStyle;
		iBoxCurColor=$iBoxNextColor
	fi

	#��ǰ��������
	boxCur=( `eval 'echo ${box'$iBoxCurType'_'$iBoxCurStyle'[@]}'` )
	#��ʼ��������ʼ����
	boxCurY=boxCur[8];
	boxCurX=boxCur[9];

	DrawCurBox 1		#���Ƶ�ǰ����
	if ! BoxMove $boxCurY $boxCurX
	then
		kill -$sigExit $PPID
		ShowExit
	fi

	PrepareNextBox
	
}

#���Ʊ߿�
DrawBorder()
{
	clear

	local i y x1 x2
	#��ʾ�߿�
	echo -ne "\033[1m\033[3${cBorder}m\033[4${cBorder}m"

	((x1 = marginLeft + 1))				#��߿�x����
	((x2 = x1 + 2 + mapWidth * 2))			#�ұ߿�x����
	for ((i = 0; i < mapHeight; i++))
	do
		((y = i + marginTop + 2))
		echo -ne "\033[${y};${x1}H||"		#������߿�
		echo -ne "\033[${y};${x2}H||"		#�����ұ߿�
	done

	((x1 = marginTop + mapHeight + 2))
	for ((i = 0; i < mapWidth + 2; i++))
	do
		((y = i * 2 + marginLeft + 1))
		echo -ne "\033[${mapTop};${y}H=="	#�����ϱ߿�
		echo -ne "\033[${x1};${y}H=="		#�����±߿�
	done
	echo -ne "\033[0m"

	#��ʾ"Score"��"Level"����
	echo -ne "\033[1m"
	((y = marginLeft + mapWidth * 2 + 7))
	((x1 = marginTop + 10))
	echo -ne "\033[3${cScore}m\033[${x1};${y}HScore"
	((x1 = marginTop + 11))
	echo -ne "\033[3${cScoreValue}m\033[${x1};${y}H${iScore}"
	((x1 = marginTop + 13))
	echo -ne "\033[3${cScore}m\033[${x1};${y}HLevel"
	((x1 = marginTop + 14))
	echo -ne "\033[3${cScoreValue}m\033[${x1};${y}H${iLevel}"
	echo -ne "\033[0m"
}

InitDraw()
{
	clear			#����
	DrawBorder		#���Ʊ߿�
	CreateBox		#��������
}

#�˳�ʱ��ʾGameOVer!
ShowExit()
{
	local y
	((y = mapHeight + mapTop + 3))
	echo -e "\033[${y};1HGameOver!\033[0m"
	exit
}

#��Ϸ�������������ʼ.
if [[ "$1" == "--version" ]]; then
	echo "$APP_NAME $APP_VERSION"
elif [[ "$1" == "--show" ]]; then
	#�����־��в���--showʱ��������ʾ����
	RunAsDisplayer
else
	bash $0 --show&	#�Բ���--show��������������һ��
	RunAsKeyReceiver $!	#����һ�в����Ľ��̵Ľ��̺���Ϊ����
fi
