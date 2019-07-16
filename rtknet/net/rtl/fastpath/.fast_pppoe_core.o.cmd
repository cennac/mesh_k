cmd_net/rtl/fastpath/fast_pppoe_core.o := msdk-linux-gcc -Wp,-MD,net/rtl/fastpath/.fast_pppoe_core.o.d  -nostdinc -isystem /opt/workspace/3.4.14/rtl819x/toolchain/msdk-4.8.5-mips-EL-3.10-u0.9.33-m32ut-180418/bin/../lib/gcc/mips-linux-uclibc/4.8.5/include -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include -Iarch/mips-ori/include/generated  -Iinclude -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/uapi -Iarch/mips-ori/include/generated/uapi -I/opt/workspace/3.4.14/rtl819x/linux-3.10/include/uapi -Iinclude/generated/uapi -include /opt/workspace/3.4.14/rtl819x/linux-3.10/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EL -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEL -D_MIPSEL -D__MIPSEL -D__MIPSEL__ -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/bsp -I/opt/workspace/3.4.14/rtl819x/linux-3.10/arch/mips-ori/include/asm/mach-generic        -I /opt/workspace/3.4.14/rtl819x/linux-3.10/net -D__KERNEL__    -c -o net/rtl/fastpath/fast_pppoe_core.o net/rtl/fastpath/fast_pppoe_core.S

source_net/rtl/fastpath/fast_pppoe_core.o := net/rtl/fastpath/fast_pppoe_core.S

deps_net/rtl/fastpath/fast_pppoe_core.o := \

net/rtl/fastpath/fast_pppoe_core.o: $(deps_net/rtl/fastpath/fast_pppoe_core.o)

$(deps_net/rtl/fastpath/fast_pppoe_core.o):
