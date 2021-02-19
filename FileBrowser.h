//****************************************************************************
// FileBrowser.h
//
// Created 2/14/2021 12:38:50 PM by Tim
//
//****************************************************************************

#pragma once

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
	static char *GetPathBuf()		{ return s_bufPath; }
	static ushort GetPathBufSize()	{ return sizeof s_bufPath; }

public:
	void Open(EditLine *pEdit)
	{
		s_fileRow.SetBackgroundTransparent(true);

		// UNDONE: parse s_bufPath to get initial folder
		m_pEdit = pEdit;
		FileOp.FolderEnum(s_bufPath);
		ScreenMgr::EnablePip2(this, 0, ToolListTop);
	}

	void Close()
	{
		ScreenMgr::DisablePip2();
	}

	void FolderEnumDone(int cntFiles)
	{
		m_cntFiles = cntFiles;
		SetTotalLines(cntFiles);
		qsort(((ushort *)FILE_BUF_END) - cntFiles, cntFiles, sizeof(ushort), (__compar_fn_t)CompareLinePtr);
		Invalidate();
		SetScrollPosition(0);
	}

	ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (StartCapture(x, y - ToolListTop, spot))
			return this;
		return NULL;
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
	// Helpers
	//*********************************************************************
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
		FileEnumInfo	*pInfo;

		if (lineNum < m_cntFiles)
		{
			pInfo = PtrInfoFromLine(lineNum);

			// Add file/folder icon
			ScreenMgr::SelectImage(&FileRow, &FileRow_Areas.FileIcon, &FileIcons, pInfo->Type);

			// Clear all text
			s_fileRow.SetArea(FileRow_Areas.AllText);
			s_fileRow.ClearArea();

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

			// Copy into place
			ScreenMgr::CopyRect(this, pArea, &FileRow);
		}
		else
		{
			ScreenMgr::FillRect(this, pArea, ToolLibBackground);
		}
	}

	virtual void LineSelected(int lineNum)
	{
		uint	cch;
		char	ch;
		int		i;
		FileEnumInfo	*pInfo;
		
		pInfo = PtrInfoFromLine(lineNum);
		cch = strlen(pInfo->Name);
		// Make sure it fits, including possible trailing '/' and null terminator
		if (cch + m_cchPath >= sizeof s_bufPath - 2)
			return;

		if (pInfo->Type == INFO_Parent)
		{
			// Peel back one level
			i = m_cchPath - 1;
			if (i >= 0)
			{
				while (i > 0)
				{
					ch = s_bufPath[i - 1];
					if (ch == '/' || ch == '\\')
						break;
					i--;
				}
				m_cchPath = i;
			}
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

		// Display in edit box
		m_pEdit->ResetPosition();
		m_pEdit->WriteString(s_bufPath);
		m_pEdit->ClearToEnd();
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	ushort		m_cntFiles;
	EditLine	*m_pEdit;
	ushort		m_cchPath;

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
	const char s_fmtSize[4][3] = {"B ", "KB", "MB", "GB"};
	const char s_amPm[2][3] = {"am", "pm"};
};

extern FileBrowser Files;
