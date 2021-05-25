//****************************************************************************
// UpdateMgr.h
//
// Created 3/9/2021 11:56:29 AM by Tim
//
//****************************************************************************

#pragma once

#include <Nvm/Nvm.h>
#include "VersionUpdate.h"
#include "ProgressBar.h"


class UpdateMgr
{
	static constexpr ulong ProgressBarForecolor = 0x00FF00;
	static constexpr ulong ProgressBarBackcolor = 0xFFFFFF;
	static constexpr ulong ProgressInterval = 0x1000;	// chunk size before updating progress bar

	static constexpr int FlashRowSize = FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES;

	enum EditMode
	{
		EDIT_None,
		EDIT_File,
		EDIT_Inspect,
		EDIT_HeaderError,
		EDIT_FileError,
	};

	enum UpdateState
	{
		UPDT_None,
		UPDT_Graphics,
		UPDT_Fonts,
	};

	enum SectionMap
	{
		// bit map of sections found
		SECMAP_None = 0,
		SECMAP_Firmware = 1,
		SECMAP_Graphics = 2,
		SECMAP_Fonts = 4,
		SECMAP_All = SECMAP_Firmware | SECMAP_Graphics | SECMAP_Fonts
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	static void Open()
	{
		Lcd.EnablePip1(&UpdateDialog, 0, 0);
		DisplayVersions(&UpdateDialog_Areas.CurrrentFirmware, PROGRAM_VERSION, GRAPHICS_VERSION, FONT_VERSION);
		FileOp.SetErrorHandler(FileError);
		Files.Open(&s_editFile, ListUpdate, UpdateFileListHeight);
	}

	static void Close()
	{
		if (s_editMode == EDIT_File)
			EndEdit();

		FileOp.SetErrorHandler(NULL);
		Lcd.DisablePip1();
		Lcd.DisablePip2();
	}

	static byte *GetUpdateBuffer()	{ return s_updateBuffer; }

	static void UpdateAction(uint spot, int x, int)
	{
		switch (spot)
		{
		case FileName:
			x -= UpdateDialog_Areas.FileName.Xpos;	// relative edit box
			if (s_editMode != EDIT_File)
				StartEdit(x);
			else
				s_editFile.SetPositionPx(x); // Already editing file
			break;

		case ClearFile:
			s_editFile.DeleteText();
			StartEdit(0);
			break;

		case UsbDriveRadio:
			if (!(Files.GetDriveMap() & UsbDriveMap))
				break;
			Files.SetDrive(UsbDrive);
			break;

		case SdDriveRadio:
			if (!(Files.GetDriveMap() & SdDriveMap))
				break;
			Files.SetDrive(SdDrive);
			break;

		case UpdateCancel:
			if (s_editMode == EDIT_FileError)
				StartEdit(EditLine::EndLinePx);
			else if (s_editMode == EDIT_File)
				EndEdit();
			else
				Close();
			break;

		case VersionMatch:
			s_fWriteAll ^= true;
			ShowVersionMatch();
			break;			

		case UpdateExecute:
			if (s_editMode == EDIT_File)
				EndEdit();

			else if (s_editMode == EDIT_FileError)
			{
				ClearFileError();
				break;
			}

			if (CheckIfFolder())
				Files.Refresh();
			else
			{
				// Read and display version info
				FileOp.ReadUpdateHeader(FileBrowser::GetPathBuf(), Files.GetDrive());
			}
			break;
		}
	}

