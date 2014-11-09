#!/bin/sh 

FC=identityTranslator
SRC2XML=../src/src2xml
XML2SRC=../src/xml2src
ROSECOMP=identityTranslator

export XEV_ENCODE="EUC-JP" # for testing h024.f90

if [ $# -ne 1 ]; then
    echo "USAGE: testXevXml.sh test.f90"
    exit 1
fi

SRC=$1
echo -n "Testing \"" 1>&2
echo -n $SRC 1>&2

case ${SRC} in
    *.f90)
	XML=`echo $SRC |sed 's/.f90$/.xml/'`
	DIF=`echo $SRC |sed 's/.f90$/.dif/'`
	LOG=`echo $SRC |sed 's/.f90$/.log/'`
	echo -n "\" r" 1>&2
	;;
    *.c)
	XML=`echo $SRC |sed 's/.c$/.xml/'`
	DIF=`echo $SRC |sed 's/.c$/.dif/'`
	LOG=`echo $SRC |sed 's/.c$/.log/'`
	echo -n "\"   r" 1>&2
	;;
esac


# error messages are written in 1
${FC} -rose:skip_syntax_check -c $SRC 2>$LOG
if [ $? -eq 0 ]
then 
    echo -n "s" 1>&2
    ${SRC2XML} $SRC 1> $XML 2>> $LOG
    if [ $? -eq 0 ]
    then 
	echo -n "x" 1>&2
	${XML2SRC} xev_$SRC < $XML  2>> $LOG
	if [ $? -eq 0 ]
	then 
	    echo -n "d" 1>&2
	    diff rose_$SRC xev_$SRC > $DIF 
	    if [ $? -eq 0 ]; 
	    then 
		echo -n " OK" 1>&2
		if [ -s $LOG ]
		then
		    echo " (warning)" 1>&2
		else
		    echo " " 1>&2
		fi
		exit 0; 
	    fi    
	else
	    echo -n " " 1>&2
	fi
    else
	echo -n "  " 1>&2
    fi
else
    echo -n "   " 1>&2
fi
 
echo -n " NG" 1>&2

if [ ! -e $DIF ]
then
    echo " (terminated)" 1>&2
elif [ -s $DIF ]
then
    echo " (mismatch)" 1>&2
elif [ -s $LOG ]
then
    echo " (warning)" 1>&2
fi
    
exit 0
