//****************************************************************************
// ToolLib.h
//
// Created 1/30/2021 2:45:39 PM by Tim
//
//****************************************************************************

#pragma once

#include "HotspotList.h"
#include "LcdDef.h"
#include "ListScroll.h"
#include "AxisDisplay.h"
#include "EditLine.h"
#include "KeyboardMgr.h"
#include "FileBrowser.h"
#include <FatFile/FatFileConst.h>

#define IMPORT_HEAD_TEXT "Tool,Diameter,Length,Flutes,Description"
static constexpr int IMPORT_HEAD_TEXT_LENGTH = STRLEN(IMPORT_HEAD_TEXT);


class ToolLib
{
	static constexpr int FlashRowSize = FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES;
	static constexpr int ToolEntrySize = FLASH_PAGE_SIZE;
	static constexpr int MaxToolCount = 500;
	static constexpr int ToolsPerRow = FlashRowSize / ToolEntrySize;
	static constexpr ulong FlashErasedValue = 0xFFFFFFFF;
	static constexpr ushort NoCurrentLine = 0xFFFF;		// occurs in s_curLineNum
	static constexpr ushort ToolBufIndex = 0xFFFF;		// occurs in s_arSortList[]
	static constexpr ushort ToolNotModified = 0xFFFF;	// occurs in m_modToolOffset
	static constexpr ushort InvalidTool = 0;			// occurs in tool number
	static constexpr ushort EmptyTool = 0xFFFF;			// occurs in tool number
	static constexpr ushort	MaxToolNumber = 999;

	#define ToolListStartAddr	0x18000
	#define ToolListStart		((ToolLibInfo *)ToolListStartAddr)
	#define ToolListEnd			((ToolLibInfo *)FLASH_SIZE)
	#define	ToolFromOffset(n)	((ToolLibInfo *)(ToolListStartAddr + n))
	#define OffsetFromTool(p)	((ulong)p - ToolListStartAddr)
	#define EndToolOffset		(FLASH_SIZE - ToolListStartAddr)
	#define ToolRowBuf			((ToolLibInfo *)g_FileBuf)

	//*********************************************************************
	// Types
	//*********************************************************************

	enum ImportError
	{
		IMPERR_None,
		IMPERR_BadFormat = -1,
		IMPERR_BadTool = -2
	};

	enum ToolButtonImages
	{
		TOOL_IMAGE_NotModified,
		TOOL_IMAGE_ConfirmDelete,
		TOOL_IMAGE_IsModified,
	};

	enum ImportExportImages
	{
		// The first two are also referenced with the boolean s_isExport
		FILE_IMAGE_Import,
		FILE_IMAGE_Export,
		FILE_IMAGE_AddFolder,
		FILE_IMAGE_OpenFolder,
	};

	enum EditMode
	{
		EDIT_None,
		EDIT_ConfirmDelete,
		EDIT_Description,
		EDIT_File,
		EDIT_Time,
		// States after this indicate an error
		EDIT_StartErrors,
		EDIT_FolderCreatePrompt = EDIT_StartErrors,
		EDIT_FileError,
	};

	struct ToolLibBase
	{
		ushort	number;
		ushort	flutes;
		double	diameter;
		double	length;
	};

	static constexpr int ToolDescSize = ToolEntrySize - sizeof(ToolLibBase);

	struct ToolLibInfo : public ToolLibBase
	{
		char	arDesc[ToolDescSize];

		bool HasData()
		{
			return diameter != 0 || length != 0 || flutes != 0 || arDesc[0] != 0;
		}

		bool IsValid()
		{
			if (number == 0)
				return false;

			return HasData();
		}

		ToolLibInfo & operator= (const ToolLibInfo &src)
		{
			// This is used when writing a tool to flash, which must
			// use 16-bit or 32-bit writes
			*(ToolLibBase *)this = *(ToolLibBase *)&src;
			Nvm::memcpy32(arDesc, src.arDesc, strlen(src.arDesc) + 1);
			return *this;
		}

		void ClearData()
		{
			number = 0;
			flutes = 0;
			diameter = 0;
			length = 0;
			arDesc[0] = 0;
		}
	};

	class ToolScroll : public ListScroll
	{
	public:
		ToolScroll() : ListScroll(ToolListWidth, ToolListHeight, ToolRowHeight,
			Color16bpp, HOTSPOT_GROUP_ToolDisplay) {}

		//*****************************************************************
		// Implement functions in ListScroll

	protected:
		virtual void FillLine(int lineNum, Area *pArea);
		virtual void LineSelected(int lineNum);
	};