	static void FileKeyHit(void *pvUser, uint key)
	{
		EditLine::EditStatus	edit;

		edit = s_editFile.ProcessKey(key);
		if (edit == EditLine::EditDone)
		{
			EndEdit();
			if (CheckIfFolder())
				Files.Refresh();
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	static void HeaderAvailable(UpdateHeader *pUpdate, int cb, uint hFile)
	{
		UpdateSection	*pSection;
		UpdateSection	*pFirmwareSection;
		UpdateSection	*pGraphicsSection;
		UpdateSection	*pFontsSection;
		uint	versionMap;
		uint	vFirmware;
		uint	vGraphics;
		uint	vFonts;
		ulong	cbTotal;

		// Verify this is a valid update file
		if (cb == FAT_SECT_SIZE && pUpdate->signature == s_signature.signature && pUpdate->countOfSections >= 3)
		{
			pSection = (UpdateSection *)ADDOFFSET(pUpdate, pUpdate->sectionsStart);
			versionMap = SECMAP_None;
			for (uint i = 0; i < pUpdate->countOfSections; i++)
			{
				// Make sure it fit in buffer
				if (ADDOFFSET(pSection, pUpdate->sectionSize) >= ADDOFFSET(pUpdate, FAT_SECT_SIZE))
					break;

				switch (pSection->progId)
				{
				case DroFirmwareId:
					if (pSection->dataSize > FLASH_SIZE)
						goto InvalidHeader;
					pFirmwareSection = pSection;
					vFirmware = pSection->progVersion;
					versionMap |= SECMAP_Firmware;
					break;

				case DroGraphicsId:
					pGraphicsSection = pSection;
					vGraphics = pSection->progVersion;
					versionMap |= SECMAP_Graphics;
					break;

				case DroFontId:
					pFontsSection = pSection;
					vFonts = pSection->progVersion;
					versionMap |= SECMAP_Fonts;
					break;

				default:
					continue;
				}

				// Skip to next section, per size in header
				pSection = (UpdateSection *)ADDOFFSET(pSection, pUpdate->sectionSize);

				if (versionMap == SECMAP_All)
				{
					if (s_editMode == EDIT_Inspect)
					{
						// Perform update
						s_pFirmwareSection = pFirmwareSection;
						cbTotal = pFirmwareSection->dataSize;

						// See if we're leaving some out
						if (!s_fWriteAll && pGraphicsSection->progVersion <= GRAPHICS_VERSION)
							pGraphicsSection = NULL;
						else
							cbTotal += pGraphicsSection->dataSize;

						if (!s_fWriteAll && pFontsSection->progVersion <= FONT_VERSION)
							pFontsSection = NULL;
						else
							cbTotal += pFontsSection->dataSize;

						s_pGraphicsSection = pGraphicsSection;
						s_pFontsSection = pFontsSection;
						s_progress.SetMax(cbTotal);
						s_progressVal = 0;
						s_progress.SetValue(0);

						// Step 1: Read program into video RAM
						FileOp.ReadFirmware(RamUpdateStart, pFirmwareSection->dataSize, pFirmwareSection->dataStart, hFile);
					}
					else
					{
						FatSys::Close(hFile);
						SetEditMode(EDIT_Inspect);
						Lcd.CopyRect(&UpdateDialog, &UpdateDialog_Areas.UpdateLabel, &UpdateLabel);
						DisplayVersions(&UpdateDialog_Areas.UpdateFirmware, vFirmware, vGraphics, vFonts);
					}
					return;
				}
			}
		}

InvalidHeader:
		// Invalid header
		FatSys::Close(hFile);
		SetEditMode(EDIT_HeaderError);
		s_versionText.SetArea(UpdateDialog_Areas.ProgressBar);
		s_versionText.WriteString(s_InvalidUpdateMsg);
	};
#pragma GCC diagnostic pop

	static void ReadUpdateComplete(uint hFile)
	{
		s_updateState = UPDT_Graphics;
		if (s_pGraphicsSection != NULL)
			FileOp.WriteToFlash(FlashScreenStart, s_pGraphicsSection->dataSize, s_pGraphicsSection->dataStart, hFile);
		else
			FlashWriteComplete(hFile);	// pretend we finished the graphics
	}

	static void FlashWriteComplete(uint hFile)
	{
		switch (s_updateState)
		{
		case UPDT_Graphics:
			if (s_pFontsSection != NULL)
			{
				s_updateState = UPDT_Fonts;
				FileOp.WriteToFlash(FlashFontStart, s_pFontsSection->dataSize, s_pFontsSection->dataStart, hFile);
				break;
			}
			//
			// Fall into case of Fonts complete
			//
		case UPDT_Fonts:
			FatSys::Close(hFile);
			PrepFirmwareUpdate(RamUpdateStart);
			UpdateFirmware(s_pFirmwareSection->dataSize);
			break;

		default:
			// Single area write
			FatSys::Close(hFile);
			printf("complete.\n");
			break;
		}
	}

	static void UpdateProgress(uint cb)
	{
		if (s_updateState == UPDT_None)
			return;

		s_progressVal += cb;
		if (s_progressVal - s_progressLast > ProgressInterval)
		{
			s_progressLast = s_progressVal;
			s_progress.SetValue(s_progressVal);
		}
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static void StartEdit(int pos)
	{
		if (s_editMode == EDIT_FileError)
			ClearFileError();
		SetEditMode(EDIT_File);
		s_editFile.StartEditPx(pos);
		KeyboardMgr::OpenKb(FileKeyHit);
	}

	static void EndEdit()
	{
		s_editFile.EndEdit();
		s_editMode = EDIT_None;
		KeyboardMgr::CloseKb();
	}

	static bool CheckIfFolder()
	{
		char	chEnd;
		bool	isFolder;
		int		cch;

		cch = s_editFile.CharCount();
		if (cch == 0)
			isFolder = true;
		else
		{
			chEnd = Files.GetPathBuf()[cch - 1];
			isFolder = chEnd == '/' || chEnd == '\\';
		}
		return isFolder;
	}

	static void ListUpdate(FileBrowser::NotifyReason reason)
	{
		switch (reason)
		{
		case FileBrowser::SelectionChanged:
			// Callback when file/folder selected from list
			if (s_editMode == EDIT_FileError)
				ClearFileError();
			SetEditMode(EDIT_None);
			break;

		case FileBrowser::DriveChanged:
			if (s_editMode == EDIT_File)
				EndEdit();

			if (Files.GetDrive() != -1)
				ClearFileError();
			//
			// Fall into DriveStatusChanged
			//
		case FileBrowser::DriveStatusChanged:
			ShowDriveChoice();
			break;
		}
	}

	static void SetEditMode(EditMode mode)
	{
		if (s_editMode == mode)
			return;

		s_versionText.SetArea(UpdateDialog_Areas.ProgressBar);
		s_versionText.ClearArea();

		s_editMode = mode;

		Lcd.SelectImage(&UpdateDialog, &UpdateDialog_Areas.UpdateButton, 
			&InspectUpdate, s_editMode == EDIT_Inspect);
	}

	static void DisplayVersions(const Area *pAreas, uint firmwareVersion, uint graphicsVersion, uint fontsVersion) NO_INLINE_ATTR
	{
		s_versionText.SetArea(*pAreas++);
		s_versionText.printf("%u", firmwareVersion);
		s_versionText.SetArea(*pAreas++);
		s_versionText.printf("%u", graphicsVersion);
		s_versionText.SetArea(*pAreas);
		s_versionText.printf("%u", fontsVersion);
	}

	static void ShowDriveChoice()
	{
		int		map;
		int		drive;
		int		index;

		map = Files.GetDriveMap();
		drive = Files.GetDrive();
		// USB
		index = map & UsbDriveMap ? (drive == UsbDrive ? RADIO_True : RADIO_False) : RADIO_NotAvailable;
		Lcd.SelectImage(&UpdateDialog, &UpdateDialog_Areas.UsbDriveBox, &RadioButtons, index);
		// SD card
		index = map & SdDriveMap ? (drive == SdDrive ? RADIO_True : RADIO_False) : RADIO_NotAvailable;
		Lcd.SelectImage(&UpdateDialog, &UpdateDialog_Areas.SdDriveBox, &RadioButtons, index);
	}

	static void ShowVersionMatch()
	{
		Lcd.SelectImage(&UpdateDialog, &UpdateDialog_Areas.VersionMatch, &UpdateCheck, s_fWriteAll);
	}

	static void ClearFileError()
	{
		SetEditMode(EDIT_None);
		Files.FileError();
	}

	static int FileError(int err)
	{
		DEBUG_PRINT("File error %i\n", -err);

		// Inform file browser
		SetEditMode(EDIT_FileError);
		Files.FileError(err);

		return err;
	}

	static void PrepFirmwareUpdate(ulong addr)
	{
		WDT->CTRL.reg = 0;	// turn off WDT

		// Queue up RA8876 data port
		Lcd.WriteReg(AW_COLOR, AW_COLOR_AddrModeLinear | DATA_BUS_WIDTH);
		Lcd.WriteReg32(CURH0, addr);
		Lcd.ReadReg(MRWDP);	// dummy read
	}

	//*********************************************************************
	// Function in RAM used to program flash
	//*********************************************************************
protected:
	RAMFUNC_ATTR NO_INLINE_ATTR static void UpdateFirmware(int cb)
	{
		ushort	*pFlash;

		pFlash = NULL;	// start programming at address zero
		__disable_irq();

		while (cb > 0)
		{
			// if address is multiple of row size, we need to erase the row
			if (((int)pFlash & (FlashRowSize - 1)) == 0)
			{
				Nvm::EraseRowReady(pFlash);
				Nvm::WaitReadyInline();
			}

			// Copy the data 16 bits at a time
			for (int i = FLASH_PAGE_SIZE; i > 0 && cb > 0; i -= 2, cb -= 2)
				*pFlash++ = Lcd.ReadData16Inline();

			// Write the page
			Nvm::WritePageReady();
			Nvm::WaitReadyInline();
		}
		NVIC_SystemReset();
	}

#ifdef DEBUG
	RAMFUNC_ATTR NO_INLINE_ATTR static void WriteByte(byte ch)
	{
		while (!SERCOM0->USART.INTFLAG.bit.DRE);
		SERCOM0->USART.DATA.reg = ch;
	}

	RAMFUNC_ATTR NO_INLINE_ATTR static void WriteDigit(byte digit)
	{
		digit = (digit & 0xF) + '0';
		if (digit > '9')
			digit += 'A' - '9' - 1;
		WriteByte(digit);
	}

	RAMFUNC_ATTR NO_INLINE_ATTR static void Show(int block)
	{
		WriteDigit(block >> 12);
		WriteDigit(block >> 8);
		WriteDigit(block >> 4);
		WriteDigit(block);
		WriteByte('\r');
		WriteByte('\n');
	}
#else
inline void Show(int block) {}
#endif

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const union
	{
		char		archSignature[UpdateSignatureLength];
		uint64_t	signature;
	} s_signature = { UPDATE_SIGNATURE };

	inline static const char s_InvalidUpdateMsg[] = "Not a valid update file.";

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static UpdateSection	*s_pFirmwareSection;
	inline static UpdateSection	*s_pGraphicsSection;
	inline static UpdateSection	*s_pFontsSection;
	inline static ulong	s_progressVal;
	inline static ulong	s_progressLast;
	inline static byte	s_editMode;
	inline static byte	s_updateState;
	inline static bool	s_fWriteAll;

	inline static EditLine		s_editFile{UpdateDialog, UpdateDialog_Areas.FileName, FileBrowser::GetPathBuf(),
		FileBrowser::GetPathBufSize(), FONT_CalcSmall, UpdateForeground, UpdateBackground};

	inline static TextField		s_versionText{UpdateDialog, UpdateDialog_Areas.CurrrentFirmware,
		FONT_CalcSmall, UpdateForeground, UpdateBackground};

	inline static ProgressBar	s_progress{UpdateDialog, UpdateDialog_Areas.ProgressBar,
		ProgressBarForecolor, ProgressBarBackcolor};

	inline static byte	s_updateBuffer[FAT_SECT_SIZE] ALIGNED_ATTR(uint32_t);
};
