#!/bin/bash
cd "$(dirname "$0")"

if [ "$(cat /etc/pibrick.lastbuild)" == "$(uname -r)" ]; then
	echo "No Linux Kernel Update."
else
	echo "Linux Kernel Changed. Rebuild"
#	apt install -y linux-headers-$(uname -r)

	make -j4 amoled
	make install &

	cd hyn_driver_release_qm
	make -j4 touch
	make install

	cd ../battery
	make
	make install
	
	echo "$(uname -r)" > /etc/pibrick.lastbuild

	if [ -f "/boot/firstrun.sh" ]; then
		echo "Firstrun build finished"
	else
		reboot
	fi
fi