	class ToolDisplay : public NumberLineBlankZ
	{
	public:
		ToolDisplay(Canvas &canvas, const Area &area, ulong foreColor, ulong backColor) :
			NumberLineBlankZ(canvas, area, FONT_CalcSmall, foreColor, backColor), arItemAreas{&area}
			{}

	public:
		void DisplayLine(ToolLibInfo *pTool)
		{
			uint	widthSpace;

			PrintDbl(IsMetric() ? "%5.2f" : "%5.3f", CheckMetric(pTool->diameter), arItemAreas[ToolDiameter]);
			PrintDbl(IsMetric() ? "%6.2f" : "%6.3f", CheckMetric(pTool->length), arItemAreas[ToolLength]);

			PrintUint("%3u", pTool->number, arItemAreas[ToolNumber]);
			PrintUint("%3u", pTool->flutes, arItemAreas[ToolFlutes]);

			// Set the space width to normal for the description field
			SetArea(arItemAreas[ToolDesc]);
			ClearArea();
			widthSpace = GetSpaceWidth();
			SetSpaceWidth();
			TextField::WriteString(pTool->arDesc);
			SetSpaceWidth(widthSpace);
		}

	protected:
		const Area *arItemAreas;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	// .cpp file
	void ToolAction(uint spot, int x, int y);
	void ShowExportTime(RtcTime time);
	void ShowToolInfo();
	int ImportStart(int cb);
	void *ImportErase(void *pv);
	int ImportTools(char *pb, uint cb, uint cbWrap);
	void ImportDone(int err);
	char *ExportTool(char *pBuf, uint iTool);

public:
	static char *ExportStart();
	static void SetTime(uint spot);
	static void ExportDone();
	static void ToolEntryKeyHitCallback(void *pvUser, uint key);
	static void FileKeyHitCallback(void *pvUser, uint key);
	static void ListUpdateCallback(FileBrowser::NotifyReason reason);
	static int FileErrorCallback(int err);

protected:
	int ImportTool(char *pchBuf);

public:
	void Init()
	{
		uint	line, lineCur;

		m_scroll.Init();

		m_pFreeTool = ToolListEnd;
		m_modToolOffset = ToolNotModified;
		line = NoCurrentLine;

		// Scan for existing tools
		for (ToolLibInfo *pTool = ToolListStart; pTool < ToolListEnd; pTool++)
		{
			uint	tool;

			tool = pTool->number;
			if (tool == EmptyTool)
			{
				m_pFreeTool = pTool;
				break;
			}

			if (tool == InvalidTool || tool > MaxToolNumber)
				continue;

			lineCur = InsertTool(OffsetFromTool(pTool));
			if (tool == Eeprom.Data.Tool)
				line = lineCur;
		}

		m_curLineNum = line;
		m_scroll.SetTotalLines(m_toolCount);
		m_scroll.ScrollToLine(line == NoCurrentLine ? 0 : line);
		ShowToolInfo();
	}

	void ShowToolLib()
	{
		Lcd.EnablePip1(&ToolLibrary, 0, 0);
		Lcd.EnablePip2(&m_scroll, 0, ToolListTop);
	}

	ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (m_scroll.StartCapture(x, y - ToolListTop, spot))
			return &m_scroll;
		return NULL;
	}

	void SetToolSide(uint side)
	{
		m_toolSides = (m_toolSides & ~(side >> ToolMaskShift)) ^ side;
		ShowToolInfo();
	}

