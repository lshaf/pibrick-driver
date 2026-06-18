#!/bin/bash

# ---------------------------------------------------------------------------
# Dependencies. The driver rebuilds its out-of-tree kernel modules on every
# kernel bump (build.sh), and the button service shells out to a few userspace
# tools at runtime. Install both up front so the first build + first boot work.
#
#   build-essential      gcc + make — compile pibrickbtn and the kernel modules
#   linux-headers-*      kbuild tree for the running kernel; without it `make`
#                        in battery/hyn_driver_release_qm/amoled has no
#                        /lib/modules/$(uname -r)/build to build against
#   device-tree-compiler dtc — Makefile 'amoled' target compiles the DSI .dtbo
#   gpiod                gpiomon/gpioget — pibrickbtn polls the buttons via these
#   evemu-tools          evemu-event — power-short.sh injects KEY_POWER
#   wlr-randr            wlr-randr — optional wlroots display-toggle action script
# ---------------------------------------------------------------------------
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y \
	build-essential \
	linux-headers-"$(uname -r)" \
	device-tree-compiler \
	gpiod \
	evemu-tools \
	wlr-randr

# Install piBrick autoBuild Kernel Modules + Button Service
mkdir -p /usr/lib/pibrick/
cp -rf ./* /usr/lib/pibrick/
cp /usr/lib/pibrick/pibrick.service /etc/systemd/system/
chmod +x /usr/lib/pibrick/build.sh

# Button action scripts live in /etc/pibrick (user-editable). Stage them before
# the service starts so the button monitor can find them on first press.
cp -r /usr/lib/pibrick/button/etc/pibrick /etc/

systemctl daemon-reload
systemctl enable pibrick.service
cd /usr/lib/pibrick/
# build.sh builds the kernel modules and compiles the pibrickbtn binary;
# pibrick.service then runs that binary (the button monitor) as ExecStart.
/usr/lib/pibrick/build.sh
systemctl start pibrick.service

# ---------------------------------------------------------------------------
# Let the logged-in user edit the Vial keyboard from a browser (WebHID).
# Without this, /dev/hidraw* is root-only and Vial fails with
# "NotAllowedError: Failed to open the device".
# ---------------------------------------------------------------------------
cat > /etc/udev/rules.d/99-vial.rules <<'EOF'
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{serial}=="*vial:f64c2b3c*", MODE="0660", GROUP="users", TAG+="uaccess", TAG+="udev-acl"
EOF
udevadm control --reload-rules
udevadm trigger --subsystem-match=hidraw
