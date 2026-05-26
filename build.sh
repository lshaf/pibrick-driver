#!/bin/bash
cd "$(dirname "$0")"
root="$(pwd)"

# Button monitor: compile the pibrickbtn daemon that pibrick.service's
# ExecStart runs (the monitor_keydown loop). Cheap, and independent of the
# kernel version, so do it on every boot if the source is newer than the
# installed binary (or the binary is missing).
btn_src="$root/button/pibrickbtn.c"
btn_bin="/usr/local/bin/pibrickbtn"
if [ -f "$btn_src" ] && { [ ! -x "$btn_bin" ] || [ "$btn_src" -nt "$btn_bin" ]; }; then
	echo ">>> compiling pibrickbtn ..."
	if gcc "$btn_src" -o "$btn_bin"; then
		chmod +x "$btn_bin"
		echo ">>> pibrickbtn OK"
	else
		echo "!!! pibrickbtn BUILD FAILED"
	fi
fi

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
