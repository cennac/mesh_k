cmd_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o := msdk-linux-gcc -Wp,-MD,drivers/net/rtl819x/rtl865x/../rtl83xx/.rtl8367c_phy_test.o.d  -nostdinc -isystem /opt/workspace/3.4.14/rtl819x/toolchain/msdk-4.8.5-mips-EL-3.10-u0.9.33-m32ut-180418/bin/../lib/gcc/mips-linux-uclibc/4.8.5/include -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include -Iarch/mips-ori/include/generated  -Iinclude -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi -Iarch/mips-ori/include/generated/uapi -I/opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi -Iinclude/generated/uapi -include /opt/workspace/3.4.14/rtl819x/linux-3.10/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -std=gnu89 -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EL -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEL -D_MIPSEL -D__MIPSEL -D__MIPSEL__ -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -DRTL_TBLDRV -D__linux__ -mno-memcpy -DRTL865X_OVER_KERNEL -DRTL865X_OVER_LINUX -Wno-implicit -I/opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl8367c_phy_test)"  -D"KBUILD_MODNAME=KBUILD_STR(rtl8367c_phy_test)" -c -o drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.c

source_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o := drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.c

deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o := \
  drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv.h \
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
  drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_asicdrv_phy.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv.h \
  drivers/net/rtl819x/rtl865x/../rtl83xx/port.h \
  drivers/net/rtl819x/rtl865x/../rtl83xx/rtk_switch.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/rtl/debug/counter.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/irqflags.h \
    $(wildcard include/config/cpu/mipsr2.h) \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/irq/cpu.h) \
    $(wildcard include/config/64bit.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  include/linux/stringify.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/hazards.h \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/bmips.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/xlr.h) \
    $(wildcard include/config/cpu/sb1.h) \

drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o: $(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o)

$(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/rtl8367c_phy_test.o):
