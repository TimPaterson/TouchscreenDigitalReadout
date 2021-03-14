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
	static constexpr int ToolEntrySize = 64;
	static constexpr int MaxToolCount = 70;
	static constexpr ushort NoCurrentLine = 0xFFFF;		// occurs in s_curLineNum
	static constexpr ushort ToolBufIndex = 0xFFFF;		// occurs in s_arSortList[]
	static constexpr ushort ToolNotModified = 0xFFFF;	// occurs in s_modToolIndex

	//*********************************************************************
	// Types
	//*********************************************************************

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
			*(ToolLibBase *)this = *(ToolLibBase *)&src;
			strcpy(arDesc, src.arDesc);
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
		virtual void FillLine(int lineNum, Area *pArea)
		{
			Area	area;

			if (lineNum < s_toolCount)
			{
				if (lineNum == s_curLineNum)
					s_textList.SetTextColor(ToolLibSelected);
				s_textList.DisplayLine(PtrFromLine(lineNum));
				s_textList.SetTextColor(ToolLibForeground);
				Lcd.CopyRect(this, pArea, &ToolRow);
			}
			else
			{
				Lcd.FillRect(this, pArea, ToolLibBackground);
				if (lineNum == s_toolCount && s_toolCount != 0)
				{
					// Draw bottom line of last grid entry.
					area = *pArea;
					area.Height = 1;
					Lcd.CopyRect(this, &area, &ToolRow);
				}
			}
		}

		virtual void LineSelected(int lineNum)
		{
			SaveTool();
			SelectLine(lineNum);
			ShowToolInfo();
		}
	};

	class ToolDisplay : public NumberLineBlankZ
	{
	public:
		ToolDisplay(Canvas &canvas, const Area &area, ulong foreColor, ulong backColor) :
			NumberLineBlankZ(canvas, area, FID_CalcSmall, foreColor, backColor), arItemAreas{&area}
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
	static void ToolAction(uint spot, int x, int y);
	static void SetTime(uint spot);
	static void ShowExportTime(RtcTime time);
	static void ShowToolInfo();
	static int ImportTools(char *pb, uint cb, uint cbWrap);
	static int ImportTool(char *pchBuf);
	static void ImportDone();
	static char *ExportStart();
	static char *ExportTool(char *pBuf, uint iTool);
	static void ExportDone();

public:
	static void Init()
	{
		s_scroll.Init();

		s_modToolIndex = ToolNotModified;
		if (s_toolCount == 0)
			s_curLineNum = NoCurrentLine;

		s_scroll.SetTotalLines(s_toolCount);
		s_scroll.ScrollToLine(s_curLineNum);
	}

	static ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (s_scroll.StartCapture(x, y - ToolListTop, spot))
			return &s_scroll;
		return NULL;
	}

	static void SetToolSide(uint side)
	{
		s_toolSides = (s_toolSides & ~(side >> ToolMaskShift)) ^ side;
		ShowToolInfo();
	}

	static void ChangeUnits()
	{
		s_scroll.InvalidateLines(0, s_toolCount - 1);
		ShowToolInfo();
	}

	static void ToolEntryKeyHit(void *pvUser, uint key)
	{
		EditLine::EditStatus	edit;

		edit = s_editDesc.ProcessKey(key);
		if (edit == EditLine::EditDone)
		{
			// See if setting tool info made it "valid"
			if (s_curLineNum == NoCurrentLine)
				InsertIfValid();
			else
				s_scroll.InvalidateLine(s_curLineNum);

			ShowToolInfo();
			EndEdit(s_editDesc);
		}
	}

	static void FileKeyHit(void *pvUser, uint key)
	{
		EditLine::EditStatus	edit;

		edit = s_editFile.ProcessKey(key);
		CheckIfFolder();
		if (edit == EditLine::EditDone)
		{
			EndEdit(s_editFile);
			if (s_isFolder)
				Files.Refresh();
		}
	}

