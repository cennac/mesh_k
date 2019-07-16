ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
else
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif

WSCSRC = $(shell pwd)

all:

romfs:
	$(ROMFSINST) $(WSCSRC)/wscd /bin/wscd
	$(ROMFSINST) $(WSCSRC)/wscd.conf /etc/wscd.conf
