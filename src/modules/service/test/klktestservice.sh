#!/bin/sh
# Tests is there a service was run or not

if  ! ps aux | grep "$1" | grep -v "grep" | grep -v "$0"  > /dev/null ; then
	exit  1;
fi

exit 0;