public:
	static void ShowToolLib()
	{
		Lcd.EnablePip1(&ToolLibrary, 0, 0);
		Lcd.EnablePip2(&s_scroll, 0, ToolListTop);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static ToolLibInfo *PtrFromIndex(uint index)
	{
		if (index == ToolBufIndex)
			return &s_bufTool;

		return &s_arToolInfo[index];
	}

	static ToolLibInfo *PtrFromLine(uint line)
	{
		return PtrFromIndex(s_arSortList[line]);
	}

	static ToolLibInfo *PtrCurrentTool()
	{
		if (s_curLineNum == NoCurrentLine)
			return &s_bufTool;
		return PtrFromLine(s_curLineNum);
	}

	static bool IsToolBuffered()
	{
		return s_curLineNum == NoCurrentLine || s_arSortList[s_curLineNum] == ToolBufIndex;
	}

	static void SelectLine(uint line) NO_INLINE_ATTR
	{
		uint	oldLine;

		oldLine = s_curLineNum;
		s_curLineNum = line;
		if (oldLine != NoCurrentLine)
			s_scroll.InvalidateLine(oldLine);
		if (line != NoCurrentLine)
		{
			s_scroll.InvalidateLine(line);
			s_scroll.ScrollToLine(line);
		}
	}

	static uint FindTool(uint num)
	{
		uint	line;

		for (line = 0; line < s_toolCount; line++)
		{
			if (PtrFromLine(line)->number == num)
			{
				SelectLine(line);
				if (s_modToolIndex != ToolNotModified)
					DEBUG_PRINT("Find replacing modified tool\n");
				s_modToolIndex = ToolNotModified;
				return line;
			}
		}
		return NoCurrentLine;
	}

	static void SaveTool()
	{
		if (s_curLineNum == NoCurrentLine)
			return;

		if (s_modToolIndex != ToolNotModified)
		{
			// Update existing tool
			s_arToolInfo[s_modToolIndex] = s_bufTool;
			s_arSortList[s_curLineNum] = s_modToolIndex;
			s_modToolIndex = ToolNotModified;
			SetToolButtonImage(TOOL_IMAGE_NotModified);
		}
		else if (s_arSortList[s_curLineNum] == ToolBufIndex)
		{
			// New tool
			if (s_freeToolIndex >= MaxToolCount)
			{
				// UNDONE: garbage collect tool list
				return;
			}

			s_arSortList[s_curLineNum] = s_freeToolIndex;
			s_arToolInfo[s_freeToolIndex++] = s_bufTool;
		}
	}

	static void InsertIfValid()
	{
		uint	line;

		if (s_bufTool.IsValid())
		{
			// Tool now valid, insert it
			line = InsertTool(ToolBufIndex);
			SelectLine(line);
			s_scroll.InvalidateLines(line + 1, s_toolCount);
		}
	}

	static uint InsertTool(uint bufIndex)
	{
		uint	line;
		uint	num;

		if (s_toolCount >= MaxToolCount)
			return NoCurrentLine;

		num = PtrFromIndex(bufIndex)->number;

		for (line = 0; line < s_toolCount && PtrFromLine(line)->number < num; line++);

		// We do not update the display in case this is a mass import
		memmove(&s_arSortList[line + 1], &s_arSortList[line], (s_toolCount - line) * sizeof s_arSortList[0]);
		s_arSortList[line] = bufIndex;
		if (s_modToolIndex != ToolNotModified)
			DEBUG_PRINT("Inserting  modified tool\n");
		s_modToolIndex = ToolNotModified;
		s_toolCount++;
		s_scroll.SetTotalLines(s_toolCount);
		return line;
	}

	static void DeleteTool()
	{
		uint	line;

		// We should only be here when displaying an unmodified tool
		line = s_curLineNum;

		// Overwrite tool data -- UNDONE: handle for flash
		// Tool might be in buffer only, not yet saved
		PtrFromLine(line)->ClearData();

		// Remove from sort list
		s_toolCount--;
		memmove(&s_arSortList[line], &s_arSortList[line + 1], (s_toolCount - line) * sizeof s_arSortList[0]);
		if (line >= s_toolCount)
		{
			line--;
			SelectLine(line);
		}
		s_scroll.SetTotalLines(s_toolCount);
		s_scroll.InvalidateLines(line, s_toolCount + 1);
		ShowToolInfo();
	}

	static void EndEdit(EditLine &edit)
	{
		edit.EndEdit();
		s_editMode = EDIT_None;
		KeyboardMgr::CloseKb();
	}

	static void StartEditTool()
	{
		s_modToolIndex = s_arSortList[s_curLineNum];
		s_arSortList[s_curLineNum] = ToolBufIndex;
		SetToolButtonImage(TOOL_IMAGE_IsModified);
	}

	static void StartEditToolDesc(int pos)
	{
		s_editMode = EDIT_Description;
		s_editDesc.StartEditPx(pos);
		KeyboardMgr::OpenKb(ToolEntryKeyHit);
	}

	//*********************************************************************
	// Import/Export dialog

protected:
	static void OpenImportExport()
	{
		Lcd.EnablePip1(&ToolImport, 0, 0);
		FileOp.SetErrorHandler(FileError);
		Files.Open(&s_editFile, ListUpdate);
	}

	static void CloseImportExport()
	{
		if (s_editMode == EDIT_File)
			EndEdit(s_editFile);

		FileOp.SetErrorHandler(NULL);
		ShowToolLib();
	}

	static void CheckIfFolder(bool fAlwaysSetIcon = false)
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
		if (fAlwaysSetIcon || isFolder != s_isFolder)
		{
			s_isFolder = isFolder;
			Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, 
				&LoadSave, isFolder ? FILE_IMAGE_OpenFolder : s_isExport);
		}
	}

	static void StartEditFile(int pos)
	{
		if (s_editMode >= EDIT_StartErrors)
			ClearFileError();
		s_editMode = EDIT_File;
		s_editFile.StartEditPx(pos);
		KeyboardMgr::OpenKb(FileKeyHit);
	}

	static void ListUpdate(FileBrowser::NotifyReason reason)
	{
		switch (reason)
		{
		case FileBrowser::SelectionChanged:
			// Callback when file/folder selected from list
			if (s_editMode < EDIT_StartErrors)
				CheckIfFolder();
			else
			{
				uint editMode = s_editMode;
				ClearFileError();
				if (editMode == EDIT_FolderCreatePrompt)
					StartEditFile(EditLine::EndLinePx);
			}
			break;

		case FileBrowser::DriveChanged:
			if (s_editMode == EDIT_File)
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

	static int FileError(int err)
	{
		DEBUG_PRINT("File error %i\n", -err);

		// Inform file browser
		if (err == FATERR_FileNotFound && s_isFolder && s_isExport)
		{
			s_editMode = EDIT_FolderCreatePrompt;
			Files.FileError(s_CreateFolderMsg);
			Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, 
				&LoadSave, FILE_IMAGE_AddFolder);
		}
		else
		{
			s_editMode = EDIT_FileError;
			Files.FileError(err);
		}

		return err;
	}

	static void ClearFileError()
	{
		s_editMode = EDIT_None;
		Files.FileError();
		CheckIfFolder(true);
	}

	static void ShowTimeSet()
	{
		RtcTime		time;

		if (s_editMode == EDIT_File)
			EndEdit(s_editFile);
		Lcd.EnablePip2(&EnterDateTime, 0, ToolListTop);
		s_editMode = EDIT_Time;
		ShowExportTime(time.ReadClock());
	}

	static void EndTimeSet()
	{
		Lcd.EnablePip2(&Files, 0, ToolListTop);
		s_editMode = EDIT_None;
	}

	static void PrintTimeDigits(uint u)
	{
		s_TimeEntry.printf("%02u", u);
	}

	//*********************************************************************
	// Static helpers

protected:
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
	inline static const char s_CreateFolderMsg[] = "Folder not found - use Add Folder to create";

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
		s_bufTool.arDesc, ToolDescSize, FID_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static EditLine		s_editFile{ToolImport, ToolImport_Areas.FileName, FileBrowser::GetPathBuf(),
		FileBrowser::GetPathBufSize(), FID_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static NumberLine	s_LiveTime{ToolImport, ToolImport_Areas.LiveTime, 
		FID_CalcSmall, ToolInfoForeground, ToolInfoBackground};

	inline static TextField		s_TimeEntry{EnterDateTime, EnterDateTime_Areas.Month, 
		FID_SettingsFont, TimeEntryForeground, TimeEntryBackground};

	inline static ushort		s_toolCount;
	inline static ushort		s_curLineNum {NoCurrentLine};
	inline static ushort		s_modToolIndex {ToolNotModified};
	inline static ushort		s_freeToolIndex;
	inline static byte			s_toolSides;
	inline static byte			s_editMode;
	inline static bool			s_isExport;
	inline static bool			s_isFolder;
	inline static ToolScroll	s_scroll;
	inline static ushort		s_arSortList[MaxToolCount];
	inline static ToolLibInfo	s_arToolInfo[MaxToolCount];
};
