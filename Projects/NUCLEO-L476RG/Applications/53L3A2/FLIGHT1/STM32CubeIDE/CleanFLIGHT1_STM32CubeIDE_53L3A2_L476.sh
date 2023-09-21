#!/bin/bash

######## Modify this Section:
# 1) Set the Installation path for STM32CubeProgrammer
# example:
#CubeProg="/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI"
CubeProg="STM32_Programmer_CLI"

## Run section

echo "/******************************************/"
echo "           Clean FP-SNS-FLIGHT1"
echo "/******************************************/"
echo "              Full Chip Erase"
echo "/******************************************/"
"${CubeProg}" -c port=swd mode=UR reset=HWrst -e all
echo "/******************************************/"
echo "              Install BootLoader"
echo "/******************************************/"
"${CubeProg}" -c port=swd mode=UR reset=HWrst -d ../../../../../../Utilities/BootLoader/STM32F4xxRE/BootLoader_L476.bin 0x08000000 -v
echo "/******************************************/"
echo "          Install FP-SNS-FLIGHT1"
echo "/******************************************/"
"${CubeProg}" -c port=swd mode=UR reset=HWrst -d ./Debug/FLIGHT1.bin 0x08004000 -v
echo "/******************************************/"
echo "      Dump FP-SNS-FLIGHT1 + BootLoader"
echo "/******************************************/"
SizeBinBL=`ls -l ./Debug/FLIGHT1.bin | awk '{print $5+0x4000};'`
"${CubeProg}" -c port=swd mode=UR reset=HWrst -u 0x08000000 ${SizeBinBL} ./Debug/FLIGHT1_BL.bin
echo "/******************************************/"
echo "                Reset STM32"
echo "/******************************************/"
"${CubeProg}" -c port=swd mode=UR reset=HWrst -rst