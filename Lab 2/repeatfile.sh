#!/bin/bash

if [[ $# -ne 3 ]]; then
	echo "Usage: $0 INPUT OUTPUT NUM"
	exit 1
fi

in=$1
out=$2
i=$3

if [[ ! -f $in ]]; then
	echo "File '$in' does not exist"
	exit 1
fi

while [[ $i -gt 0 ]]; do
	cat $in >> $out
	i=$(($i-1))
done
