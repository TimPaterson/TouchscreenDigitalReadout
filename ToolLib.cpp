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
	bool	isExport;
	ToolLibInfo	*pTool;

	if (m_editMode == EDIT_ConfirmDelete)
	{
		// Lock out anything but Confirm or Cancel deletion of a tool.
		// Confirm is ToolDone, Cancel is ToolImportExport.
		if (spot != ToolsDone && spot != ToolImportExport)
			return;

		if (spot == ToolsDone)
			DeleteTool();

		m_editMode = EDIT_None;
		Lcd.SetPip1Modal(false);
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
				if (m_curLineNum != NoCurrentLine && tool == 0)
				{
					if (s_arSortList[m_curLineNum] != ToolBufIndex)
						s_bufTool = *PtrFromLine(m_curLineNum);
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
					s_bufTool = *PtrFromLine(m_curLineNum);
					if (IsLibShown())
						StartEditTool();	// We're in the tool library
					else
					{
						// If changing something, tool number no longer valid.
						s_bufTool.number = 0;
						SelectLine(NoCurrentLine);
					}
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
				if (m_editMode != EDIT_Description)
					StartEditToolDesc(x);
				else
					s_editDesc.SetPositionPx(x);	// Already editing description
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
			if (m_curLineNum == NoCurrentLine)
				InsertIfValid();
			else
				m_scroll.InvalidateLine(m_curLineNum);

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
		if (m_editMode == EDIT_Description)
			EndEdit(s_editDesc);

		Lcd.DisablePip1();
		Lcd.DisablePip2();
		break;

	case ToolDelete:
		if (m_curLineNum == NoCurrentLine)
		{
			s_bufTool.ClearData();
			ShowToolInfo();
			break;
		}
		else if (s_arSortList[m_curLineNum] == ToolBufIndex && m_modToolIndex != ToolNotModified)
		{
			// Tool is modified, treat as Cancel
			s_bufTool.ClearData();
			s_arSortList[m_curLineNum] = m_modToolIndex;
			m_modToolIndex = ToolNotModified;
			m_scroll.InvalidateLine(m_curLineNum);
			SetToolButtonImage(TOOL_IMAGE_NotModified);
			if (m_editMode == EDIT_Description)
				EndEdit(s_editDesc);
			ShowToolInfo();
			break;
		}
		m_editMode = EDIT_ConfirmDelete;
		Lcd.SetPip1Modal(true);
		SetToolButtonImage(TOOL_IMAGE_ConfirmDelete);
		break;

	case ToolImportExport:
		SaveTool();
		if (m_editMode == EDIT_Description)
			EndEdit(s_editDesc);
		OpenImportExport();
		break;

	case ClearDesc:
		if (!IsToolBuffered())
		{
			s_bufTool = *PtrFromLine(m_curLineNum);
			StartEditTool();
		}
		s_editDesc.DeleteText();
		StartEditToolDesc(0);
		break;

	//*********************************************************************
	// Import/Export dialog

	case ImportRadio:
		isExport = false;
		goto SetImportExportImages;

	case ExportRadio:
		isExport = true;
SetImportExportImages:
		// Ignore radio buttons while prompting to create folder
		if (m_editMode >= EDIT_StartErrors)
			break;

		m_isExport = isExport;
		Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImportBox, &RadioButtons, !isExport);
		Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ExportBox, &RadioButtons, isExport);
		if (!m_isFolder)
			Lcd.SelectImage(&ToolImport, &ToolImport_Areas.ImpExpButton, &LoadSave, isExport);
		if (isExport)
		{
			Lcd.FillRect(&ToolImport, &ToolImport_Areas.ImportWarning, ToolInfoBackground);
			Lcd.CopyRect(&ToolImport, &ToolImport_Areas.TimeLabel, &TimestampLabel);
			Lcd.CopyRect(&ToolImport, &ToolImport_Areas.TimeSetButton, &TimeSetButton);
			RtcTime time;
			ShowExportTime(time.ReadClock());
		}
		else
		{
			// Clear Set button first, as it's taller than ImportWarning
			Lcd.FillRect(&ToolImport, &ToolImport_Areas.TimeSetButton, ToolInfoBackground);
			Lcd.CopyRect(&ToolImport, &ToolImport_Areas.ImportWarning, &ImportWarning);
		}
		break;

	case ImpExpExecute:
		if (m_editMode == EDIT_File)
			EndEdit(s_editFile);

		else if (m_editMode >= EDIT_StartErrors)
		{
			uint editMode = m_editMode;
			ClearFileError();
			if (editMode == EDIT_FolderCreatePrompt)
				Files.Refresh(true);
			break;
		}

		if (m_isFolder)
			Files.Refresh();
		else
		{
			if (m_isExport)
				FileOp.ToolExport(FileBrowser::GetPathBuf(), Files.GetDrive());
			else
				FileOp.ToolImport(FileBrowser::GetPathBuf(), Files.GetDrive());
		}
		break;

	case ImpExpCancel:
		if (m_editMode >= EDIT_StartErrors)
			StartEditFile(EditLine::EndLinePx);
		else
			CloseImportExport();
		break;

	case FileName:
		x -= ToolImport_Areas.FileName.Xpos;	// relative edit box
		if (m_editMode != EDIT_File)
			StartEditFile(x);
		else
			s_editFile.SetPositionPx(x); // Already editing file
		break;

	case ClearFile:
		s_editFile.DeleteText();
		StartEditFile(0);
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

	case TimeSet:
		if (m_editMode == EDIT_FolderCreatePrompt)
			break;
		if (!m_isExport)
			break;		// button only displayed on Export

		if (m_editMode == EDIT_Time)
			EndTimeSet();
		else
			ShowTimeSet();
		break;

	case TimeEntryDone:
		EndTimeSet();
		break;
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
	sides = m_toolSides;
	DrawTool(sides & ToolLeftBit,  ToolLeft_X,  ToolLeft_Y);
	DrawTool(sides & ToolRightBit, ToolRight_X, ToolRight_Y);
	DrawTool(sides & ToolBackBit,  ToolBack_X,  ToolBack_Y);
	DrawTool(sides & ToolFrontBit, ToolFront_X, ToolFront_Y);

	val = CheckMetric(pInfo->diameter / 2);
	Xdisplay.SetOffset(sides & ToolLeftBit ? val : (sides & ToolRightBit ? -val : 0));
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Ydisplay.SetOffset(sides & ToolFrontBit ? -val : (sides & ToolBackBit ? val : 0));
	val = CheckMetric(pInfo->length);
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Zdisplay.SetOffset(Eeprom.Data.fToolLenAffectsZ ? val : 0);

	color = Eeprom.Data.fHighlightOffset && pInfo->diameter != 0 ? ToolColor : AxisForeColor;
	Xdisplay.SetTextColor(sides & (ToolLeftBit | ToolRightBit) ? color : AxisForeColor);
	Ydisplay.SetTextColor(sides & (ToolBackBit | ToolFrontBit) ? color : AxisForeColor);
}


