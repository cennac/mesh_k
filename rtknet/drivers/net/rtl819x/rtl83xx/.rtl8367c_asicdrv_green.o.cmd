cmd_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o := msdk-linux-gcc -Wp,-MD,drivers/net/rtl819x/rtl865x/../rtl83xx/.rtl8367c_asicdrv_green.o.d  -nostdinc -isystem /opt/workspace/3.4.14/rtl819x/toolchain/msdk-4.8.5-mips-EL-3.10-u0.9.33-m32ut-180418/bin/../lib/gcc/mips-linux-uclibc/4.8.5/include -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include -Iarch/mips-ori/include/generated  -Iinclude -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi -Iarch/mips-ori/include/generated/uapi -I/opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi -Iinclude/generated/uapi -include /opt/workspace/3.4.14/rtl819x/linux-3.10/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -std=gnu89 -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EL -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEL -D_MIPSEL -D__MIPSEL -D__MIPSEL__ -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -DRTL_TBLDRV -D__linux__ -mno-memcpy -DRTL865X_OVER_KERNEL -DRTL865X_OVER_LINUX -Wno-implicit -I/opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl8367c_asicdrv_green)"  -D"KBUILD_MODNAME=KBUILD_STR(rtl8367c_asicdrv_green)" -c -o drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.c

source_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o := drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.c

deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o := \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_green.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtk_types.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtk_error.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_reg.h \
    $(wildcard include/config/rst/offset.h) \
    $(wildcard include/config/rst/mask.h) \
    $(wildcard include/config/dummy/15/offset.h) \
    $(wildcard include/config/dummy/15/mask.h) \
    $(wildcard include/config/sel/offset.h) \
    $(wildcard include/config/sel/mask.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_base.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_phy.h \

drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o: $(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o)

$(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_green.o):
