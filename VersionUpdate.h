//****************************************************************************
// VersionUpdate.h
//
// Created 3/7/2021 4:37:37 PM by Tim
//
//****************************************************************************

#pragma once


enum ProgUpdateId
{
	DroFirmwareId = 0xBCB46430,
	DroGraphicsId = 0x3ED74774,
	DroFontId = 0x8EB0F55D,
};

//****************************************************************************
// This structure is placed at the very end of the code. Update software can
// find it by seeking back from the end of binary file.

struct VersionInfo_t
{
	uint32_t	progId;
	uint32_t	firmwareVersion;
	uint32_t	graphicsVersion;
	uint32_t	fontVersion;
};

//****************************************************************************
// These structures form the header of a firmware update file.

#define SIGNATURE	'P','a','t','e','r','s','o','n'

static constexpr int UpdateSectionCount = 3;

struct UpdateSection
{
	uint32_t	progId;
	uint32_t	progVersion;
	uint32_t	dataStart;
	uint32_t	dataSize;
};

struct UpdateHeader
{
	union
	{
		char		archSignature[8];
		uint64_t	signature;
	};
	uint32_t		sectionsStart;
	uint32_t		sectionSize;
	uint32_t		countOfSections;
	UpdateSection	arSections[UpdateSectionCount];
};
