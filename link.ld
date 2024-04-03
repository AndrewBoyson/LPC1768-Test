OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(main)

MEMORY
{
	flash   (rx) : ORIGIN = 0x00000000, LENGTH = 512K
	sram   (rwx) : ORIGIN = 0x10000000, LENGTH =  32K
	usbram ( rw) : ORIGIN = 0x2007c000, LENGTH =  16K
	ethram ( rw) : ORIGIN = 0x20080000, LENGTH =  16K
}

_stack = ORIGIN(sram) + LENGTH(sram); /* Define the top of our stack at the end of SRAM */

SECTIONS
{
	.usbram (NOLOAD) :
	{
	} >usbram
	
	.ethram (NOLOAD) :
	{
	} >ethram
	
	.text :
	{
		*(.vectors)
		*(.startup)
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
/*		*(.ARM*)
		*(.vfp11_veneer)
		*(.v4_bx)
*/
	} >flash
	
	.persist (NOLOAD) :
	{
	} >sram
	. = ALIGN(4);

	.data :
	{
		_sdata = .;
		*(.data)
		*(.data.*)
		_edata = .;
	} >sram AT >flash
	_sidata = LOADADDR(.data);
	. = ALIGN(4);

	.bss :
	{
		_sbss = .;
		*(.bss)
		*(.bss.*)
	} >sram
	. = ALIGN(4);
	_ebss = .;
	_heap = .;	/* Define our heap after data and bss sections in SRAM */
}

_end = .;
PROVIDE(end = .);

