cmd_drivers/net/rtl819x/rtl865x/../rtl83xx/port.o := msdk-linux-gcc -Wp,-MD,drivers/net/rtl819x/rtl865x/../rtl83xx/.port.o.d  -nostdinc -isystem /opt/workspace/3.4.14/rtl819x/toolchain/msdk-4.8.5-mips-EL-3.10-u0.9.33-m32ut-180418/bin/../lib/gcc/mips-linux-uclibc/4.8.5/include -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include -Iarch/mips-ori/include/generated  -Iinclude -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi -Iarch/mips-ori/include/generated/uapi -I/opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi -Iinclude/generated/uapi -include /opt/workspace/3.4.14/rtl819x/linux-3.10/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -std=gnu89 -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EL -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEL -D_MIPSEL -D__MIPSEL -D__MIPSEL__ -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -DRTL_TBLDRV -D__linux__ -mno-memcpy -DRTL865X_OVER_KERNEL -DRTL865X_OVER_LINUX -Wno-implicit -I/opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(port)"  -D"KBUILD_MODNAME=KBUILD_STR(port)" -c -o drivers/net/rtl819x/rtl865x/../rtl83xx/port.o drivers/net/rtl819x/rtl865x/../rtl83xx/port.c

source_drivers/net/rtl819x/rtl865x/../rtl83xx/port.o := drivers/net/rtl819x/rtl865x/../rtl83xx/port.c

deps_drivers/net/rtl819x/rtl865x/../rtl83xx/port.o := \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtk_switch.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtk_types.h \
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
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtk_error.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/port.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/string.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
  include/uapi/linux/types.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/types.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/posix_types.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/sgidefs.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/asm-generic/posix_types.h \
  /opt/workspace/3.4.14/rtl819x/toolchain/msdk-4.8.5-mips-EL-3.10-u0.9.33-m32ut-180418/lib/gcc/mips-linux-uclibc/4.8.5/include/stdarg.h \
  include/uapi/linux/string.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/string.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/cpu/r3000.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_reg.h \
    $(wildcard include/config/rst/offset.h) \
    $(wildcard include/config/rst/mask.h) \
    $(wildcard include/config/dummy/15/offset.h) \
    $(wildcard include/config/dummy/15/mask.h) \
    $(wildcard include/config/sel/offset.h) \
    $(wildcard include/config/sel/mask.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_base.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_port.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_unknownMulticast.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_phy.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_misc.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/drivers/net/rtl819x//rtl83xx/rtl8367c_asicdrv_portIsolation.h \
  include/linux/delay.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/linkage.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/linkage.h \
  include/linux/bitops.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/bitops.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/kvm/guest.h) \
    $(wildcard include/config/dma/noncoherent.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/linux/const.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/cpu-features.h \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/cpu.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp/cpu-feature-overrides.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp/war.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/arch_hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/printk/func.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
  include/linux/kern_levels.h \
  include/linux/dynamic_debug.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/errno.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/errno.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/asm-generic/errno-base.h \
  include/uapi/linux/kernel.h \
    $(wildcard include/config/rlx.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/linux/sysinfo.h \
    $(wildcard include/config/rtl/819x.h) \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/div64.h \
  include/asm-generic/div64.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/delay.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi/linux/param.h \
  /opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \

drivers/net/rtl819x/rtl865x/../rtl83xx/port.o: $(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/port.o)

$(deps_drivers/net/rtl819x/rtl865x/../rtl83xx/port.o):
