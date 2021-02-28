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


class FileBrowser : public ListScroll
{
public:
	FileBrowser() : ListScroll(FileListWidth, FileListHeight, FileRowHeight, Color16bpp, HOTSPOT_GROUP_FileDisplay) {}

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	typedef void UpdateNotify();

public:
	static char *GetPathBuf()		{ return s_bufPath; }
	static ushort GetPathBufSize()	{ return sizeof s_bufPath; }

public:
	void Open(EditLine *pEdit, UpdateNotify *pfnNotify = NULL)
	{
		m_pEdit = pEdit;
		m_pfnNotify = pfnNotify;
		ScreenMgr::EnablePip2(this, 0, ToolListTop);
		Refresh();
	}

	void Refresh(bool fCreate = false)
	{
		// Should be called if edit box has new folder
		FindLastFolder(strlen(s_bufPath));		// sets m_cchPath
		m_pEdit->UpdateBuffer();
		FileOp.FolderEnum(s_bufPath, m_cchPath, fCreate);
	}

	void Close()
	{
		ScreenMgr::DisablePip2();
	}

	void FolderEnumDone(int cntFiles)
	{
		SetTotalLines(cntFiles);
		qsort(((ushort *)FILE_BUF_END) - cntFiles, cntFiles, sizeof(ushort), (__compar_fn_t)CompareLinePtr);
		InvalidateAllLines();
	}

	ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (StartCapture(x, y - ToolListTop, spot))
			return this;
		return NULL;
	}

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

protected:
	static FileEnumInfo *PtrInfoFromLine(int lineNum)
	{
		return (FileEnumInfo *)ADDOFFSET(g_FileBuf, *(((ushort *)FILE_BUF_END) - 1 - lineNum));
	}

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
			ScreenMgr::SelectImage(&FileRow, &FileRow_Areas.FileIcon, &FileIcons, m_pErrMsg ? INFO_Error : pInfo->Type);

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
			ScreenMgr::CopyRect(this, pArea, &FileRow);
		}
		else
		{
			ScreenMgr::FillRect(this, pArea, ToolLibBackground); 
			if (lineNum == m_lineCnt && m_lineCnt != 0)
			{
				// Draw bottom line of last grid entry.
				area = *pArea;
				area.Height = 1;
				ScreenMgr::CopyRect(this, &area, &FileRow);
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
					FileOp.FolderEnum(s_bufPath, m_cchPath);
				}
			}

			m_pEdit->UpdateBuffer();
		}

		if (m_pfnNotify != NULL)
			m_pfnNotify();
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
