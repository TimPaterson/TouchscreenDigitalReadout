//****************************************************************************
// Definitions of FAT file system
//****************************************************************************

#pragma once

#define FAT_BPB_START		11	// offset in boot sector of BPB
#define FAT_PARTITION_START	446	// offset in MBR of partition table
#define FAT_SIG_OFFSET		510
#define FAT_SIGNATURE		0xAA55
#define FAT_YEAR_BASE		1980
#define FAT_MAX_YEAR		(FAT_YEAR_BASE + 127)


// Partition table entry
struct FatPartitionEnt
{
	byte	BootInd;
	byte	FirstHead;
	byte	FirstSector;
	byte	FirstTrack;
	byte	FileSystem;
	byte	LastHead;
	byte	LastSector;
	byte	LastTrack;
	ulong	StartSector;
	ulong	TotalSectors;
};

// The common part of the BPB
#pragma pack(push, 1)
struct FatBpb
{
	ushort	BytsPerSec;
	byte	SecPerClus;
	ushort	RsvdSecCnt;
	byte	NumFATs;
	ushort	RootEntCnt;
	ushort	TotSec16;
	byte	Media;
	ushort	FATSz16;
	ushort	SecPerTrk;
	ushort	NumHeads;
	ulong	HiddSec;
	ulong	TotSec32;
};

// FAT32 extension of the BPB
struct FatBpb32
{
	ulong	FATSz32;
	ushort	ExtFlags;
	ushort	FSVer;
	ulong	RootClus;
	ushort	FSInfo;
	ushort	BkBootSec;
	byte	Reserved[12];
};

// Boot sector following the BPB
struct FatBootSecTail
{
	byte	DrvNum;
	byte	Reserved1;
	byte	BootSig;
	ulong	VolID;
	byte	VolLab[11];
	byte	FilSysType[8];
};

// Full layout of the boot sector
struct FatBootSect
{
	byte		BS_jmpBoot[3];
	byte		BS_OEMName[8];
	FatBpb		Bpb;
	union
	{
		FatBpb32		Bpb32;
		FatBootSecTail	Tail16;
	};
	FatBootSecTail	Tail32;
};
#pragma pack(pop)

//****************************************************************************
// Directory entry layout for short file names
union FatTime
{
	ushort	wb;
	struct
	{
		ushort	seconds:5;
		ushort	minutes:6;
		ushort	hours:5;
	};
};

union FatDate
{
	ushort	wb;
	struct
	{
		ushort	day:5;
		ushort	month:4;
		ushort	year:7;
	};
};

union FatDateTime
{
	ulong	ul;
	struct
	{
		FatTime	time;
		FatDate	date;
	};
};

struct FatShortDirEnt
{
	char	Name[11];
	byte	Attr;
	byte	CaseFlags;
	byte	CrtTimTenth;
	FatTime	CrtTime;
	FatDate	CrtDate;
	FatDate	LastAccDate;
	ushort	FirstClusHi;
	FatTime	WriteTime;
	FatDate	WriteDate;
	ushort	FirstClusLo;
	ulong	FileSize;
};

// Directory entry layout for long file names
#pragma pack(push, 1)
struct FatLongDirEnt
{
	byte	Order;
	ushort	Name1[5];
	byte	Attr;
	byte	Type;			// Always zero
	byte	Checksum;
	ushort	Name2[6];
	ushort	FirstClusLo;	// Always zero
	ushort	Name3[2];
};
#pragma pack(pop)

union FatDirEnt
{
	FatShortDirEnt	Short;
	FatLongDirEnt	Long;
};

// Attribute bits
#define	ATTR_READ_ONLY	0x01
#define	ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define ATTR_LONG_NAME_MASK	(ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE)

// CaseFlags bits
#define CASE_EXT_LOWER	0x10
#define CASE_BASE_LOWER	0x08

#define LAST_LONG_ENTRY		0x40
#define CHAR_PER_LONG_NAME	13
#define CHAR_PER_SHORT_NAME	11
#define	EMPTY_DIR_ENTRY		0xE5
#define END_DIR_ENTRY		0
#define	KANJI_LEAD_BYTE		0x05

#define FAT12_EOC		0xFFF
#define FAT16_EOC		0xFFFF
#define FAT32_EOC		0xFFFFFFFL

#define FAT12_EOC_MIN	0xFF8
#define FAT16_EOC_MIN	0xFFF8
#define FAT32_EOC_MIN	0xFFFFFF8L

#define FAT12_MAX_CLUS	4085
#define FAT16_MAX_CLUS	65525
