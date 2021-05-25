// DroUpdateBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Dro/VersionUpdate.h"


static const int FileNameSize = 500;
static const int FileAlignment = 512;
static const int FileBufSize = 0x10000;

static char s_FileCopyBuffer[FileBufSize];
static char s_inFileNameBuf[FileNameSize];
static char s_outFileNameBuf[FileNameSize];
static char s_graphicsFileName[] = "Images\\Screen.bin";
static char s_fontFileName[] = "Fonts\\Fonts.bin";
static char s_codeExtension[] = ".bin";
static char s_outExtension[] = ".upd";

VersionInfo_t	s_versionInfo;

enum SectionIndex
{
	IDX_Firmware,
	IDX_Graphics,
	IDX_Fonts,
};

UpdateHeader	s_fileHeader =
{
	{ UPDATE_SIGNATURE },
	offsetof(UpdateHeader, arSections),
	sizeof(UpdateSection),
	UpdateSectionCount,
	// Array of sections
	{
		// firmware section
		{
			DroFirmwareId,
			0,	// version
			FileAlignment,
		},
		// graphics section
		{
			DroGraphicsId,
		},
		// font section
		{
			DroFontId,
		},
	}
};

bool CopyFile(FILE* dst, FILE* src)
{
	int	size;
	while (size = fread(s_FileCopyBuffer, 1, FileBufSize, src))
	{
		if (fwrite(s_FileCopyBuffer, 1, size, dst) != size)
			return true;
	}
	fclose(src);
	return false;
}

int main(int argc, char *argv[])
{
	FILE* pFileCode;
	FILE* pFileGraphics;
	FILE* pFileFont;
	FILE* pFileUpdate;
	char* pstr;
	int	cch;
	int size;
	int position;

	if (argc != 2)
	{
CodeError:
		printf("Must run with a single filename argument with the extension '.bin'.\n");
		return -1;
	}

	// Verify extension
	cch = strlen(argv[1]);
	if (cch < sizeof s_codeExtension - 2 || _stricmp(s_codeExtension, &argv[1][cch - (sizeof s_codeExtension - 1)]) != 0)
		goto CodeError;

	if (fopen_s(&pFileCode, argv[1], "rb"))
	{
		printf("Update code file not found.\n");
		return -1;
	}

	// Verify VersionInfo_t at end
	fseek(pFileCode, -(int)sizeof s_versionInfo, SEEK_END);
	if (fread(&s_versionInfo, sizeof s_versionInfo, 1, pFileCode) != 1 || s_versionInfo.progId != DroFirmwareId)
	{
		printf("File is not a valid DRO update image.\n");
		return -1;
	}
	// We are now at end of file, so it's easy to get its length
	size = ftell(pFileCode);
	s_fileHeader.arSections[IDX_Firmware].dataSize = size;
	fseek(pFileCode, 0, SEEK_SET);

	// Fill in header with file versions
	s_fileHeader.arSections[IDX_Firmware].progVersion = s_versionInfo.firmwareVersion;
	s_fileHeader.arSections[IDX_Graphics].progVersion = s_versionInfo.graphicsVersion;
	s_fileHeader.arSections[IDX_Fonts].progVersion = s_versionInfo.fontVersion;

	// Create name for graphics file
	_fullpath(s_inFileNameBuf, argv[1], FileNameSize);
	strcpy_s(s_outFileNameBuf, FileNameSize, s_inFileNameBuf);
	pstr = strrchr(s_inFileNameBuf, '\\');		// peel off filename
	if (pstr == NULL)
		goto GraphicsError;
	*pstr = '\0';
	pstr = strrchr(s_inFileNameBuf, '\\');		// peel off build folder
	if (pstr == NULL)
		goto GraphicsError;
	pstr++;	// include '\'
	strcpy_s(pstr, FileNameSize - (pstr - s_inFileNameBuf), s_graphicsFileName);
	// Open graphics file
	if (fopen_s(&pFileGraphics, s_inFileNameBuf, "rb"))
	{
GraphicsError:
		printf("Update graphcs file not found.\n");
		return -1;
	}

	// Create name for font file
	strcpy_s(pstr, FileNameSize - (pstr - s_inFileNameBuf), s_fontFileName);
	// Open font file
	if (fopen_s(&pFileFont, s_inFileNameBuf, "rb"))
	{
		printf("Update font file not found.\n");
		return -1;
	}

	// Create name for output file.
	memcpy(&s_outFileNameBuf[strlen(s_outFileNameBuf) - (sizeof s_codeExtension - 1)], s_outExtension, sizeof s_outExtension);
	if (fopen_s(&pFileUpdate, s_outFileNameBuf, "wb"))
	{
		printf("Unable to create output file.\n");
		return -1;
	}

	// All files successfully opened. Finish building header.
	// Round up position of each section's data
	position = s_fileHeader.arSections[IDX_Firmware].dataStart;
	position += (size + FileAlignment - 1) & ~(FileAlignment - 1);
	s_fileHeader.arSections[IDX_Graphics].dataStart = position;
	fseek(pFileGraphics, 0, SEEK_END);
	size = ftell(pFileGraphics);
	fseek(pFileGraphics, 0, SEEK_SET);
	s_fileHeader.arSections[IDX_Graphics].dataSize = size;

	position += (size + FileAlignment - 1) & ~(FileAlignment - 1);
	s_fileHeader.arSections[IDX_Fonts].dataStart = position;
	fseek(pFileFont, 0, SEEK_END);
	size = ftell(pFileFont);
	fseek(pFileFont, 0, SEEK_SET);
	s_fileHeader.arSections[IDX_Fonts].dataSize = size;

	// Header complete, write everything out
	if (fwrite(&s_fileHeader, sizeof s_fileHeader, 1, pFileUpdate) != 1)
	{
WriteError:
		printf("Error writing output file.\n");
	}
	fseek(pFileUpdate, s_fileHeader.arSections[IDX_Firmware].dataStart, SEEK_SET);
	if (CopyFile(pFileUpdate, pFileCode))
		goto WriteError;

	fseek(pFileUpdate, s_fileHeader.arSections[IDX_Graphics].dataStart, SEEK_SET);
	if (CopyFile(pFileUpdate, pFileGraphics))
		goto WriteError;

	fseek(pFileUpdate, s_fileHeader.arSections[IDX_Fonts].dataStart, SEEK_SET);
	if (CopyFile(pFileUpdate, pFileFont))
		goto WriteError;

	fclose(pFileUpdate);
	printf("Update file successfully written. Versions: firmware %u, graphics %u, font %u.\n",
		s_fileHeader.arSections[IDX_Firmware].progVersion,
		s_fileHeader.arSections[IDX_Graphics].progVersion, 
		s_fileHeader.arSections[IDX_Fonts].progVersion);
}
