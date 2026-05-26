#!/bin/bash

# Install piBrick autoBuild Kernel Modules
mkdir -p /usr/lib/pibrick/
cp -rf ./* /usr/lib/pibrick/
cp /usr/lib/pibrick/pibrick.service /etc/systemd/system/
chmod +x /usr/lib/pibrick/build.sh
systemctl daemon-reload
systemctl enable pibrick.service
cd /usr/lib/pibrick/
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
