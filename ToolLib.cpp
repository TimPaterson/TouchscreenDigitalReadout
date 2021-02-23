//****************************************************************************
// ToolLib.cpp
//
// Created 1/31/2021 5:23:39 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "ToolLib.h"
#include "Actions.h"
#include "FileOperations.h"


void ToolLib::ToolAction(uint spot, int x, int y)
{
	double	val;
	uint	tool;
	uint	line;
	ToolLibInfo	*pTool;

	if (s_editMode == EDIT_ConfirmDelete)
	{
		// Lock out anything but Confirm or Cancel deletion of a tool.
		// Confirm is ToolDone, Cancel is ToolImportExport.
		if (spot != ToolsDone && spot != ToolImportExport)
			return;

		if (spot == ToolsDone)
			DeleteTool();

		s_editMode = EDIT_None;
		ScreenMgr::SetPip1Modal(false);
		SetToolButtonImage(TOOL_IMAGE_NotModified);
		return;
	}

	if (spot == ToolNumber)
	{
		val = Actions::GetCalcValue();
		tool = (uint)std::min(val, 999.0);

		// Switching tools, save current tool if modified
		// It's already in the sort list
		SaveTool();
		line = FindTool(tool);

		if (line == NoCurrentLine)
		{
			// Tool number not present
			if (IsLibShown())
			{
				// Start entering new tool. If we're just clearing
				// the tool number, start with the last tool.
				if (s_curLineNum != NoCurrentLine && tool == 0)
				{
					if (s_arSortList[s_curLineNum] != ToolBufIndex)
						s_bufTool = *PtrFromLine(s_curLineNum);
				}
				else if (s_bufTool.number != 0)
					s_bufTool.ClearData();

				s_bufTool.number = tool;
				SelectLine(NoCurrentLine);
				InsertIfValid();
			}
			else
			{
				// Didn't find tool, display error
				s_textMain.SetArea(MainScreen_Areas.ToolNumber);
				s_textMain.ClearArea();
				s_textMain.WriteString("???");
				return;		// don't show tool info
			}
		}
		ShowToolInfo();
		return;
	}

	if (spot < ToolLastValue)
	{
		if (Actions::HasCalcValue() || spot == ToolDesc)
		{
			val = Actions::GetCalcValue();

			if (spot < ToolInfoLast)
			{
				// Copy the tool to the buffer if it's not already there.
				if (!IsToolBuffered())
				{
					s_bufTool = *PtrFromLine(s_curLineNum);
					if (IsLibShown())
					{
						// We're in the tool library
						s_modToolIndex = s_arSortList[s_curLineNum];
						s_arSortList[s_curLineNum] = ToolBufIndex;
						SetToolButtonImage(TOOL_IMAGE_IsModified);
					}
					else if (spot != ToolDesc)
					{
						// If changing something, tool number no longer valid.
						s_bufTool.number = 0;
						SelectLine(NoCurrentLine);
					}
					else
						return; // Description on main screen is read-only
				}
			}

			// Set the entry
			switch (spot)
			{
			case ToolFlutes:
				val = std::min(val, 99.0);
				s_bufTool.flutes = (int)val;
				break;

			case ToolDiameter:
				val = LimitVal(val, 9.999);
				s_bufTool.diameter = val;
				break;

			case ToolLength:
				val = LimitVal(val, 99.999);
				s_bufTool.length = val;
				break;

			case ToolDesc:
				x -= ToolLibrary_Areas.ToolDesc.Xpos;	// relative edit box
				if (s_editMode != EDIT_Description)
				{
					s_editMode = EDIT_Description;
					s_editDesc.StartEditPx(x);
					KeyboardMgr::OpenKb(ToolEntryKeyHit);
				}
				else
				{
					// Already editing description
					s_editDesc.SetPositionPx(x);
				}
				return;

			case ToolChipLoad:
				val = LimitVal(val, 0.9999);
				Eeprom.Data.ChipLoad = val;
				break;

			case ToolSfm:
				val = CheckMetricSurface(std::min(val, 9999.0), false);
				Eeprom.Data.Sfm = val;
				break;
			}

			// See if setting tool info made it "valid"
			if (s_curLineNum == NoCurrentLine)
				InsertIfValid();
			else
				s_scroll.InvalidateLine(s_curLineNum);

			ShowToolInfo();
		}
		else
		{
			// Just reading the entry
			pTool = PtrCurrentTool();

			switch (spot)
			{
			case ToolFlutes:
				val = pTool->flutes;
				break;

			case ToolDiameter:
				val = CheckMetric(pTool->diameter);
				break;

			case ToolLength:
				val = CheckMetric(pTool->length);
				break;

			case ToolChipLoad:
				val = CheckMetric(Eeprom.Data.ChipLoad);
				break;

			case ToolSfm:
				val = CheckMetricSurface(Eeprom.Data.Sfm);
				break;

			default:
				return;
			}
			Actions::SetCalcValue(val);
		}
		return;
	}

	switch (spot)
	{
	case ToolsDone:
		SaveTool();
		if (s_editMode == EDIT_Description)
			EndEdit(s_editDesc);

		ScreenMgr::DisablePip1();
		ScreenMgr::DisablePip2();
		break;

	case ToolDelete:
		if (s_curLineNum == NoCurrentLine)
		{
			s_bufTool.ClearData();
			ShowToolInfo();
			break;
		}
		else if (s_arSortList[s_curLineNum] == ToolBufIndex && s_modToolIndex != ToolNotModified)
		{
			// Tool is modified, treat as Cancel
			s_bufTool.ClearData();
			s_arSortList[s_curLineNum] = s_modToolIndex;
			s_modToolIndex = ToolNotModified;
			s_scroll.InvalidateLine(s_curLineNum);
			SetToolButtonImage(TOOL_IMAGE_NotModified);
			if (s_editMode == EDIT_Description)
				EndEdit(s_editDesc);
			ShowToolInfo();
			break;
		}
		s_editMode = EDIT_ConfirmDelete;
		ScreenMgr::SetPip1Modal(true);
		SetToolButtonImage(TOOL_IMAGE_ConfirmDelete);
		break;

	case ToolImportExport:
		SaveTool();
		if (s_editMode == EDIT_Description)
			EndEdit(s_editDesc);
		ShowImportExport();
		break;

	case ImpExpCancel:
		if (s_editMode == EDIT_File)
			EndEdit(s_editFile);

		ShowToolLib();
		break;

	case ImportRadio:
		s_isExport = false;
		goto SetImportExportImages;

	case ExportRadio:
		s_isExport = true;
SetImportExportImages:
		ScreenMgr::SelectImage(&ToolImport, &ToolImport_Areas.ImportBox, &RadioButtons, !s_isExport);
		ScreenMgr::SelectImage(&ToolImport, &ToolImport_Areas.ExportBox, &RadioButtons, s_isExport);
		if (!s_isFolder)
			ScreenMgr::SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, &LoadSave, s_isExport);
		if (s_isExport)
		{
			ScreenMgr::FillRect(&ToolImport, &ToolImport_Areas.ImportWarning, ToolInfoBackground);
			ScreenMgr::CopyRect(&ToolImport, &ToolImport_Areas.TimeLabel, &TimestampLabel);
			ScreenMgr::CopyRect(&ToolImport, &ToolImport_Areas.TimeSetButton, &TimeSetButton);
		}
		else
			ScreenMgr::CopyRect(&ToolImport, &ToolImport_Areas.ImportWarning, &ImportWarning);
		break;

	case ImpExpExecute:
		if (s_editMode == EDIT_File)
			EndEdit(s_editFile);

		CheckIfFolder();
		if (s_isFolder)
			Files.Open(&s_editFile);
		else
		{
			if (s_isExport)
				FileOp.ToolExport(FileBrowser::GetPathBuf());
			else
				FileOp.ToolImport(FileBrowser::GetPathBuf());
		}
		break;

	case FileName:
		x -= ToolImport_Areas.FileName.Xpos;	// relative edit box
		if (s_editMode != EDIT_File)
		{
			s_editMode = EDIT_File;
			s_editFile.StartEditPx(x);
			KeyboardMgr::OpenKb(FileKeyHit);
		}
		else
		{
			// Already editing description
			s_editFile.SetPositionPx(x);
		}
	}
}

