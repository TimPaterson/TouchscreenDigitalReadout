//****************************************************************************
// UpdateMgr.h
//
// Created 3/9/2021 11:56:29 AM by Tim
//
//****************************************************************************

#pragma once

#include "VersionUpdate.h"


class UpdateMgr
{
	enum EditMode
	{
		EDIT_None,
		EDIT_File,
		EDIT_Inspect,
		// States after this indicate an error
		EDIT_StartErrors,
		EDIT_FileError = EDIT_StartErrors,
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	static void Open()
	{
		ScreenMgr::EnablePip1(&UpdateDialog, 0, 0);
		DisplayVersions(&UpdateDialog_Areas.CurrrentFirmware, PROGRAM_VERSION, GRAPHICS_VERSION, FONT_VERSION);
		FileOp.SetErrorHandler(FileError);
		Files.Open(&s_editFile, ListUpdate, UpdateFileListHeight);
	}

	static void Close()
	{
		if (s_editMode == EDIT_File)
			EndEdit();

		FileOp.SetErrorHandler(NULL);
		ScreenMgr::DisablePip1();
		ScreenMgr::DisablePip2();
	}

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
			if (s_editMode >= EDIT_StartErrors)
				StartEdit(EditLine::EndLinePx);
			else if (s_editMode == EDIT_File)
				EndEdit();
			else
				Close();
			break;

		case VersionMatch:
			s_fVersionMatch ^= true;
			ShowVersionMatch();
			break;			

		case UpdateExecute:
			if (s_editMode == EDIT_File)
				EndEdit();

			else if (s_editMode >= EDIT_StartErrors)
			{
				ClearFileError();
				break;
			}

			if (CheckIfFolder())
				Files.Refresh();
			else if (s_editMode == EDIT_Inspect)
			{
				// We've read in the version info, perform the update
				DEBUG_PRINT("Updating firmware\n");
				// UNDONE: firmware update
			}
			else
			{
				// Read and display version info
				DEBUG_PRINT("Reading version info\n");
				// UNDONE: read version info

				// If it worked
				SetEditMode(EDIT_Inspect);
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

	static int FirmwareUpdate(const char *pFilename, bool fWriteAll)
	{
		s_fWriteAll = fWriteAll;
		return 0;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static void StartEdit(int pos)
	{
		if (s_editMode >= EDIT_StartErrors)
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
			if (s_editMode >= EDIT_StartErrors)
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

		s_editMode = mode;

		ScreenMgr::SelectImage(&UpdateDialog, &UpdateDialog_Areas.UpdateButton, 
			&InspectUpdate, s_editMode == EDIT_Inspect);
		
	}

	static void DisplayVersions(const Area *pAreas, uint firmwareVersion, uint graphicsVersion, uint fontsVersion)
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
		ScreenMgr::SelectImage(&UpdateDialog, &UpdateDialog_Areas.UsbDriveBox, &RadioButtons, index);
		// SD card
		index = map & SdDriveMap ? (drive == SdDrive ? RADIO_True : RADIO_False) : RADIO_NotAvailable;
		ScreenMgr::SelectImage(&UpdateDialog, &UpdateDialog_Areas.SdDriveBox, &RadioButtons, index);
	}

	static void ShowVersionMatch()
	{
		ScreenMgr::SelectImage(&UpdateDialog, &UpdateDialog_Areas.VersionMatch, &UpdateCheck, s_fVersionMatch);
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

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static EditLine		s_editFile{UpdateDialog, UpdateDialog_Areas.FileName, FileBrowser::GetPathBuf(),
		FileBrowser::GetPathBufSize(), FID_CalcSmall, UpdateForeground, UpdateBackground};

	inline static NumberLine	s_versionText{UpdateDialog, UpdateDialog_Areas.CurrrentFirmware,
		FID_CalcSmall, UpdateForeground, UpdateBackground};

	inline static byte	s_editMode;
	inline static bool	s_fVersionMatch;
	inline static bool	s_fWriteAll;
};
