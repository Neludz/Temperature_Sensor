
/* AUTO-GENERATED by gen_isr_tables.py, do not edit! */

#include <toolchain.h>
#include <linker/sections.h>
#include <sw_isr_table.h>
#include <arch/cpu.h>

#if defined(CONFIG_GEN_SW_ISR_TABLE) && defined(CONFIG_GEN_IRQ_VECTOR_TABLE)
#define ISR_WRAPPER ((uintptr_t)&_isr_wrapper)
#else
#define ISR_WRAPPER NULL
#endif

typedef void (* ISR)(const void *);
uintptr_t __irq_vector_table _irq_vector_table[48] = {
	ISR_WRAPPER,
	289027,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	288971,
	ISR_WRAPPER,
	ISR_WRAPPER,
	289001,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
};
struct _isr_table_entry __sw_isr_table _sw_isr_table[48] = {
	{(const void *)0x47eab, (ISR)0x47df5},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x4e5dc, (ISR)0x46257},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x1b93d, (ISR)0x47df5},
	{(const void *)0x4e5f4, (ISR)0xf89d},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x4e57c, (ISR)0x45e23},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x200055f0, (ISR)0x11a11},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)0x11f51},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)0x121d9},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x1df11, (ISR)0x47df5},
	{(const void *)0x4e5c4, (ISR)0x46257},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
	{(const void *)0x0, (ISR)&z_irq_spurious},
};
