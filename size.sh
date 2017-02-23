#!/bin/bash

if [ -z "$1" ]
then
    FILE=main.elf
else
    FILE=$1
fi

TOTAL_ROM=1000000
TOTAL_RAM=192000
ROM=$(arm-none-eabi-size $FILE | tail -1 | awk '{print $1+$3}')
RAM=$(arm-none-eabi-size $FILE | tail -1 | awk '{print $2+$3}')
ROM_PRC=$(echo "scale=2;" $ROM/$TOTAL_ROM*100 | bc -l)
RAM_PRC=$(echo "scale=2;" $RAM/$TOTAL_RAM*100 | bc -l)

echo "Memory used: "
echo -e "ROM:" $ROM/$TOTAL_ROM "\t"$ROM_PRC"%"
echo -e "RAM:" $RAM/$TOTAL_RAM "\t"$RAM_PRC"%"
