#!/bin/bash

i=1
while [ $i -le 10000 ]
do
VAL=`expr $RANDOM / 12`
    echo  $VAL
    i=$(($i+1))
done

