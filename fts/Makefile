
obj-m += ft5x06.o
MODEL_FILE := /proc/device-tree/model
MODEL_EXISTS := $(wildcard $(MODEL_FILE))
MODULE_NAME := ft5x06.ko
MODULE_INSTALL_DIR := /lib/modules/$(shell uname -r)/kernel/drivers/input/touchscreen


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
