#!/bin/sh

#this script is used to insure the connection of 4G

route del default
insmod /root/4g/pcie_vcc.ko
sleep 15

cd /root/4g
while true
do
	pid=`pidof pppd call yuga`
	if [ $pid -gt 0 ];then
		echo "connect is still running"
	else
		echo "restart yuga connect"
		pppd call yuga &
	fi

	sleep 30
done

cd -