//*********************************************************************
// Clock Set (for tool library export timestamp)
//*********************************************************************

void ToolLib::SetTime(uint spot)
{
	RtcTime	time;
	uint	val;

	if (spot != AmPm)
	{
		val = (uint)Actions::GetCalcValue();
		if (spot == Year)
		{
			// Accept 2- or 4-digit year
			if (val < RtcTime::BASE_YEAR)
				val += 2000;
			if (val < RtcTime::BASE_YEAR || val > RtcTime::MAX_YEAR)
				return;
		}
		else if (val >= 60)
			return;
	}
		 
	time.ReadClock();

	switch (spot)
	{
	case Second:
		time.SetSecond(val);
		break;

	case Minute:
		time.SetMinute(val);
		break;

	case Hour:
		time.SetHour(val, time.AmPm());
		break;

	case Day:
		time.SetDay(val);
		break;

	case Month:
		time.SetMonth(val);
		break;

	case Year:
		time.SetYear(val);
		break;

	case AmPm:
		time.SetHour(time.Hour12(), !time.AmPm());
		break;
	}

	time.SetClock();
}

void ToolLib::ShowExportTime(RtcTime time)
{
	if (!m_isExport)
		return;

	s_LiveTime.ResetPosition();
	s_LiveTime.printf("%2u/%02u/%u  %2u:%02u:%02u",
		time.Month(), time.Day(), time.Year(), time.Hour12(), time.Minute(), time.Second());
	s_LiveTime.SetSpaceWidth();
	s_LiveTime.WriteString(time.AmPm() ? " pm" : " am");
	s_LiveTime.SetSpaceWidth(s_LiveTime.GetCharWidth('0'));

	if (m_editMode == EDIT_Time)
	{
		s_TimeEntry.SetArea(EnterDateTime_Areas.Month);
		PrintTimeDigits(time.Month());

		s_TimeEntry.SetArea(EnterDateTime_Areas.Day);
		PrintTimeDigits(time.Day());

		s_TimeEntry.SetArea(EnterDateTime_Areas.Year);
		PrintTimeDigits(time.Year() - 2000);

		s_TimeEntry.SetArea(EnterDateTime_Areas.Hour);
		PrintTimeDigits(time.Hour12());

		s_TimeEntry.SetArea(EnterDateTime_Areas.Minute);
		PrintTimeDigits(time.Minute());

		s_TimeEntry.SetArea(EnterDateTime_Areas.Second);
		PrintTimeDigits(time.Second());

		s_TimeEntry.SetArea(EnterDateTime_Areas.AmPm);
		s_TimeEntry.WriteString(time.AmPm() ? "pm" : "am");
	}
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
	m_curLineNum = InsertTool(ToolBufIndex);
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
		m_freeToolIndex = 0;
		m_toolCount = 0;
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
	m_scroll.InvalidateAllLines();
	s_bufTool.ClearData();
	m_curLineNum = m_toolCount == 0 ? NoCurrentLine : 0;
	ShowToolInfo();
	CloseImportExport();
}


