#!/bin/bash

RESULT_DIR=/userdata/rockchip-test
RESULT_LOG=${RESULT_DIR}/stressapptest.log

mkdir -p ${RESULT_DIR}

#get free memory size
mem_avail_size=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
mem_test_size=$((mem_avail_size/1024/2))

#run stressapptest_test
echo "*************************** DDR STRESSAPPTEST TEST 240H ***************************************"
echo "**run: stressapptest -s 864000 -i 4 -C 4 -W --stop_on_errors -M $mem_test_size -l $RESULT_LOG**"

stressapptest -s 864000 -i 4 -C 4 -W --stop_on_errors -M $mem_test_size -l $RESULT_LOG &

echo "************************** DDR STRESSAPPTEST START, LOG AT $RESULT_LOG ************************"
