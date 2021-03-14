//****************************************************************************
// FileBrowser.h
//
// Created 2/14/2021 12:38:50 PM by Tim
//
//****************************************************************************

#pragma once

#include "LcdDef.h"
#include "ListScroll.h"
#include "FileOperations.h"

static void FatDriveStatusChange(int drive, int status);


enum DriveList
{
	UsbDrive,
	SdDrive,

	UsbDriveMap = 1 << UsbDrive,
	SdDriveMap = 1 << SdDrive,
};

enum RadioButtonImages
{
	RADIO_False,
	RADIO_True,
	RADIO_NotAvailable,
};


class FileBrowser : public ListScroll
{
public:
	FileBrowser() : ListScroll(FileListWidth, FileListHeight, FileRowHeight, Color16bpp, HOTSPOT_GROUP_FileDisplay) {}

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	enum NotifyReason
	{
		SelectionChanged,
		DriveStatusChanged,
		DriveChanged,
	};

	typedef void UpdateNotify(NotifyReason reason);

public:
	int GetDriveMap()	{ return m_driveMap; }
	int GetDrive()		{ return m_curDrive; }

public:
	static char *GetPathBuf()		{ return s_bufPath; }
	static ushort GetPathBufSize()	{ return sizeof s_bufPath; }

public:
	void Open(EditLine *pEdit, UpdateNotify *pfnNotify, uint height = FileListHeight)
	{
		int		drive;
		uint	map;

		m_pEdit = pEdit;
		m_pfnNotify = pfnNotify;
		SetViewHeight(height);
		Lcd.EnablePip2(this, 0, ScreenHeight - height);
		// See what drives are available
		for (drive = 0, map = 0; drive < FAT_NUM_DRIVES; drive++)
		{
			if (FatSys::IsDriveMounted(drive))
				map |= 1 << drive;
		}
		m_driveMap = map;
		FatSys::SetStatusNotify(FatDriveStatusChange);
		// If drive is valid, notify that drive map was updated
		drive = m_curDrive;
		if (SetDrive(drive) == drive && pfnNotify != NULL)
		{
			pfnNotify(DriveStatusChanged);
			Refresh();
		}
	}

	void Refresh(bool fCreate = false)
	{
		// Should be called if edit box has new folder
		FindLastFolder(strlen(s_bufPath));		// sets m_cchPath
		m_pEdit->UpdateBuffer();
		if (m_curDrive != -1)
			FileOp.FolderEnum(s_bufPath, m_curDrive, m_cchPath, fCreate);
	}

	void Close()
	{
		Lcd.DisablePip2();
	}

	int SetDrive(int drive)
	{
		int		newDrive;

		if (!((1 << drive) & m_driveMap))
		{
			// Drive isn't available
			if (m_driveMap == 0)
			{
				FileError(s_noDrivesMsg);
				newDrive = -1;
			}
			else
			{
				// Pick an available drive
				for (newDrive = 0; newDrive < FAT_NUM_DRIVES && ((1 << newDrive) & m_driveMap) == 0; newDrive++);
			}
		}
		else
			newDrive = drive;

		if (newDrive != m_curDrive)
		{
			// Changing drives, clean the slate
			m_curDrive = newDrive;
			s_bufPath[0] = '\0';
			if (m_pfnNotify != NULL)
				m_pfnNotify(DriveChanged);
			Refresh();
		}
		return newDrive;
	}

	//*********************************************************************
	// Notifications

