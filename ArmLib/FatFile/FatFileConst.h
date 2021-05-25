#pragma once

#include <FatFile\Storage.h>

#define	FAT_SECT_SIZE	512

// flags for Open()
enum OpenFlags
{
	OPENFLAG_CreateBits		= 0x07,
	OPENFLAG_OpenExisting	= 0x00,	// Must exist
	OPENFLAG_CreateAlways	= 0x01,	// Create, truncate
	OPENFLAG_OpenAlways		= 0x02,	// Create, don't truncate
	OPENFLAG_CreateNew		= 0x03,	// Must not exist
	OPENFLAG_DeleteBit		= 0x04,	// Internal use only
	OPENFLAG_Delete			= OPENFLAG_DeleteBit,
	OPENFLAG_Rename			= (OPENFLAG_DeleteBit + 1),

	OPENFLAG_File			= 0x08,
	OPENFLAG_Folder			= 0x10,
	OPENFLAG_Hidden			= 0x20,	// Allow finding hidden/system files
	OPENFLAG_ShortConflict	= 0x40,	// Internal use only
	OPENFLAG_FoundFree		= 0x80,	// Internal use only
};

// Values for Seek origin
enum FatSeek
{
	FAT_SEEK_SET = 0,
	FAT_SEEK_CUR = 1,
	FAT_SEEK_END = 2
};

enum FAT_ERROR
{
	FATERR_None = STERR_None,

	FATERR_Busy = STERR_Busy,
	// Hardware errors fit here from STORAGE_ERROR
	//
	// File system errors
	FATERR_CantMount = STERR_Last,		// -11
	NEG_ENUM(FATERR_InvalidDrive),		// -12
	NEG_ENUM(FATERR_NoHandles),			// -13
	NEG_ENUM(FATERR_InvalidHandle),		// -14
	NEG_ENUM(FATERR_FileNotFound),		// -15
	NEG_ENUM(FATERR_InvalidArgument),	// -16
	NEG_ENUM(FATERR_FileExists),		// -17
	NEG_ENUM(FATERR_RootDirFull),		// -18
	NEG_ENUM(FATERR_DiskFull),			// -19
	NEG_ENUM(FATERR_InvalidFileName),	// -20
	NEG_ENUM(FATERR_FormatError),		// -21
	NEG_ENUM(FATERR_InternalError),		// -22
	NEG_ENUM(FATERR_DirtyNotClosed),	// -23
};

