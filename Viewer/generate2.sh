#!/bin/bash

i=1
while [ $i -le 10000 ]
do
    echo  $RANDOM
    i=$(($i+1))
done

