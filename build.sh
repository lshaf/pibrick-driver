#!/bin/bash
cd "$(dirname "$0")"
root="$(pwd)"

if [ "$(cat /etc/pibrick.lastbuild 2>/dev/null)" == "$(uname -r)" ]; then
	echo "No Linux Kernel Update."
	exit 0
fi

echo "Linux Kernel Changed. Rebuild for $(uname -r)"
#	apt install -y linux-headers-$(uname -r)

fail=0

# Each module: build + install; record failure but keep going so the others
# still get rebuilt. NOTE: the kernel is only marked "built" if ALL succeed,
# so a failed build is retried on the next boot instead of being lost.
build_one() {
	local label="$1"; shift   # remaining args: build target(s)
	echo ">>> building $label ..."
	if make "$@" && make install; then
		echo ">>> $label OK"
	else
		echo "!!! $label BUILD FAILED"
		fail=1
	fi
}

cd "$root";                       build_one amoled  -j4 amoled
cd "$root/hyn_driver_release_qm"; build_one touch   -j4 touch
cd "$root/battery";               build_one battery
cd "$root"

if [ "$fail" -ne 0 ]; then
	echo "!!! One or more pibrick modules failed to build."
	echo "!!! NOT marking $(uname -r) as built and NOT rebooting; will retry next boot."
	echo "!!! Check 'journalctl -u pibrick.service' and rebuild manually."
	exit 0
fi

echo "$(uname -r)" > /etc/pibrick.lastbuild
echo "All pibrick modules built for $(uname -r)."

if [ -f "/boot/firstrun.sh" ]; then
	echo "Firstrun build finished"
else
	reboot
fi