void ToolLib::ShowToolInfo()
{
	double	val;
	uint	sides;
	ulong	color;
	ToolLibInfo	*pInfo;

	// Tool info
	pInfo = PtrCurrentTool();

	s_textMain.DisplayLine(pInfo);
	s_textInfo.DisplayLine(pInfo);

	// SFM
	s_textMain.PrintDbl(
		"%5.0f",
		CheckMetricSurface(Eeprom.Data.Sfm),
		MainScreen_Areas.Sfm);

	// Chip load
	s_textMain.PrintDbl(
		IsMetric() ? "%6.3f" : "%6.4f",
		CheckMetric(Eeprom.Data.ChipLoad),
		MainScreen_Areas.ChipLoad);

	// Compute and display RPM
	if (pInfo->diameter != 0 && Eeprom.Data.Sfm != 0)
	{
		val = Eeprom.Data.Sfm / (pInfo->diameter * M_PI);
		val *= Eeprom.Data.fToolLibMetric ? 1000 : 12;
		val = std::min(val, (double)Eeprom.Data.MaxRpm);
	}
	else
		val = 0;

	s_textMain.PrintUint("%5u", (uint)val, MainScreen_Areas.Rpm);

	// Compute and display feed rate
	val *= CheckMetric(Eeprom.Data.ChipLoad) * pInfo->flutes;
	s_textMain.PrintUint("%5u", (uint)val, MainScreen_Areas.FeedRate);

	// Update cutter radius offset
	PrepareDrawTool();
	sides = s_toolSides;
	DrawTool(sides & ToolLeftBit,  ToolLeft_X,  ToolLeft_Y);
	DrawTool(sides & ToolRightBit, ToolRight_X, ToolRight_Y);
	DrawTool(sides & ToolBackBit,  ToolBack_X,  ToolBack_Y);
	DrawTool(sides & ToolFrontBit, ToolFront_X, ToolFront_Y);

	val = CheckMetric(pInfo->diameter / 2);
	Xaxis.SetOffset(sides & ToolLeftBit ? val : (sides & ToolRightBit ? -val : 0));
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Yaxis.SetOffset(sides & ToolFrontBit ? -val : (sides & ToolBackBit ? val : 0));
	val = CheckMetric(pInfo->length);
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Zaxis.SetOffset(Eeprom.Data.fToolLenAffectsZ ? val : 0);

	color = Eeprom.Data.fHighlightOffset && pInfo->diameter != 0 ? ToolColor : AxisForeColor;
	Xaxis.SetTextColor(sides & (ToolLeftBit | ToolRightBit) ? color : AxisForeColor);
	Yaxis.SetTextColor(sides & (ToolBackBit | ToolFrontBit) ? color : AxisForeColor);
}