//*********************************************************************
// Tool Library Export
//*********************************************************************

char *ToolLib::ExportStart()
{
	memcpy(g_FileBuf, s_archImportHead, STRLEN(s_archImportHead));
	return (char *)ADDOFFSET(g_FileBuf, STRLEN(s_archImportHead));
}

char *ToolLib::ExportTool(char *pBuf, uint line)
{
	char	*pCur, *pQuote;
	int		cb;
	ToolLibInfo	*pInfo;

	if (line >= m_toolCount)
		return NULL;

	pInfo = PtrFromLine(line);
	pBuf += sprintf(pBuf, "%u,%.9g,%.9g,%u,\"", pInfo->number, CheckMetric(pInfo->diameter), 
		CheckMetric(pInfo->length), pInfo->flutes);

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
	Files.Refresh();
}

//*********************************************************************
// Static callbacks
//*********************************************************************

void ToolLib::ToolEntryKeyHitCallback(void *pvUser, uint key)
{
	Tools.ToolEntryKeyHit(pvUser, key);
}

void ToolLib::FileKeyHitCallback(void *pvUser, uint key)
{
	Tools.FileKeyHit(pvUser, key);
}

void ToolLib::ListUpdateCallback(FileBrowser::NotifyReason reason)
{
	Tools.ListUpdate(reason);
}

int ToolLib::FileErrorCallback(int err)
{
	return Tools.FileError(err);
}

//*********************************************************************
// Implement functions in ListScroll
//*********************************************************************

void ToolLib::ToolScroll::FillLine(int lineNum, Area *pArea)
{
	Area	area;

	if (lineNum < Tools.m_toolCount)
	{
		if (lineNum == Tools.m_curLineNum)
			s_textList.SetTextColor(ToolLibSelected);
		s_textList.DisplayLine(PtrFromLine(lineNum));
		s_textList.SetTextColor(ToolLibForeground);
		Lcd.CopyRect(this, pArea, &ToolRow);
	}
	else
	{
		Lcd.FillRect(this, pArea, ToolLibBackground);
		if (lineNum == Tools.m_toolCount && Tools.m_toolCount != 0)
		{
			// Draw bottom line of last grid entry.
			area = *pArea;
			area.Height = 1;
			Lcd.CopyRect(this, &area, &ToolRow);
		}
	}
}

void ToolLib::ToolScroll::LineSelected(int lineNum)
{
	Tools.SaveTool();
	Tools.SelectLine(lineNum);
	Tools.ShowToolInfo();
}
