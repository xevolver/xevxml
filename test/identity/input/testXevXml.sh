#!/bin/sh -

FC=/usr/local/rose-devel/bin/identityTranslator
SRC2XML=../../../mybuild/src/src2xml
XML2SRC=../../../mybuild/src/xml2src
ROSECOMP=/usr/local/rose-devel/bin/identityTranslator

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
	echo -n "\"\t\t\t r" 1>&2
	;;
    *.c)
	XML=`echo $SRC |sed 's/.c$/.xml/'`
	DIF=`echo $SRC |sed 's/.c$/.dif/'`
	LOG=`echo $SRC |sed 's/.c$/.log/'`
	echo -n "\"\t\t\t   r" 1>&2
	;;
    *.F)
	XML=`echo $SRC |sed 's/.F$/.xml/'`
	DIF=`echo $SRC |sed 's/.F$/.dif/'`
	LOG=`echo $SRC |sed 's/.F$/.log/'`
	echo -n "\"\t\t\t   r" 1>&2
	;;
esac
OUT=../output

# error messages are written in 1
${FC} -rose:skip_syntax_check -rose:skipfinalCompileStep -c $SRC -rose:o $OUT/rose_$SRC 2> $OUT/$LOG
if [ $? -eq 0 ]
then
    echo -n "s" 1>&2
    ${SRC2XML} $SRC 1> $OUT/$XML 2>> $OUT/$LOG
    if [ $? -eq 0 ]
    then
	echo -n "x" 1>&2
	${XML2SRC} $OUT/xev_$SRC < $OUT/$XML  2>> $OUT/$LOG
	if [ $? -eq 0 ]
	then
	    echo -n "d" 1>&2
	    diff $OUT/rose_$SRC $OUT/xev_$SRC > $OUT/$DIF
	    if [ $? -eq 0 ];
	    then
		echo -n " OK" 1>&2
		if [ -s $OUT/$LOG ]
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

if [ ! -e $OUT/$DIF ]
then
    echo " (terminated)" 1>&2
    exit -1
elif [ -s $OUT/$DIF ]
then
    echo " (mismatch)" 1>&2
    exit -1
elif [ -s $OUT/$LOG ]
then
    echo " (warning)" 1>&2
fi

exit 0