//*********************************************************************
// Tool Library Import
//*********************************************************************

int ToolLib::ImportTool(char *pchBuf)
{
	char	ch;
	char	*pchDesc;

	s_bufTool.number = strtoul(pchBuf, &pchBuf, 0);
	if (*pchBuf++ != ',')
		return -1;	// UNDONE: error handling on tool import

	s_bufTool.diameter = strtod(pchBuf, &pchBuf);
	if (*pchBuf++ != ',')
		return -1;	// UNDONE: error handling on tool import

	s_bufTool.length = strtod(pchBuf, &pchBuf);
	if (*pchBuf++ != ',')
		return -1;	// UNDONE: error handling on tool import

	s_bufTool.flutes = strtoul(pchBuf, &pchBuf, 0);
	if (*pchBuf++ != ',')
		return -1;	// UNDONE: error handling on tool import

	if (*pchBuf == '"')
	{
		pchBuf++;
		pchDesc = s_bufTool.arDesc;
		while (pchDesc < &s_bufTool.arDesc[ToolDescSize - 2])
		{
			ch = *pchBuf++;
			if (ch == '"')
			{
				if (*pchBuf == '"')
					pchBuf++;	// skip escaped quote
				else
					ch = 0;		// closing quote
			}
			*pchDesc++ = ch;
			if (ch == 0)
				break;
		}
	}
	else
		strncpy(s_bufTool.arDesc, pchBuf, ToolDescSize - 1);

	s_bufTool.arDesc[ToolDescSize - 1] = '\0';	// ensure null terminated
	s_curLineNum = InsertTool(ToolBufIndex);
	SaveTool();

	return 0;
}

