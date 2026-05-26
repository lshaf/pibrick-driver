
obj-m += hyn_ts.o
MODEL_FILE := /proc/device-tree/model
MODEL_EXISTS := $(wildcard $(MODEL_FILE))
MODULE_NAME := hyn_ts.ko
MODULE_INSTALL_DIR := /lib/modules/$(shell uname -r)/kernel/drivers/input/touchscreen

hyn_ts-objs += hyn_core.o hyn_lib/hyn_i2c.o hyn_lib/hyn_spi.o hyn_lib/hyn_ts_ext.o hyn_lib/hyn_fs_node.o
hyn_ts-objs += hyn_lib/hyn_tool.o
hyn_ts-objs += hyn_lib/hyn_gesture.o
hyn_ts-objs += hyn_lib/hyn_prox.o
hyn_ts-objs += hyn_chips/hyn_cst66xx.o
hyn_ts-objs += hyn_chips/hyn_cst3240.o
hyn_ts-objs += hyn_chips/hyn_cst92xx.o
hyn_ts-objs += hyn_chips/hyn_cst3xx.o
hyn_ts-objs += hyn_chips/hyn_cst7xx.o
hyn_ts-objs += hyn_chips/hyn_cst8xxT.o
hyn_ts-objs += hyn_chips/hyn_cst226se.o
hyn_ts-objs += hyn_chips/hyn_cst36xxes.o

modules:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

touch: modules


install: remove
	install -m 644 -D $(MODULE_NAME) $(MODULE_INSTALL_DIR)
	depmod -a

remove:
	rm -rf $(MODULE_INSTALL_DIR)/$(MODULE_NAME)
