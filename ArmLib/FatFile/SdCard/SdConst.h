#pragma once

#define SDCARD_BLOCK_POWER	9
#define SDCARD_BLOCK_SIZE	(1 << SDCARD_BLOCK_POWER)

// SD card commands
#define SDCARD_GO_IDLE_STATE		0
#define SDCARD_SEND_IF_COND			8
#define SDCARD_SEND_CSD				9
#define SDCARD_SEND_CID				10
#define SDCARD_SEND_STATUS			13
#define SDCARD_READ_BLOCK			17
#define SDCARD_READ_MULTIPLE_BLOCK	18
#define SDCARD_WRITE_BLOCK			24
#define SDCARD_WRITE_MULTIPLE_BLOCK	25
#define SDCARD_READ_OCR				58
#define SDCARD_CRC_ON_OFF			59
#define SDCARD_APP_CMD				55
#define SDCARD_APP_SEND_OP_COND		41

// SD card constants
#define SDCARD_START_TOKEN		0xFE
#define SDCARD_START_MULWRITE	0xFC
#define SDCARD_STOP_TRAN		0xFD
#define SDCARD_GO_IDLE_CRC		0x95
#define SDCARD_VHS				0x01
#define SDCARD_CHECK_PATTERN	0xAA
#define SDCARD_IF_COND_VHS_ARG	((SDCARD_VHS << 8) | SDCARD_CHECK_PATTERN)
#define SDCARD_IF_COND_CRC		0x87
#define SDCARD_OP_COND_HCS_ARG	(1L << 30)
#define SDCARD_R3_MSB_CCS		0x40

// Response format R1
#define SDCARD_R1_READY			0x00
#define SDCARD_R1_IDLE			0x01
#define SDCARD_R1_ERASE_RESET	0x02
#define SDCARD_R1_BAD_CMD		0x04
#define SDCARD_R1_CRC_ERR		0x08
#define SDCARD_R1_ERASE_SEQ		0x10
#define SDCARD_R1_ADDR_ERR		0x20
#define SDCARD_R1_ARG_ERR		0x40

// Response format R2 (2nd byte)
#define SDCARD_R2_CARD_LOCKED	0x01
#define SDCARD_R2_WRITE_PROT	0x22
#define SDCARD_R2_ERASE_ERR		0x40
#define SDCARD_R2_ECC_ERR		0x10
#define SDCARD_R2_ERROR			0x0C

// Data Error Token on read
#define SDCARD_ERRTOK_GENERAL	0x01
#define SDCARD_ERRTOK_CONTROL	0x02
#define SDCARD_ERRTOK_BAD_DATA	0x04
#define SDCARD_ERRTOK_BAD_ADDR	0x08

// Data Response Token on write
#define SDCARD_DATRESP_MASK		0x1F
#define SDCARD_DATRESP_ACCEPTED	0x05
#define SDCARD_DATRESP_CRC_ERR	0x0B
#define SDCARD_DATRESP_WRITE_ERR 0x0D

//****************************************************************************
// Note that SD Card documentation defines bitfield from the MSB,
// while the compiler assigns them from the LSB. Also the SD Card
// spec is big-endian byte order, while the ARM is little-endian.
// This means that any fields split across a byte have to be
// re-assembled with explicit code.

struct SdCard_CSD_1
{
	// Blank line indicates byte boundary in bit fields
	byte	CSD_STRUCTURE;
	byte	TAAC;
	byte	NSAC;
	byte	TRAN_SPEED;

	byte	CCC_hi;

	byte	READ_BL_LEN:4;
	byte	CCC_lo:4;

	byte	C_SIZE_hi:2;
	byte	:2;
	byte	DSR_IMP:1;
	byte	READ_BLK_MISALIGN:1;
	byte	WRITE_BLK_MISALIGN:1;
	byte	READ_BL_PARTIAL:1;

	byte	C_SIZE_mid;

	byte	VDD_R_CURR_MAX:3;
	byte	VDD_R_CURR_MIN:3;
	byte	C_SIZE_lo:2;

	byte	C_SIZE_MULT_hi:2;
	byte	VDD_W_CURR_MAX:3;
	byte	VDD_W_CURR_MIN:3;

	byte	SECTOR_SIZE_hi:6;
	byte	ERASE_BLK_EN:1;
	byte	C_SIZE_MULT_lo:1;

	byte	WP_GRP_SIZE:7;
	byte	SECTOR_SIZE_lo:1;

	byte	WRITE_BL_LEN_hi:2;
	byte	R2W_FACTOR:3;
	byte	:2;
	byte	WP_GRP_ENABLE:1;

	byte	:5;
	byte	WRITE_BL_PARTIAL:1;
	byte	WRITE_BL_LEN_lo:2;

	byte	:2;
	byte	FILE_FORMAT:2;
	byte	TMP_WRITE_PROTECT:1;
	byte	PERM_WRITE_PROTECT:1;
	byte	COPY:1;
	byte	FILE_FORMAT_GRP:1;

	byte	:1;
	byte	CRC7:7;

	// Accessors recombine bit fields
	ushort	C_SIZE() {return (C_SIZE_hi << 10) | (C_SIZE_mid << 2) | C_SIZE_lo;}
	byte	C_SIZE_MULT() {return (C_SIZE_MULT_hi << 1) | C_SIZE_MULT_lo;}
	byte	SECTOR_SIZE() {return (SECTOR_SIZE_hi << 1) | SECTOR_SIZE_lo;}
	byte	WRITE_BL_LEN() {return (WRITE_BL_LEN_hi << 2) | WRITE_BL_LEN_lo;}

	// Get size of card in 512-byte units
	ulong	CardSize() {return ((ulong)(C_SIZE() + 1)) << (C_SIZE_MULT() + 2 + READ_BL_LEN - 9);}
};

struct SdCard_CSD_2
{
	// Blank line indicates byte boundary
	byte	CSD_STRUCTURE;
	byte	TAAC;
	byte	NSAC;
	byte	TRAN_SPEED;

	byte	CCC_hi;

	byte	READ_BL_LN:4;
	byte	CCC_lo:4;

	byte	:4;
	byte	DSR_IMP:1;
	byte	READ_BLK_MISALIGN:1;
	byte	WRITE_BLK_MISALIGN:1;
	byte	READ_BL_PARTIAL:1;

	byte	C_SIZE_hi;
	byte	C_SIZE_mid;
	byte	C_SIZE_lo;

	byte	SECTOR_SIZE_hi:6;
	byte	ERASE_BLK_EN:1;
	byte	:1;

	byte	WP_GRP_SIZE:7;
	byte	SECTOR_SIZE_lo:1;

	byte	WRITE_BL_LEN_hi:2;
	byte	R2W_FACTOR:3;
	byte	:2;
	byte	WP_GRP_ENABLE:1;

	byte	:5;
	byte	WRITE_BL_PARTIAL:1;
	byte	WRITE_BL_LEN_lo:2;

	byte	:2;
	byte	FILE_FORMAT:2;
	byte	TMP_WRITE_PROTECT:1;
	byte	PERM_WRITE_PROTECT:1;
	byte	COPY:1;
	byte	FILE_FORMAT_GRP:1;

	byte	:1;
	byte	CRC7:7;

	// Accessors recombine bit fields
	ulong	C_SIZE() {return ((ulong)C_SIZE_hi << 16) | ((uint)C_SIZE_mid << 8) | C_SIZE_lo;}

	// Get size of card in 512-byte units
	ulong	CardSize() {return C_SIZE();}
};

struct SdCard_CID
{
};
