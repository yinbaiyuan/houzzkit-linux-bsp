ifconfig wlan0 down
ifconfig eth0 down
pppd call quectel-ppp &

#ping -I pp0 baidu.com
