#!/bin/bash
#This is gpio_test script


#pull up gpio0
echo 1 > /proc/rp_gpio/gpio0

#pull down gpio0
echo 0 > /proc/rp_gpio/gpio0