	void ChangeUnits()
	{
		m_scroll.InvalidateLines(0, m_toolCount - 1);
		ShowToolInfo();
	}

public:
	static void ShowFeedRate(double rate)
	{
		s_feedRate.PrintDbl("\n%5.0f", rate);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	void ToolEntryKeyHit(void *pvUser, uint key)
	{
		EditLine::EditStatus	edit;

		edit = s_editDesc.ProcessKey(key);
		if (edit == EditLine::EditDone)
		{
			// See if setting tool info made it "valid"
			if (m_curLineNum == NoCurrentLine)
				InsertIfValid();
			else
				m_scroll.InvalidateLine(m_curLineNum);

			ShowToolInfo();
			EndEdit(s_editDesc);
		}
	}

	void FileKeyHit(void *pvUser, uint key)
	{
		EditLine::EditStatus	edit;

		edit = s_editFile.ProcessKey(key);
		CheckIfFolder();
		if (edit == EditLine::EditDone)
		{
			EndEdit(s_editFile);
			if (m_isFolder)
				Files.Refresh();
		}
	}

	ToolLibInfo *PtrCurrentTool()
	{
		if (m_curLineNum == NoCurrentLine)
			return &s_bufTool;
		return PtrFromLine(m_curLineNum);
	}

	bool IsToolBuffered()
	{
		return m_curLineNum == NoCurrentLine || s_arSortList[m_curLineNum] == ToolBufIndex;
	}

	void SelectLine(uint line) NO_INLINE_ATTR
	{
		uint	oldLine;

		oldLine = m_curLineNum;
		m_curLineNum = line;
		if (oldLine != NoCurrentLine)
			m_scroll.InvalidateLine(oldLine);
		if (line != NoCurrentLine)
		{
			Eeprom.Data.Tool = PtrFromLine(line)->number;
			Eeprom.StartSave();
			m_scroll.InvalidateLine(line);
			m_scroll.ScrollToLine(line);
		}
	}

	uint FindTool(uint num)
	{
		uint	line;

		for (line = 0; line < m_toolCount; line++)
		{
			if (PtrFromLine(line)->number == num)
			{
				SelectLine(line);
				if (m_modToolOffset != ToolNotModified)
					DEBUG_PRINT("Find replacing modified tool\n");
				m_modToolOffset = ToolNotModified;
				return line;
			}
		}
		return NoCurrentLine;
	}

	void SaveTool()
	{
		if (m_curLineNum == NoCurrentLine)
			return;

		if (m_modToolOffset != ToolNotModified)
		{
			// Invalidate existing tool
			ToolFromOffset(m_modToolOffset)->number = 0;
			Nvm::WritePageReady();

			m_modToolOffset = ToolNotModified;
			SetToolButtonImage(TOOL_IMAGE_NotModified);
			goto NewTool;
		}

		if (s_arSortList[m_curLineNum] == ToolBufIndex)
		{
NewTool:
			// New tool
			if (m_pFreeTool >= ToolListEnd || m_pFreeTool->number != EmptyTool)
				m_pFreeTool = FindEmptyTool();

			*m_pFreeTool = s_bufTool;
			Nvm::WritePageReady();
			s_arSortList[m_curLineNum] = OffsetFromTool(m_pFreeTool);
			m_pFreeTool++;
		}
	}

	void SaveImportedTool()
	{
		m_curLineNum = InsertTool(ToolBufIndex);
		SaveTool();
	}

	ToolLibInfo *FindEmptyTool()
	{
		ToolLibInfo	*pTool, *pToolFree;

		for (pTool = ToolListStart; pTool->number != InvalidTool; pTool++)
		{
			if (pTool->number == EmptyTool)
				return pTool;
		}
		pToolFree = pTool;

		// Copy valid tools in row to buffer, then erase row
		pTool = (ToolLibInfo *)((ulong)pTool & (FlashRowSize - 1));	// start of row
		for (int i = 0; i < ToolsPerRow; i++)
		{
			if (pTool[i].number != InvalidTool && pTool[i].number != EmptyTool)
				ToolRowBuf[i] = pTool[i];
			else
				ToolRowBuf[i].number = EmptyTool;
		}
		Nvm::EraseRowReady(pToolFree);

		// Write data back to row
		for (int i = 0; i < ToolsPerRow; i++)
		{
			if (pTool->number != EmptyTool)
			{
				ToolRowBuf[i] = pTool[i];
				Nvm::WritePage();
			}
		}
		return pToolFree;
	}

	void InsertIfValid()
	{
		uint	line;

		if (s_bufTool.IsValid())
		{
			// Tool now valid, insert it
			line = InsertTool(ToolBufIndex);
			SelectLine(line);
			m_scroll.InvalidateLines(line + 1, m_toolCount);
		}
	}

	uint InsertTool(uint offset) NO_INLINE_ATTR
	{
		uint	line;
		uint	num;

		if (m_toolCount >= MaxToolCount)
			return NoCurrentLine;

		num = PtrFromOffset(offset)->number;

		for (line = 0; line < m_toolCount && PtrFromLine(line)->number <= num; line++);

		// We do not update the display in case this is a mass import
		memmove(&s_arSortList[line + 1], &s_arSortList[line], (m_toolCount - line) * sizeof s_arSortList[0]);
		s_arSortList[line] = offset;
		if (m_modToolOffset != ToolNotModified)
			DEBUG_PRINT("Inserting  modified tool\n");
		m_modToolOffset = ToolNotModified;
		m_toolCount++;
		m_scroll.SetTotalLines(m_toolCount);
		return line;
	}

	void DeleteTool()
	{
		uint	line;
		ToolLibInfo	*pTool;

		// We should only be here when displaying an unmodified tool
		line = m_curLineNum;

		// Tool might be in buffer only, not yet saved
		pTool = PtrFromLine(line);
		if (pTool == &s_bufTool)
			pTool->ClearData();
		else
		{
			pTool->number = InvalidTool;
			Nvm::WritePageReady();
		}

		// Remove from sort list
		m_toolCount--;
		memmove(&s_arSortList[line], &s_arSortList[line + 1], (m_toolCount - line) * sizeof s_arSortList[0]);
		if (line >= m_toolCount)
		{
			line--;
			SelectLine(line);
		}
		m_scroll.SetTotalLines(m_toolCount);
		m_scroll.InvalidateLines(line, m_toolCount + 1);
		ShowToolInfo();
	}

	void EndEdit(EditLine &edit)
	{
		edit.EndEdit();
		m_editMode = EDIT_None;
		KeyboardMgr::CloseKb();
	}

	void StartEditTool()
	{
		m_modToolOffset = s_arSortList[m_curLineNum];
		s_arSortList[m_curLineNum] = ToolBufIndex;
		SetToolButtonImage(TOOL_IMAGE_IsModified);
	}

	void StartEditToolDesc(int pos)
	{
		m_editMode = EDIT_Description;
		s_editDesc.StartEditPx(pos);
		KeyboardMgr::OpenKb(ToolEntryKeyHitCallback);
	}

protected:
	static ToolLibInfo *PtrFromOffset(uint offset)
	{
		if (offset == ToolBufIndex)
			return &s_bufTool;

		return ToolFromOffset(offset);
	}

	static ToolLibInfo *PtrFromLine(uint line)
	{
		return PtrFromOffset(s_arSortList[line]);
	}

	//*********************************************************************
	// Import/Export dialog

protected:
	void CloseImportExport()
	{
		if (m_editMode == EDIT_File)
			EndEdit(s_editFile);

		FileOp.SetErrorHandler(NULL);
		ShowToolLib();
	}

	void CheckIfFolder(bool fAlwaysSetIcon = false)
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
		if (fAlwaysSetIcon || isFolder != m_isFolder)
		{
			m_isFolder = isFolder;
			Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, 
				&LoadSave, isFolder ? FILE_IMAGE_OpenFolder : m_isExport);
		}
	}

	void StartEditFile(int pos)
	{
		if (m_editMode >= EDIT_StartErrors)
			ClearFileError();
		m_editMode = EDIT_File;
		s_editFile.StartEditPx(pos);
		KeyboardMgr::OpenKb(FileKeyHitCallback);
	}

	void ListUpdate(FileBrowser::NotifyReason reason)
	{
		switch (reason)
		{
		case FileBrowser::SelectionChanged:
			// Callback when file/folder selected from list
			if (m_editMode < EDIT_StartErrors)
				CheckIfFolder();
			else
			{
				uint editMode = m_editMode;
				ClearFileError();
				if (editMode == EDIT_FolderCreatePrompt)
					StartEditFile(EditLine::EndLinePx);
			}
			break;

		case FileBrowser::DriveChanged:
			if (m_editMode == EDIT_File)
				EndEdit(s_editFile);

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

	int FileError(int err)
	{
		DEBUG_PRINT("File error %i\n", -err);

		// Inform file browser
		if (err == FATERR_FileNotFound && m_isFolder && m_isExport)
		{
			m_editMode = EDIT_FolderCreatePrompt;
			Files.FileError(s_CreateFolderMsg);
			Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, 
				&LoadSave, FILE_IMAGE_AddFolder);
		}
		else
		{
			m_editMode = EDIT_FileError;
			Files.FileError(err);
		}

		return err;
	}

	void ClearFileError()
	{
		m_editMode = EDIT_None;
		Files.FileError();
		CheckIfFolder(true);
	}

	void ShowTimeSet()
	{
		RtcTime		time;

		if (m_editMode == EDIT_File)
			EndEdit(s_editFile);
		Lcd.EnablePip2(&EnterDateTime, 0, ToolListTop);
		m_editMode = EDIT_Time;
		ShowExportTime(time.ReadClock());
	}

	void EndTimeSet()
	{
		Lcd.EnablePip2(&Files, 0, ToolListTop);
		m_editMode = EDIT_None;
	}

	//*********************************************************************
	// Static helpers

protected:
	static void OpenImportExport()
	{
		Lcd.EnablePip1(&ToolImport, 0, 0);
		FileOp.SetErrorHandler(FileErrorCallback);
		Files.Open(&s_editFile, ListUpdateCallback);
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
		Lcd.SelectImage(&ToolImport, &ToolImport_Areas.UsbDriveBox, &RadioButtons, index);
		// SD card
		index = map & SdDriveMap ? (drive == SdDrive ? RADIO_True : RADIO_False) : RADIO_NotAvailable;
		Lcd.SelectImage(&ToolImport, &ToolImport_Areas.SdDriveBox, &RadioButtons, index);
	}

	static void PrintTimeDigits(uint u)
	{
		s_TimeEntry.printf("%02u", u);
	}

	static bool IsMetric()
	{
		return Eeprom.Data.fIsMetric;
	}

	static bool IsLibShown()
	{
		return Lcd.GetPip1()->pImage == &ToolLibrary;
	}

	static double LimitVal(double val, double max)
	{
		if (IsMetric())
			max *= 10.0;
		return CheckMetric(std::min(val, max), false);
	}

	static void PrepareDrawTool()
	{
		Lcd.SetDrawCanvas(&MainScreen);
		Lcd.WriteRegXY(ELL_A0, ToolImageRadius, ToolImageRadius);
	}

	static void DrawTool(bool fEnable, uint x, uint y)
	{
		Lcd.WriteRegXY(DEHR0, x, y);
		Lcd.SetForeColor(fEnable ? ToolColor : NoToolColor);
		Lcd.WriteReg(DCR1, DCR1_DrawEllipse | DCR1_FillOn | DCR1_DrawActive);
		Lcd.WaitWhileBusy();
	}

	static double CheckMetric(double val, bool fToDisplay = true)
	{
		double	factor;

		if (Eeprom.Data.fIsMetric == Eeprom.Data.fToolLibMetric)
			return val;

		factor = (fToDisplay ^ Eeprom.Data.fToolLibMetric) ? MmPerInch : 1.0 / MmPerInch;
		return val * factor;
	}

	static double CheckMetricSurface(double val, bool fToDisplay = true)
	{
		double	factor;

		if (Eeprom.Data.fIsMetric == Eeprom.Data.fToolLibMetric)
			return val;

		factor = MmPerInch * 12.0 / 1000.0;	// meters / foot
		if (!(fToDisplay ^ Eeprom.Data.fToolLibMetric))
			factor = 1 / factor;
		return val * factor;
	}

	static void SetToolButtonImage(ToolButtonImages image)
	{
		Lcd.SelectImage(&ToolLibrary, &ToolLibrary_Areas.ToolButtons, &ToolButtons, image);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const char s_archImportHead[] = IMPORT_HEAD_TEXT "\r\n";
	inline static const char s_InvalidToolFileMsg[] = "Not a valid tool library file";
	inline static const char s_InvalidToolMsg[] = "Error in file - not all tools imported";
	inline static const char s_CreateFolderMsg[] = "Folder not found - use Add Folder to create";

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	ToolLibInfo	*m_pFreeTool;
	ushort		m_toolCount;
	ushort		m_curLineNum;
	ushort		m_modToolOffset;
	byte		m_toolSides;
	byte		m_editMode;
	bool		m_isExport;
	bool		m_isFolder;
	ToolScroll	m_scroll;

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static ToolLibInfo	s_bufTool;

	inline static ToolDisplay	s_textMain {MainScreen, MainScreen_Areas.ToolNumber,
		ScreenForeColor, ScreenBackColor};
	inline static ToolDisplay	s_textInfo  {ToolLibrary, ToolLibrary_Areas.ToolNumber,
		ToolInfoForeground, ToolInfoBackground};
	inline static ToolDisplay	s_textList  {ToolRow, ToolRow_Areas.ToolNumber,
		ToolLibForeground, ToolLibBackground};

	inline static EditLine		s_editDesc {ToolLibrary, ToolLibrary_Areas.ToolDesc,
		s_bufTool.arDesc, ToolDescSize, FONT_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static EditLine		s_editFile{ToolImport, ToolImport_Areas.FileName, FileBrowser::GetPathBuf(),
		FileBrowser::GetPathBufSize(), FONT_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static NumberLine	s_LiveTime{ToolImport, ToolImport_Areas.LiveTime, 
		FONT_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static TextField		s_TimeEntry{EnterDateTime, EnterDateTime_Areas.Month, 
		FONT_SettingsFont, TimeEntryForeground, TimeEntryBackground};

	inline static NumberLineBlankZ	s_feedRate {MainScreen, MainScreen_Areas.CurrentFeedRate,
		FONT_SettingsFont, FeedRateColor, ToolDiagramColor};

	inline static ushort		s_arSortList[MaxToolCount];
};

extern ToolLib Tools;
