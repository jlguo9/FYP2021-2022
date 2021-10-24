#!/bin/bash

if [ "$1" == "add" ]
then
	sudo tc qdisc add dev lo root handle 1: prio
	sudo tc qdisc add dev lo parent 1:1 handle 10: netem
	sudo tc filter add dev lo protocol ip parent 1: u32 match ip dport 12306 0xffff flowid 1:1
elif [ "$1" == "delete" ]
then
	sudo tc qdisc del dev lo root
	echo "Traffic control has been disabled"
elif [ "$1" == "delay" ]
then
	sudo tc qdisc change dev lo parent 1:1 handle 10: netem delay $2
elif [ "$1" == "loss" ]
then
	sudo tc qdisc change dev lo parent 1:1 handle 10: netem loss $2
elif [ "$1" == "show" ]
then
	sudo tc qdisc show dev lo
fi