	// Notification from Actions class
	ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (StartCapture(x, y - Lcd.GetPip2()->y, spot))
			return this;
		return NULL;
	}

	// Notification from FileOperations class
	void FolderEnumDone(int cntFiles)
	{
		SetTotalLines(cntFiles);
		qsort(((ushort *)FILE_BUF_END) - cntFiles, cntFiles, sizeof(ushort), (__compar_fn_t)CompareLinePtr);
		InvalidateAllLines();
	}

	// Notification from FileOperations class
	void DriveMountComplete(int drive)
	{
		m_driveMap |= 1 << drive;
		if (m_curDrive == -1)
			SetDrive(drive);

		if (m_pfnNotify != NULL)
			m_pfnNotify(DriveStatusChanged);
	}

	// Notification from ToolLib class, from FileOperations class
	void FileError(int err)
	{
		FileError(s_arErrMsg[-(err + 1)]);
	}

	void FileError(const char *psz = NULL)
	{
		m_pErrMsg = psz;
		SetTotalLines(psz == NULL ? 0 : 1);
		InvalidateAllLines();
	}

	// Notification from FatSys class
	void DriveStatusChange(int drive, int status)
	{
		// Ignore Mount notification because the driver in 
		// FileOperations hasn't finished yet.
		if (status == FatDrive::FDS_Dismounted)
		{
			m_driveMap &= ~(1 << drive);
			SetDrive(m_curDrive);	// See if still valid
		}

		if (m_pfnNotify != NULL)
			m_pfnNotify(DriveStatusChanged);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	void FindLastFolder(int pos)
	{
		char	ch;

		while (pos > 0)
		{
			ch = s_bufPath[pos - 1];
			if (ch == '/' || ch == '\\')
				break;
			pos--;
		}
		m_cchPath = pos;
	}

	//*********************************************************************
	// Implement functions in ListScroll

protected:
	virtual void FillLine(int lineNum, Area *pArea)
	{
		ulong		size;
		int			index;
		uint		hours;
		const char	*pAmPm;
		Area		area;
		FileEnumInfo	*pInfo;

		if (lineNum < m_lineCnt)
		{
			// Clear all text
			s_fileRow.SetArea(FileRow_Areas.AllText);
			s_fileRow.ClearArea();

			// Add file/folder icon
			pInfo = PtrInfoFromLine(lineNum);
			Lcd.SelectImage(&FileRow, &FileRow_Areas.FileIcon, &FileIcons, m_pErrMsg ? INFO_Error : pInfo->Type);

			if (m_pErrMsg)
			{
				// Just use the AllText area set above
				s_fileRow.SetTextColor(ToolLibSelected);
				s_fileRow.WriteString(m_pErrMsg);
				s_fileRow.SetTextColor(ToolLibForeground);
			}
			else
			{
				if (pInfo->Type != INFO_Parent)
				{
					// Write filename
					s_fileRow.SetArea(FileRow_Areas.FileName);
					s_fileRow.WriteString(pInfo->Name);
				}

				if (pInfo->Type == INFO_File)
				{
					// Date
					s_fileRow.SetSpaceWidth(s_fileRow.GetCharWidth('0'));
					s_fileRow.SetArea(FileRow_Areas.FileDate);
					s_fileRow.printf("%2u/%02u/%u", pInfo->DateTime.date.month, 
						pInfo->DateTime.date.day, pInfo->DateTime.date.year + FAT_YEAR_BASE);

					// Time
					s_fileRow.SetArea(FileRow_Areas.FileTime);
					hours = pInfo->DateTime.time.hours;
					if (hours > 12)
					{
						pAmPm = s_amPm[1];
						hours -= 12;
					}
					else
					{
						pAmPm = s_amPm[0];
						if (hours == 0)
							hours = 12;
					}
					s_fileRow.printf("%2u:%02u", hours, pInfo->DateTime.time.minutes);
					s_fileRow.SetSpaceWidth(0);
					s_fileRow.printf(" %s", pAmPm);
					s_fileRow.SetSpaceWidth(s_fileRow.GetCharWidth('0'));

					// Size
					s_fileRow.SetArea(FileRow_Areas.FileSize);
					size = pInfo->Size;
					index = 0;
					if (size >= 1000000)
					{
						size /= 1000000;
						index = 2;
					}
					if (size >= 1000)
					{
						size /= 1000;
						index ++;
					}
					s_fileRow.printf("%3lu", size);
					s_fileRow.SetSpaceWidth(0);
					s_fileRow.printf(" %s", s_fmtSize[index]);
				}
			}

			// Copy into place
			Lcd.CopyRect(this, pArea, &FileRow);
		}
		else
		{
			Lcd.FillRect(this, pArea, ToolLibBackground); 
			if (lineNum == m_lineCnt && m_lineCnt != 0)
			{
				// Draw bottom line of last grid entry.
				area = *pArea;
				area.Height = 1;
				Lcd.CopyRect(this, &area, &FileRow);
			}
		}
	}

	virtual void LineSelected(int lineNum)
	{
		uint	cch;
		FileEnumInfo	*pInfo;

		if (m_pErrMsg == NULL)
		{
			pInfo = PtrInfoFromLine(lineNum);
			cch = strlen(pInfo->Name);
			// Make sure it fits, including possible trailing '/' and null terminator
			if (cch + m_cchPath >= sizeof s_bufPath - 2)
				return;

			if (pInfo->Type == INFO_Parent)
			{
				// Peel back one level
				if (m_cchPath != 0)
					FindLastFolder(m_cchPath - 1);
				goto EndFolder;
			}
			else
			{
				// Append to path
				memcpy(&s_bufPath[m_cchPath], pInfo->Name, cch + 1);

				if (pInfo->Type == INFO_Folder)
				{
					m_cchPath += cch;
					s_bufPath[m_cchPath++] = '/';
EndFolder:
					s_bufPath[m_cchPath] = '\0';
					FileOp.FolderEnum(s_bufPath, m_curDrive, m_cchPath);
				}
			}

			m_pEdit->UpdateBuffer();
		}

		if (m_pfnNotify != NULL)
			m_pfnNotify(SelectionChanged);
	}

	//*********************************************************************
	// static functions

protected:
	static FileEnumInfo *PtrInfoFromLine(int lineNum)
	{
		return (FileEnumInfo *)ADDOFFSET(g_FileBuf, *(((ushort *)FILE_BUF_END) - 1 - lineNum));
	}

	static int CompareLinePtr(const ushort *pOff1, const ushort *pOff2)
	{
		return CompareLines(*pOff2, *pOff1);
	}

	static int CompareLines(uint off1, uint off2)
	{
		FileEnumInfo	*pInfo1, *pInfo2;

		pInfo1 = (FileEnumInfo *)ADDOFFSET(g_FileBuf, off1);
		pInfo2 = (FileEnumInfo *)ADDOFFSET(g_FileBuf, off2);

		if (pInfo1->Type == pInfo2->Type)
			return strcasecmp(pInfo1->Name, pInfo2->Name);

		if (pInfo1->Type == INFO_Parent)
			return -1;

		if (pInfo2->Type == INFO_Parent)
			return 1;

		if (pInfo1->Type == INFO_Folder)
			return -1;

		return 1;
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	EditLine		*m_pEdit;
	const char		*m_pErrMsg;
	UpdateNotify	*m_pfnNotify;
	ushort			m_cchPath;
	sbyte			m_curDrive;
	byte			m_driveMap;

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static TextField	s_fileRow{FileRow, FileRow_Areas.FileName, 
		FID_CalcSmall, ToolLibForeground, ToolLibBackground};

	inline static char		s_bufPath[MAX_PATH + 1];

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const char s_fmtSize[4][3] = {"B ", "KB", "MB", "GB"};
	inline static const char s_amPm[2][3] = {"am", "pm"};

	//*********************************************************************
	// File error messages

	inline static const char s_noDrivesMsg[] = "No drives available.";

	inline static const char * const s_arErrMsg[] = {
		// From Storage.h
		"Drive busy",
		"Drive not present",
		"Drive not mounted",
		"Bad block on drive",
		"Drive is write protected",
		"",	// Bad command
		"Drive failed",
		"", // Invalid block
		"Drive timed out",
		"Drive not available",

		// From FatFileConst.h
		"Drive can't be mounted",
		"Invalid drive identifier",
		"Too many files open",
		"", // Invalid handle
		"File/folder not found",
		"", // Invalid argument
		"", // File exists
		"Root folder is full",
		"Drive is full",
		"Bad character in file name",
		"Drive not formatted correctly",
		"Internal error",
		"", // Dirty file not closed
	};
};

extern FileBrowser Files;

inline void FatDriveStatusChange(int drive, int status)
{
	Files.DriveStatusChange(drive, status);
}