int ToolLib::ImportTools(char *pchBuf, uint cb, uint cbWrap)
{
	char	*pch;
	int		cbLine;
	int		err;

	if (pchBuf == NULL)
	{
		// special flag to indicate start of import
		pch = (char *)memchr(g_FileBuf, '\r', cb);
		if (pch == NULL)
			return -1;	// UNDONE: error handling on tool import

		cbLine = pch - (char *)g_FileBuf;
		if (cbLine != IMPORT_HEAD_TEXT_LENGTH)
			return -1;	// UNDONE: error handling on tool import

		if (memcmp(g_FileBuf, s_archImportHead, cbLine) != 0)
			return -1;	// UNDONE: error handling on tool import

		pchBuf = pch + 1;
		cb -= cbLine + 1;
		Eeprom.Data.fToolLibMetric = Eeprom.Data.fIsMetric;
		// UNDONE: delete existing library
		s_freeToolIndex = 0;
		s_toolCount = 0;

	}

	if (cbWrap != 0)
	{
		// We reached the end of the last buffer before finding EOL.
		// Wrap around to the first buffer and search from there.
		pch = (char *)memchr(g_FileBuf, '\r', cbWrap);
		if (pch == NULL)
			return -1;	// UNDONE: error handling on tool import

		*pch = '\0';		// zero terminate
		cbLine = pch - (char *)g_FileBuf + 1;

		// Build complete line
		memmove(pchBuf - cbLine, pchBuf, cb);
		pchBuf -= cbLine;
		memcpy(pchBuf + cb, g_FileBuf, cbLine);

		err = ImportTool(pchBuf);
		if (err < 0)
			return err;

		pchBuf = (char *)g_FileBuf + cbLine;
		cb = cbWrap - cbLine;
	}

	while ((pch = (char *)memchr(pchBuf, '\r', cb)) != NULL)
	{
		*pch = '\0';		// zero terminate
		cbLine = pch - pchBuf + 1;

		err = ImportTool(pchBuf);
		if (err < 0)
			return err;

		pchBuf += cbLine;
		cb -= cbLine;
	}

	return cb;
}

void ToolLib::ImportDone()
{
	s_scroll.Invalidate();
	SelectLine(0);
	ShowToolInfo();
	ShowToolLib();
}


//*********************************************************************
// Tool Library Export
//*********************************************************************

char *ToolLib::ExportStart()
{
	memcpy(g_FileBuf, s_archImportHead, sizeof s_archImportHead - 1);
	return (char *)ADDOFFSET(g_FileBuf, sizeof s_archImportHead - 1);
}

char *ToolLib::ExportTool(char *pBuf, uint line)
{
	char	*pCur, *pQuote;
	int		cb;
	ToolLibInfo	*pInfo;

	if (line >= s_toolCount)
		return NULL;

	pInfo = PtrFromLine(line);
	pBuf += sprintf(pBuf, "%u,%g,%g,%u,\"", pInfo->number, pInfo->diameter, pInfo->length, pInfo->flutes);

	// Check for " character in description so we can escape it
	for (pCur = pInfo->arDesc; ;) 
	{
		pQuote = strchr(pCur, '"');
		if (pQuote == NULL)
		{
			pBuf += sprintf(pBuf, "%s\"\r\n", pCur);
			return pBuf;
		}
		cb = pQuote - pCur + 1;	// include "
		do 
		{
			*pBuf++ = *pCur++;
		} while (--cb != 0);
		*pBuf++ = '"';	// escape with double "
	}
}
void ToolLib::ExportDone()
{
	ShowToolLib();
}

