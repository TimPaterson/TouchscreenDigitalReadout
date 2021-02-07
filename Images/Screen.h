#ifndef START_SCREEN
#define START_SCREEN(a)
#endif
#ifndef IMAGE_ADDRESS
#define IMAGE_ADDRESS(a)
#endif
#ifndef IMAGE_SIZE
#define IMAGE_SIZE(a)
#endif
#ifndef IMAGE_WIDTH
#define IMAGE_WIDTH(a)
#endif
#ifndef IMAGE_HEIGHT
#define IMAGE_HEIGHT(a)
#endif
#ifndef IMAGE_STRIDE
#define IMAGE_STRIDE(a)
#endif
#ifndef IMAGE_DEPTH
#define IMAGE_DEPTH(a)
#endif
#ifndef END_SCREEN
#define END_SCREEN(a)
#endif
#ifndef START_HOTSPOTS
#define START_HOTSPOTS(a)
#endif
#ifndef DEFINE_HOTSPOT
#define DEFINE_HOTSPOT(a,b,c,d,e,f)
#endif
#ifndef END_HOTSPOTS
#define END_HOTSPOTS(a)
#endif
#ifndef HOTSPOT_COUNT
#define HOTSPOT_COUNT(a,b)
#endif
#ifndef START_LOCATIONS
#define START_LOCATIONS(a)
#endif
#ifndef DEFINE_LOCATION
#define DEFINE_LOCATION(a,b,c)
#endif
#ifndef END_LOCATIONS
#define END_LOCATIONS(a)
#endif
#ifndef START_AREAS
#define START_AREAS(a)
#endif
#ifndef DEFINE_AREA
#define DEFINE_AREA(a,b,c,d,e)
#endif
#ifndef END_AREAS
#define END_AREAS(a)
#endif
#ifndef START_GROUP
#define START_GROUP(a)
#endif
#ifndef GROUP_HOTSPOT
#define GROUP_HOTSPOT(a,b,c,d,e,f)
#endif
#ifndef END_GROUP
#define END_GROUP(a)
#endif
#ifndef START_VALUES
#define START_VALUES()
#endif
#ifndef DEFINE_VALUE
#define DEFINE_VALUE(a,b)
#endif
#ifndef END_VALUES
#define END_VALUES()
#endif
#ifndef START_STR_VALUES
#define START_STR_VALUES()
#endif
#ifndef DEFINE_STR_VALUE
#define DEFINE_STR_VALUE(a,b)
#endif
#ifndef END_STR_VALUES
#define END_STR_VALUES()
#endif
#ifndef DEFINE_COLOR
#define DEFINE_COLOR(a,b)
#endif
#ifndef SCREEN_FILE_LENGTH
#define SCREEN_FILE_LENGTH(a)
#endif

DEFINE_COLOR(ScreenForeColor, 0x000000)
DEFINE_COLOR(ScreenBackColor, 0xADDBE7)
DEFINE_COLOR(AxisForeColor, 0x000000)
DEFINE_COLOR(AxisBackColor, 0xFFFF00)
DEFINE_COLOR(CalcBackColor, 0xFFFFFF)
DEFINE_COLOR(MemColorOdd, 0xD6C3D6)
DEFINE_COLOR(MemColorEven, 0x9CFB9C)
DEFINE_COLOR(BtnBackground, 0x606060)
DEFINE_COLOR(ToolInfoBackground, 0x0000FF)
DEFINE_COLOR(ToolInfoForeground, 0xFFFFFF)
DEFINE_COLOR(ToolLibBackground, 0x00008C)
DEFINE_COLOR(ToolLibForeground, 0xD6D3D6)
DEFINE_COLOR(ToolLibSelected, 0xFFFFFF)
DEFINE_COLOR(ToggleOn, 0xFFFF00)
DEFINE_COLOR(SetBtnText, 0xFFFF00)
DEFINE_COLOR(ToolColor, 0xC60000)
DEFINE_COLOR(NoToolColor, 0x84CFFF)
DEFINE_COLOR(SettingBackColor, 0x000000)
DEFINE_COLOR(SettingForeColor, 0xFFFFFF)
DEFINE_COLOR(CheckForeground, 0xFFFFFF)
DEFINE_COLOR(CheckBackground, 0x000000)

START_VALUES()
	DEFINE_VALUE(ScrollGapLeft, 4)
	DEFINE_VALUE(ScrollGapRight, 0)
	DEFINE_VALUE(ScrollThumbWidth, 60)
	DEFINE_VALUE(ScrollBarWidth, 64)
	DEFINE_VALUE(ToolImageRadius, 15)
	DEFINE_VALUE(ToolListHeight, 483)
	DEFINE_VALUE(ToolListWidth, 576)
	DEFINE_VALUE(ToolRowHeight, 50)
	DEFINE_VALUE(ToolListTop, 117)
END_VALUES()

START_STR_VALUES()
END_STR_VALUES()

START_SCREEN(MainScreen)
	IMAGE_ADDRESS(0)
	IMAGE_SIZE(1228800)
	IMAGE_WIDTH(1024)
	IMAGE_HEIGHT(600)
	IMAGE_STRIDE(1024)
	IMAGE_DEPTH(Color16bpp)
END_SCREEN(MainScreen)

START_HOTSPOTS(MainScreen)
	DEFINE_HOTSPOT(Xsensor, Axis, 0, 12, 409, 122)
	DEFINE_HOTSPOT(Xsensor, AxisButton, 435, 19, 519, 114)
	DEFINE_HOTSPOT(Ysensor, Axis, 0, 148, 409, 258)
	DEFINE_HOTSPOT(Ysensor, AxisButton, 435, 155, 519, 250)
	DEFINE_HOTSPOT(Zsensor, Axis, 0, 284, 409, 394)
	DEFINE_HOTSPOT(Zsensor, AxisButton, 435, 291, 519, 386)
	DEFINE_HOTSPOT(Settings, , 894, 10, 1013, 69)
	DEFINE_HOTSPOT(BrightDown, , 894, 80, 953, 139)
	DEFINE_HOTSPOT(BrightUp, , 954, 80, 1013, 139)
	DEFINE_HOTSPOT(Undo, , 584, 25, 873, 108)
	DEFINE_HOTSPOT(CalcDisplay, , 584, 119, 873, 178)
	DEFINE_HOTSPOT(InchMetric, , 884, 517, 943, 576)
	DEFINE_HOTSPOT(AbsInc, , 954, 517, 1013, 576)
	DEFINE_HOTSPOT(Key_backSpace, Edit, 594, 237, 653, 296)
	DEFINE_HOTSPOT(Key_clear, Edit, 664, 237, 723, 296)
	DEFINE_HOTSPOT(Key_pi, Edit, 734, 237, 793, 296)
	DEFINE_HOTSPOT(Key_plus, Operator, 804, 237, 863, 296)
	DEFINE_HOTSPOT(Mem1, Memory, 874, 237, 1023, 296)
	DEFINE_HOTSPOT(Key_7, Digit, 594, 307, 653, 366)
	DEFINE_HOTSPOT(Key_8, Digit, 664, 307, 723, 366)
	DEFINE_HOTSPOT(Key_9, Digit, 734, 307, 793, 366)
	DEFINE_HOTSPOT(Key_minus, Operator, 804, 307, 863, 366)
	DEFINE_HOTSPOT(Mem2, Memory, 874, 307, 1023, 366)
	DEFINE_HOTSPOT(Key_4, Digit, 594, 377, 653, 436)
	DEFINE_HOTSPOT(Key_5, Digit, 664, 377, 723, 436)
	DEFINE_HOTSPOT(Key_6, Digit, 734, 377, 793, 436)
	DEFINE_HOTSPOT(Key_mult, Operator, 804, 377, 863, 436)
	DEFINE_HOTSPOT(Mem3, Memory, 874, 377, 1023, 436)
	DEFINE_HOTSPOT(Key_1, Digit, 594, 447, 653, 506)
	DEFINE_HOTSPOT(Key_2, Digit, 664, 447, 723, 506)
	DEFINE_HOTSPOT(Key_3, Digit, 734, 447, 793, 506)
	DEFINE_HOTSPOT(Key_divide, Operator, 804, 447, 863, 506)
	DEFINE_HOTSPOT(Mem4, Memory, 874, 447, 1023, 506)
	DEFINE_HOTSPOT(Key_sign, Digit, 594, 517, 653, 576)
	DEFINE_HOTSPOT(Key_0, Digit, 664, 517, 723, 576)
	DEFINE_HOTSPOT(Key_decimal, Digit, 734, 517, 793, 576)
	DEFINE_HOTSPOT(Key_equal, Operator, 804, 517, 863, 576)
	DEFINE_HOTSPOT(ToolNumber, ToolLib, 10, 405, 64, 465)
	DEFINE_HOTSPOT(ToolDiameter, ToolLib, 64, 405, 134, 465)
	DEFINE_HOTSPOT(ToolFlutes, ToolLib, 134, 405, 189, 465)
	DEFINE_HOTSPOT(ToolLength, ToolLib, 189, 405, 274, 465)
	DEFINE_HOTSPOT(ToolDesc, ToolLib, 274, 405, 503, 465)
	DEFINE_HOTSPOT(ToolMenu, , 514, 405, 573, 464)
	DEFINE_HOTSPOT(ToolLeft, ToolSide, 10, 505, 69, 564)
	DEFINE_HOTSPOT(ToolBack, ToolSide, 70, 475, 129, 534)
	DEFINE_HOTSPOT(ToolFront, ToolSide, 70, 535, 129, 594)
	DEFINE_HOTSPOT(ToolRight, ToolSide, 130, 505, 189, 564)
	DEFINE_HOTSPOT(ToolSfm, ToolLib, 200, 475, 379, 570)
	DEFINE_HOTSPOT(ToolChipLoad, ToolLib, 390, 475, 569, 570)
END_HOTSPOTS(MainScreen)

HOTSPOT_COUNT(MainScreen, 49)

START_LOCATIONS(MainScreen)
	DEFINE_LOCATION(Undo3, 584, 25)
	DEFINE_LOCATION(Undo2, 584, 53)
	DEFINE_LOCATION(Undo1, 584, 81)
	DEFINE_LOCATION(ToolLeft, 25, 535)
	DEFINE_LOCATION(ToolRight, 175, 535)
	DEFINE_LOCATION(ToolBack, 100, 490)
	DEFINE_LOCATION(ToolFront, 100, 580)
END_LOCATIONS(MainScreen)

START_AREAS(MainScreen)
	DEFINE_AREA(Xdisplay, 0, 12, 410, 111)
	DEFINE_AREA(Ydisplay, 0, 148, 410, 111)
	DEFINE_AREA(Zdisplay, 0, 284, 410, 111)
	DEFINE_AREA(Undo, 584, 25, 290, 84)
	DEFINE_AREA(CalcDisplay, 584, 119, 290, 60)
	DEFINE_AREA(CalcText, 584, 189, 290, 28)
	DEFINE_AREA(InchMetric, 884, 517, 60, 60)
	DEFINE_AREA(AbsInc, 954, 517, 60, 60)
	DEFINE_AREA(Mem1, 884, 252, 140, 30)
	DEFINE_AREA(Mem2, 884, 322, 140, 30)
	DEFINE_AREA(Mem3, 884, 392, 140, 30)
	DEFINE_AREA(Mem4, 884, 462, 140, 30)
	DEFINE_AREA(ToolNumber, 18, 437, 45, 28)
	DEFINE_AREA(ToolDiameter, 69, 437, 64, 28)
	DEFINE_AREA(ToolFlutes, 143, 437, 45, 28)
	DEFINE_AREA(ToolLength, 195, 437, 78, 28)
	DEFINE_AREA(ToolDesc, 279, 437, 224, 28)
	DEFINE_AREA(SpeedDisplay, 200, 475, 371, 97)
	DEFINE_AREA(Sfm, 305, 509, 74, 28)
	DEFINE_AREA(ChipLoad, 488, 509, 81, 28)
	DEFINE_AREA(Rpm, 305, 541, 74, 28)
	DEFINE_AREA(FeedRate, 495, 541, 74, 28)
END_AREAS(MainScreen)

START_HOTSPOTS(Coord)
END_HOTSPOTS(Coord)

HOTSPOT_COUNT(Coord, 0)

START_HOTSPOTS(InchMetricBtn)
END_HOTSPOTS(InchMetricBtn)

HOTSPOT_COUNT(InchMetricBtn, 0)

START_HOTSPOTS(SpeedDisplay)
	DEFINE_HOTSPOT(ToolSfm, ToolLib, 0, 0, 179, 95)
	DEFINE_HOTSPOT(ToolChipLoad, ToolLib, 190, 0, 369, 95)
	DEFINE_HOTSPOT(ToolSfm, ToolLib, 371, 0, 550, 95)
	DEFINE_HOTSPOT(ToolChipLoad, ToolLib, 561, 0, 740, 95)
END_HOTSPOTS(SpeedDisplay)

HOTSPOT_COUNT(SpeedDisplay, 4)

START_SCREEN(ToolLibrary)
	IMAGE_ADDRESS(1401936)
	IMAGE_SIZE(239616)
	IMAGE_WIDTH(1024)
	IMAGE_HEIGHT(117)
	IMAGE_STRIDE(1024)
	IMAGE_DEPTH(Color16bpp)
END_SCREEN(ToolLibrary)

START_HOTSPOTS(ToolLibrary)
	DEFINE_HOTSPOT(ToolNumber, ToolLib, 10, 10, 64, 70)
	DEFINE_HOTSPOT(ToolDiameter, ToolLib, 64, 10, 134, 70)
	DEFINE_HOTSPOT(ToolFlutes, ToolLib, 134, 10, 189, 70)
	DEFINE_HOTSPOT(ToolLength, ToolLib, 189, 10, 274, 70)
	DEFINE_HOTSPOT(ToolDesc, ToolLib, 274, 10, 803, 70)
	DEFINE_HOTSPOT(ToolsDone, ToolLib, 814, 10, 873, 69)
	DEFINE_HOTSPOT(ToolDelete, ToolLib, 884, 10, 943, 69)
	DEFINE_HOTSPOT(ToolImportExport, ToolLib, 954, 10, 1013, 69)
END_HOTSPOTS(ToolLibrary)

HOTSPOT_COUNT(ToolLibrary, 8)

START_AREAS(ToolLibrary)
	DEFINE_AREA(ToolNumber, 18, 28, 45, 28)
	DEFINE_AREA(ToolDiameter, 69, 28, 64, 28)
	DEFINE_AREA(ToolFlutes, 143, 28, 45, 28)
	DEFINE_AREA(ToolLength, 195, 28, 78, 28)
	DEFINE_AREA(ToolDesc, 279, 28, 524, 28)
	DEFINE_AREA(ToolButtons, 814, 10, 200, 107)
END_AREAS(ToolLibrary)

START_SCREEN(ToolRow)
	IMAGE_ADDRESS(1641552)
	IMAGE_SIZE(52224)
	IMAGE_WIDTH(512)
	IMAGE_HEIGHT(51)
	IMAGE_STRIDE(512)
	IMAGE_DEPTH(Color16bpp)
END_SCREEN(ToolRow)

START_HOTSPOTS(ToolRow)
END_HOTSPOTS(ToolRow)

HOTSPOT_COUNT(ToolRow, 0)

START_AREAS(ToolRow)
	DEFINE_AREA(ToolNumber, 18, 12, 45, 28)
	DEFINE_AREA(ToolDiameter, 69, 12, 64, 28)
	DEFINE_AREA(ToolFlutes, 143, 12, 45, 28)
	DEFINE_AREA(ToolLength, 195, 12, 78, 28)
	DEFINE_AREA(ToolDesc, 279, 12, 232, 28)
END_AREAS(ToolRow)

START_HOTSPOTS(ToolButtons)
END_HOTSPOTS(ToolButtons)

HOTSPOT_COUNT(ToolButtons, 0)

START_SCREEN(SettingsScreen)
	IMAGE_ADDRESS(1822176)
	IMAGE_SIZE(345600)
	IMAGE_WIDTH(576)
	IMAGE_HEIGHT(600)
	IMAGE_STRIDE(576)
	IMAGE_DEPTH(Color8bpp)
END_SCREEN(SettingsScreen)

START_HOTSPOTS(SettingsScreen)
	DEFINE_HOTSPOT(Xsensor, Resolution, 63, 70, 182, 130)
	DEFINE_HOTSPOT(Xsensor, Invert, 182, 70, 301, 130)
	DEFINE_HOTSPOT(Xsensor, Correction, 301, 70, 420, 130)
	DEFINE_HOTSPOT(Ysensor, Resolution, 63, 130, 182, 190)
	DEFINE_HOTSPOT(Ysensor, Invert, 182, 130, 301, 190)
	DEFINE_HOTSPOT(Ysensor, Correction, 301, 130, 420, 190)
	DEFINE_HOTSPOT(Zsensor, Resolution, 63, 190, 182, 250)
	DEFINE_HOTSPOT(Zsensor, Invert, 182, 190, 301, 250)
	DEFINE_HOTSPOT(Zsensor, Correction, 301, 190, 420, 250)
	DEFINE_HOTSPOT(Qsensor, Resolution, 63, 250, 182, 310)
	DEFINE_HOTSPOT(Qsensor, Invert, 182, 250, 301, 310)
	DEFINE_HOTSPOT(Qsensor, Correction, 301, 250, 420, 310)
	DEFINE_HOTSPOT(HighlightXY, SettingToggle, 10, 320, 354, 379)
	DEFINE_HOTSPOT(OffsetZ, SettingToggle, 10, 390, 354, 449)
	DEFINE_HOTSPOT(CncCoordinates, SettingToggle, 10, 460, 354, 519)
	DEFINE_HOTSPOT(MaxRpm, , 10, 530, 354, 589)
	DEFINE_HOTSPOT(TouchCal, , 365, 320, 564, 379)
END_HOTSPOTS(SettingsScreen)

HOTSPOT_COUNT(SettingsScreen, 17)

START_AREAS(SettingsScreen)
	DEFINE_AREA(Xresolution, 113, 80, 69, 42)
	DEFINE_AREA(Xinvert, 226, 84, 32, 32)
	DEFINE_AREA(Xcorrection, 306, 80, 114, 42)
	DEFINE_AREA(Yresolution, 113, 140, 69, 42)
	DEFINE_AREA(Yinvert, 226, 144, 32, 32)
	DEFINE_AREA(Ycorrection, 306, 140, 114, 42)
	DEFINE_AREA(Zresolution, 113, 200, 69, 42)
	DEFINE_AREA(Zinvert, 226, 204, 32, 32)
	DEFINE_AREA(Zcorrection, 306, 200, 114, 42)
	DEFINE_AREA(Qresolution, 113, 260, 69, 42)
	DEFINE_AREA(Qinvert, 226, 264, 32, 32)
	DEFINE_AREA(Qcorrection, 306, 260, 114, 42)
	DEFINE_AREA(HighlightXY, 313, 334, 32, 32)
	DEFINE_AREA(OffsetZ, 313, 404, 32, 32)
	DEFINE_AREA(CncCoordinates, 313, 474, 32, 32)
	DEFINE_AREA(MaxRpm, 245, 540, 100, 42)
END_AREAS(SettingsScreen)

START_HOTSPOTS(CheckBox)
END_HOTSPOTS(CheckBox)

HOTSPOT_COUNT(CheckBox, 0)

START_SCREEN(KeyLower)
	IMAGE_ADDRESS(2169824)
	IMAGE_SIZE(292320)
	IMAGE_WIDTH(810)
	IMAGE_HEIGHT(360)
	IMAGE_STRIDE(812)
	IMAGE_DEPTH(Color8bpp)
END_SCREEN(KeyLower)

START_HOTSPOTS(KeyLower)
	DEFINE_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	DEFINE_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	DEFINE_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	DEFINE_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	DEFINE_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	DEFINE_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	DEFINE_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	DEFINE_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	DEFINE_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	DEFINE_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	DEFINE_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	DEFINE_HOTSPOT(Key_q, Keyboard, 40, 80, 99, 139)
	DEFINE_HOTSPOT(Key_w, Keyboard, 110, 80, 169, 139)
	DEFINE_HOTSPOT(Key_e, Keyboard, 180, 80, 239, 139)
	DEFINE_HOTSPOT(Key_r, Keyboard, 250, 80, 309, 139)
	DEFINE_HOTSPOT(Key_t, Keyboard, 320, 80, 379, 139)
	DEFINE_HOTSPOT(Key_y, Keyboard, 390, 80, 449, 139)
	DEFINE_HOTSPOT(Key_u, Keyboard, 460, 80, 519, 139)
	DEFINE_HOTSPOT(Key_i, Keyboard, 530, 80, 589, 139)
	DEFINE_HOTSPOT(Key_o, Keyboard, 600, 80, 659, 139)
	DEFINE_HOTSPOT(Key_p, Keyboard, 670, 80, 729, 139)
	DEFINE_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	DEFINE_HOTSPOT(Key_a, Keyboard, 70, 150, 129, 209)
	DEFINE_HOTSPOT(Key_s, Keyboard, 140, 150, 199, 209)
	DEFINE_HOTSPOT(Key_d, Keyboard, 210, 150, 269, 209)
	DEFINE_HOTSPOT(Key_f, Keyboard, 280, 150, 339, 209)
	DEFINE_HOTSPOT(Key_g, Keyboard, 350, 150, 409, 209)
	DEFINE_HOTSPOT(Key_h, Keyboard, 420, 150, 479, 209)
	DEFINE_HOTSPOT(Key_j, Keyboard, 490, 150, 549, 209)
	DEFINE_HOTSPOT(Key_k, Keyboard, 560, 150, 619, 209)
	DEFINE_HOTSPOT(Key_l, Keyboard, 630, 150, 689, 209)
	DEFINE_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	DEFINE_HOTSPOT(Key_z, Keyboard, 110, 220, 169, 279)
	DEFINE_HOTSPOT(Key_x, Keyboard, 180, 220, 239, 279)
	DEFINE_HOTSPOT(Key_c, Keyboard, 250, 220, 309, 279)
	DEFINE_HOTSPOT(Key_v, Keyboard, 320, 220, 379, 279)
	DEFINE_HOTSPOT(Key_b, Keyboard, 390, 220, 449, 279)
	DEFINE_HOTSPOT(Key_n, Keyboard, 460, 220, 519, 279)
	DEFINE_HOTSPOT(Key_m, Keyboard, 530, 220, 589, 279)
	DEFINE_HOTSPOT(Key_comma, Keyboard, 600, 220, 659, 279)
	DEFINE_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	DEFINE_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	DEFINE_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	DEFINE_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	DEFINE_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	DEFINE_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	DEFINE_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
END_HOTSPOTS(KeyLower)

HOTSPOT_COUNT(KeyLower, 49)

START_SCREEN(KeyUpper)
	IMAGE_ADDRESS(2462144)
	IMAGE_SIZE(292320)
	IMAGE_WIDTH(810)
	IMAGE_HEIGHT(360)
	IMAGE_STRIDE(812)
	IMAGE_DEPTH(Color8bpp)
END_SCREEN(KeyUpper)

START_HOTSPOTS(KeyUpper)
	DEFINE_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	DEFINE_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	DEFINE_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	DEFINE_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	DEFINE_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	DEFINE_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	DEFINE_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	DEFINE_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	DEFINE_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	DEFINE_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	DEFINE_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	DEFINE_HOTSPOT(Key_Q, Keyboard, 40, 80, 99, 139)
	DEFINE_HOTSPOT(Key_W, Keyboard, 110, 80, 169, 139)
	DEFINE_HOTSPOT(Key_E, Keyboard, 180, 80, 239, 139)
	DEFINE_HOTSPOT(Key_R, Keyboard, 250, 80, 309, 139)
	DEFINE_HOTSPOT(Key_T, Keyboard, 320, 80, 379, 139)
	DEFINE_HOTSPOT(Key_Y, Keyboard, 390, 80, 449, 139)
	DEFINE_HOTSPOT(Key_U, Keyboard, 460, 80, 519, 139)
	DEFINE_HOTSPOT(Key_I, Keyboard, 530, 80, 589, 139)
	DEFINE_HOTSPOT(Key_O, Keyboard, 600, 80, 659, 139)
	DEFINE_HOTSPOT(Key_P, Keyboard, 670, 80, 729, 139)
	DEFINE_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	DEFINE_HOTSPOT(Key_A, Keyboard, 70, 150, 129, 209)
	DEFINE_HOTSPOT(Key_S, Keyboard, 140, 150, 199, 209)
	DEFINE_HOTSPOT(Key_D, Keyboard, 210, 150, 269, 209)
	DEFINE_HOTSPOT(Key_F, Keyboard, 280, 150, 339, 209)
	DEFINE_HOTSPOT(Key_G, Keyboard, 350, 150, 409, 209)
	DEFINE_HOTSPOT(Key_H, Keyboard, 420, 150, 479, 209)
	DEFINE_HOTSPOT(Key_J, Keyboard, 490, 150, 549, 209)
	DEFINE_HOTSPOT(Key_K, Keyboard, 560, 150, 619, 209)
	DEFINE_HOTSPOT(Key_L, Keyboard, 630, 150, 689, 209)
	DEFINE_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	DEFINE_HOTSPOT(Key_Z, Keyboard, 110, 220, 169, 279)
	DEFINE_HOTSPOT(Key_X, Keyboard, 180, 220, 239, 279)
	DEFINE_HOTSPOT(Key_C, Keyboard, 250, 220, 309, 279)
	DEFINE_HOTSPOT(Key_V, Keyboard, 320, 220, 379, 279)
	DEFINE_HOTSPOT(Key_B, Keyboard, 390, 220, 449, 279)
	DEFINE_HOTSPOT(Key_N, Keyboard, 460, 220, 519, 279)
	DEFINE_HOTSPOT(Key_M, Keyboard, 530, 220, 589, 279)
	DEFINE_HOTSPOT(Key_comma, Keyboard, 600, 220, 659, 279)
	DEFINE_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	DEFINE_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	DEFINE_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	DEFINE_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	DEFINE_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	DEFINE_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	DEFINE_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
END_HOTSPOTS(KeyUpper)

HOTSPOT_COUNT(KeyUpper, 49)

START_SCREEN(KeySymbol)
	IMAGE_ADDRESS(2754464)
	IMAGE_SIZE(292320)
	IMAGE_WIDTH(810)
	IMAGE_HEIGHT(360)
	IMAGE_STRIDE(812)
	IMAGE_DEPTH(Color8bpp)
END_SCREEN(KeySymbol)

START_HOTSPOTS(KeySymbol)
	DEFINE_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	DEFINE_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	DEFINE_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	DEFINE_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	DEFINE_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	DEFINE_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	DEFINE_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	DEFINE_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	DEFINE_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	DEFINE_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	DEFINE_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	DEFINE_HOTSPOT(Key_tilde, Keyboard, 40, 80, 99, 139)
	DEFINE_HOTSPOT(Key_exclamation, Keyboard, 110, 80, 169, 139)
	DEFINE_HOTSPOT(Key_at, Keyboard, 180, 80, 239, 139)
	DEFINE_HOTSPOT(Key_hash, Keyboard, 250, 80, 309, 139)
	DEFINE_HOTSPOT(Key_dollar, Keyboard, 320, 80, 379, 139)
	DEFINE_HOTSPOT(Key_percent, Keyboard, 390, 80, 449, 139)
	DEFINE_HOTSPOT(Key_hat, Keyboard, 460, 80, 519, 139)
	DEFINE_HOTSPOT(Key_star, Keyboard, 530, 80, 589, 139)
	DEFINE_HOTSPOT(Key_lparen, Keyboard, 600, 80, 659, 139)
	DEFINE_HOTSPOT(Key_rparen, Keyboard, 670, 80, 729, 139)
	DEFINE_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	DEFINE_HOTSPOT(Key_backApostrophe, Keyboard, 70, 150, 129, 209)
	DEFINE_HOTSPOT(Key_ampersand, Keyboard, 140, 150, 199, 209)
	DEFINE_HOTSPOT(Key_underscore, Keyboard, 210, 150, 269, 209)
	DEFINE_HOTSPOT(Key_plus, Keyboard, 280, 150, 339, 209)
	DEFINE_HOTSPOT(Key_equal, Keyboard, 350, 150, 409, 209)
	DEFINE_HOTSPOT(Key_lbracket, Keyboard, 420, 150, 479, 209)
	DEFINE_HOTSPOT(Key_rbracket, Keyboard, 490, 150, 549, 209)
	DEFINE_HOTSPOT(Key_lbrace, Keyboard, 560, 150, 619, 209)
	DEFINE_HOTSPOT(Key_rbrace, Keyboard, 630, 150, 689, 209)
	DEFINE_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	DEFINE_HOTSPOT(Key_vbar, Keyboard, 110, 220, 169, 279)
	DEFINE_HOTSPOT(Key_backslash, Keyboard, 180, 220, 239, 279)
	DEFINE_HOTSPOT(Key_colon, Keyboard, 250, 220, 309, 279)
	DEFINE_HOTSPOT(Key_semi, Keyboard, 320, 220, 379, 279)
	DEFINE_HOTSPOT(Key_apostrophe, Keyboard, 390, 220, 449, 279)
	DEFINE_HOTSPOT(Key_lessthan, Keyboard, 460, 220, 519, 279)
	DEFINE_HOTSPOT(Key_greaterthan, Keyboard, 530, 220, 589, 279)
	DEFINE_HOTSPOT(Key_question, Keyboard, 600, 220, 659, 279)
	DEFINE_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	DEFINE_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	DEFINE_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	DEFINE_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	DEFINE_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	DEFINE_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	DEFINE_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	DEFINE_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
END_HOTSPOTS(KeySymbol)

HOTSPOT_COUNT(KeySymbol, 49)

START_GROUP(Axis)
	GROUP_HOTSPOT(Xsensor, Axis, 0, 12, 409, 122)
	GROUP_HOTSPOT(Ysensor, Axis, 0, 148, 409, 258)
	GROUP_HOTSPOT(Zsensor, Axis, 0, 284, 409, 394)
END_GROUP(Axis)

START_GROUP(AxisButton)
	GROUP_HOTSPOT(Xsensor, AxisButton, 435, 19, 519, 114)
	GROUP_HOTSPOT(Ysensor, AxisButton, 435, 155, 519, 250)
	GROUP_HOTSPOT(Zsensor, AxisButton, 435, 291, 519, 386)
END_GROUP(AxisButton)

START_GROUP()
	GROUP_HOTSPOT(Settings, , 894, 10, 1013, 69)
	GROUP_HOTSPOT(BrightDown, , 894, 80, 953, 139)
	GROUP_HOTSPOT(BrightUp, , 954, 80, 1013, 139)
	GROUP_HOTSPOT(Undo, , 584, 25, 873, 108)
	GROUP_HOTSPOT(CalcDisplay, , 584, 119, 873, 178)
	GROUP_HOTSPOT(InchMetric, , 884, 517, 943, 576)
	GROUP_HOTSPOT(AbsInc, , 954, 517, 1013, 576)
	GROUP_HOTSPOT(ToolMenu, , 514, 405, 573, 464)
	GROUP_HOTSPOT(MaxRpm, , 10, 530, 354, 589)
	GROUP_HOTSPOT(TouchCal, , 365, 320, 564, 379)
END_GROUP()

START_GROUP(Edit)
	GROUP_HOTSPOT(Key_backSpace, Edit, 594, 237, 653, 296)
	GROUP_HOTSPOT(Key_clear, Edit, 664, 237, 723, 296)
	GROUP_HOTSPOT(Key_pi, Edit, 734, 237, 793, 296)
END_GROUP(Edit)

START_GROUP(Operator)
	GROUP_HOTSPOT(Key_plus, Operator, 804, 237, 863, 296)
	GROUP_HOTSPOT(Key_minus, Operator, 804, 307, 863, 366)
	GROUP_HOTSPOT(Key_mult, Operator, 804, 377, 863, 436)
	GROUP_HOTSPOT(Key_divide, Operator, 804, 447, 863, 506)
	GROUP_HOTSPOT(Key_equal, Operator, 804, 517, 863, 576)
END_GROUP(Operator)

START_GROUP(Memory)
	GROUP_HOTSPOT(Mem1, Memory, 874, 237, 1023, 296)
	GROUP_HOTSPOT(Mem2, Memory, 874, 307, 1023, 366)
	GROUP_HOTSPOT(Mem3, Memory, 874, 377, 1023, 436)
	GROUP_HOTSPOT(Mem4, Memory, 874, 447, 1023, 506)
END_GROUP(Memory)

START_GROUP(Digit)
	GROUP_HOTSPOT(Key_7, Digit, 594, 307, 653, 366)
	GROUP_HOTSPOT(Key_8, Digit, 664, 307, 723, 366)
	GROUP_HOTSPOT(Key_9, Digit, 734, 307, 793, 366)
	GROUP_HOTSPOT(Key_4, Digit, 594, 377, 653, 436)
	GROUP_HOTSPOT(Key_5, Digit, 664, 377, 723, 436)
	GROUP_HOTSPOT(Key_6, Digit, 734, 377, 793, 436)
	GROUP_HOTSPOT(Key_1, Digit, 594, 447, 653, 506)
	GROUP_HOTSPOT(Key_2, Digit, 664, 447, 723, 506)
	GROUP_HOTSPOT(Key_3, Digit, 734, 447, 793, 506)
	GROUP_HOTSPOT(Key_sign, Digit, 594, 517, 653, 576)
	GROUP_HOTSPOT(Key_0, Digit, 664, 517, 723, 576)
	GROUP_HOTSPOT(Key_decimal, Digit, 734, 517, 793, 576)
END_GROUP(Digit)

START_GROUP(ToolLib)
	GROUP_HOTSPOT(ToolNumber, ToolLib, 10, 405, 64, 465)
	GROUP_HOTSPOT(ToolDiameter, ToolLib, 64, 405, 134, 465)
	GROUP_HOTSPOT(ToolFlutes, ToolLib, 134, 405, 189, 465)
	GROUP_HOTSPOT(ToolLength, ToolLib, 189, 405, 274, 465)
	GROUP_HOTSPOT(ToolDesc, ToolLib, 274, 405, 503, 465)
	GROUP_HOTSPOT(ToolSfm, ToolLib, 200, 475, 379, 570)
	GROUP_HOTSPOT(ToolChipLoad, ToolLib, 390, 475, 569, 570)
	GROUP_HOTSPOT(ToolSfm, ToolLib, 0, 0, 179, 95)
	GROUP_HOTSPOT(ToolChipLoad, ToolLib, 190, 0, 369, 95)
	GROUP_HOTSPOT(ToolSfm, ToolLib, 371, 0, 550, 95)
	GROUP_HOTSPOT(ToolChipLoad, ToolLib, 561, 0, 740, 95)
	GROUP_HOTSPOT(ToolNumber, ToolLib, 10, 10, 64, 70)
	GROUP_HOTSPOT(ToolDiameter, ToolLib, 64, 10, 134, 70)
	GROUP_HOTSPOT(ToolFlutes, ToolLib, 134, 10, 189, 70)
	GROUP_HOTSPOT(ToolLength, ToolLib, 189, 10, 274, 70)
	GROUP_HOTSPOT(ToolDesc, ToolLib, 274, 10, 803, 70)
	GROUP_HOTSPOT(ToolsDone, ToolLib, 814, 10, 873, 69)
	GROUP_HOTSPOT(ToolDelete, ToolLib, 884, 10, 943, 69)
	GROUP_HOTSPOT(ToolImportExport, ToolLib, 954, 10, 1013, 69)
END_GROUP(ToolLib)

START_GROUP(ToolSide)
	GROUP_HOTSPOT(ToolLeft, ToolSide, 10, 505, 69, 564)
	GROUP_HOTSPOT(ToolBack, ToolSide, 70, 475, 129, 534)
	GROUP_HOTSPOT(ToolFront, ToolSide, 70, 535, 129, 594)
	GROUP_HOTSPOT(ToolRight, ToolSide, 130, 505, 189, 564)
END_GROUP(ToolSide)

START_GROUP(Resolution)
	GROUP_HOTSPOT(Xsensor, Resolution, 63, 70, 182, 130)
	GROUP_HOTSPOT(Ysensor, Resolution, 63, 130, 182, 190)
	GROUP_HOTSPOT(Zsensor, Resolution, 63, 190, 182, 250)
	GROUP_HOTSPOT(Qsensor, Resolution, 63, 250, 182, 310)
END_GROUP(Resolution)

START_GROUP(Invert)
	GROUP_HOTSPOT(Xsensor, Invert, 182, 70, 301, 130)
	GROUP_HOTSPOT(Ysensor, Invert, 182, 130, 301, 190)
	GROUP_HOTSPOT(Zsensor, Invert, 182, 190, 301, 250)
	GROUP_HOTSPOT(Qsensor, Invert, 182, 250, 301, 310)
END_GROUP(Invert)

START_GROUP(Correction)
	GROUP_HOTSPOT(Xsensor, Correction, 301, 70, 420, 130)
	GROUP_HOTSPOT(Ysensor, Correction, 301, 130, 420, 190)
	GROUP_HOTSPOT(Zsensor, Correction, 301, 190, 420, 250)
	GROUP_HOTSPOT(Qsensor, Correction, 301, 250, 420, 310)
END_GROUP(Correction)

START_GROUP(SettingToggle)
	GROUP_HOTSPOT(HighlightXY, SettingToggle, 10, 320, 354, 379)
	GROUP_HOTSPOT(OffsetZ, SettingToggle, 10, 390, 354, 449)
	GROUP_HOTSPOT(CncCoordinates, SettingToggle, 10, 460, 354, 519)
END_GROUP(SettingToggle)

START_GROUP(Keyboard)
	GROUP_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT(Key_q, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT(Key_w, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT(Key_e, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT(Key_r, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT(Key_t, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT(Key_y, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT(Key_u, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT(Key_i, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT(Key_o, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT(Key_p, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT(Key_a, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT(Key_s, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT(Key_d, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT(Key_f, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT(Key_g, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT(Key_h, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT(Key_j, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT(Key_k, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT(Key_l, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT(Key_z, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT(Key_x, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT(Key_c, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT(Key_v, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT(Key_b, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT(Key_n, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT(Key_m, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT(Key_comma, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
	GROUP_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT(Key_Q, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT(Key_W, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT(Key_E, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT(Key_R, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT(Key_T, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT(Key_Y, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT(Key_U, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT(Key_I, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT(Key_O, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT(Key_P, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT(Key_A, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT(Key_S, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT(Key_D, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT(Key_F, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT(Key_G, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT(Key_H, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT(Key_J, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT(Key_K, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT(Key_L, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT(Key_Z, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT(Key_X, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT(Key_C, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT(Key_V, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT(Key_B, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT(Key_N, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT(Key_M, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT(Key_comma, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
	GROUP_HOTSPOT(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT(Key_tilde, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT(Key_exclamation, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT(Key_at, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT(Key_hash, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT(Key_dollar, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT(Key_percent, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT(Key_hat, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT(Key_star, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT(Key_lparen, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT(Key_rparen, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT(Key_backApostrophe, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT(Key_ampersand, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT(Key_underscore, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT(Key_plus, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT(Key_equal, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT(Key_lbracket, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT(Key_rbracket, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT(Key_lbrace, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT(Key_rbrace, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT(Key_vbar, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT(Key_backslash, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT(Key_colon, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT(Key_semi, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT(Key_apostrophe, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT(Key_lessthan, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT(Key_greaterthan, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT(Key_question, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT(Key_backspace, Keyboard, 710, 290, 769, 349)
END_GROUP(Keyboard)

#ifndef START_GROUP_Axis
#define START_GROUP_Axis(a)
#endif
#ifndef GROUP_HOTSPOT_Axis
#define GROUP_HOTSPOT_Axis(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Axis
#define END_GROUP_Axis(a)
#endif

START_GROUP_Axis(Axis)
	GROUP_HOTSPOT_Axis(Xsensor, Axis, 0, 12, 409, 122)
	GROUP_HOTSPOT_Axis(Ysensor, Axis, 0, 148, 409, 258)
	GROUP_HOTSPOT_Axis(Zsensor, Axis, 0, 284, 409, 394)
END_GROUP_Axis(Axis)

#undef START_GROUP_Axis
#undef GROUP_HOTSPOT_Axis
#undef END_GROUP_Axis

#ifndef START_GROUP_AxisButton
#define START_GROUP_AxisButton(a)
#endif
#ifndef GROUP_HOTSPOT_AxisButton
#define GROUP_HOTSPOT_AxisButton(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_AxisButton
#define END_GROUP_AxisButton(a)
#endif

START_GROUP_AxisButton(AxisButton)
	GROUP_HOTSPOT_AxisButton(Xsensor, AxisButton, 435, 19, 519, 114)
	GROUP_HOTSPOT_AxisButton(Ysensor, AxisButton, 435, 155, 519, 250)
	GROUP_HOTSPOT_AxisButton(Zsensor, AxisButton, 435, 291, 519, 386)
END_GROUP_AxisButton(AxisButton)

#undef START_GROUP_AxisButton
#undef GROUP_HOTSPOT_AxisButton
#undef END_GROUP_AxisButton

#ifndef START_GROUP_
#define START_GROUP_(a)
#endif
#ifndef GROUP_HOTSPOT_
#define GROUP_HOTSPOT_(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_
#define END_GROUP_(a)
#endif

START_GROUP_()
	GROUP_HOTSPOT_(Settings, , 894, 10, 1013, 69)
	GROUP_HOTSPOT_(BrightDown, , 894, 80, 953, 139)
	GROUP_HOTSPOT_(BrightUp, , 954, 80, 1013, 139)
	GROUP_HOTSPOT_(Undo, , 584, 25, 873, 108)
	GROUP_HOTSPOT_(CalcDisplay, , 584, 119, 873, 178)
	GROUP_HOTSPOT_(InchMetric, , 884, 517, 943, 576)
	GROUP_HOTSPOT_(AbsInc, , 954, 517, 1013, 576)
	GROUP_HOTSPOT_(ToolMenu, , 514, 405, 573, 464)
	GROUP_HOTSPOT_(MaxRpm, , 10, 530, 354, 589)
	GROUP_HOTSPOT_(TouchCal, , 365, 320, 564, 379)
END_GROUP_()

#undef START_GROUP_
#undef GROUP_HOTSPOT_
#undef END_GROUP_

#ifndef START_GROUP_Edit
#define START_GROUP_Edit(a)
#endif
#ifndef GROUP_HOTSPOT_Edit
#define GROUP_HOTSPOT_Edit(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Edit
#define END_GROUP_Edit(a)
#endif

START_GROUP_Edit(Edit)
	GROUP_HOTSPOT_Edit(Key_backSpace, Edit, 594, 237, 653, 296)
	GROUP_HOTSPOT_Edit(Key_clear, Edit, 664, 237, 723, 296)
	GROUP_HOTSPOT_Edit(Key_pi, Edit, 734, 237, 793, 296)
END_GROUP_Edit(Edit)

#undef START_GROUP_Edit
#undef GROUP_HOTSPOT_Edit
#undef END_GROUP_Edit

#ifndef START_GROUP_Operator
#define START_GROUP_Operator(a)
#endif
#ifndef GROUP_HOTSPOT_Operator
#define GROUP_HOTSPOT_Operator(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Operator
#define END_GROUP_Operator(a)
#endif

START_GROUP_Operator(Operator)
	GROUP_HOTSPOT_Operator(Key_plus, Operator, 804, 237, 863, 296)
	GROUP_HOTSPOT_Operator(Key_minus, Operator, 804, 307, 863, 366)
	GROUP_HOTSPOT_Operator(Key_mult, Operator, 804, 377, 863, 436)
	GROUP_HOTSPOT_Operator(Key_divide, Operator, 804, 447, 863, 506)
	GROUP_HOTSPOT_Operator(Key_equal, Operator, 804, 517, 863, 576)
END_GROUP_Operator(Operator)

#undef START_GROUP_Operator
#undef GROUP_HOTSPOT_Operator
#undef END_GROUP_Operator

#ifndef START_GROUP_Memory
#define START_GROUP_Memory(a)
#endif
#ifndef GROUP_HOTSPOT_Memory
#define GROUP_HOTSPOT_Memory(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Memory
#define END_GROUP_Memory(a)
#endif

START_GROUP_Memory(Memory)
	GROUP_HOTSPOT_Memory(Mem1, Memory, 874, 237, 1023, 296)
	GROUP_HOTSPOT_Memory(Mem2, Memory, 874, 307, 1023, 366)
	GROUP_HOTSPOT_Memory(Mem3, Memory, 874, 377, 1023, 436)
	GROUP_HOTSPOT_Memory(Mem4, Memory, 874, 447, 1023, 506)
END_GROUP_Memory(Memory)

#undef START_GROUP_Memory
#undef GROUP_HOTSPOT_Memory
#undef END_GROUP_Memory

#ifndef START_GROUP_Digit
#define START_GROUP_Digit(a)
#endif
#ifndef GROUP_HOTSPOT_Digit
#define GROUP_HOTSPOT_Digit(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Digit
#define END_GROUP_Digit(a)
#endif

START_GROUP_Digit(Digit)
	GROUP_HOTSPOT_Digit(Key_7, Digit, 594, 307, 653, 366)
	GROUP_HOTSPOT_Digit(Key_8, Digit, 664, 307, 723, 366)
	GROUP_HOTSPOT_Digit(Key_9, Digit, 734, 307, 793, 366)
	GROUP_HOTSPOT_Digit(Key_4, Digit, 594, 377, 653, 436)
	GROUP_HOTSPOT_Digit(Key_5, Digit, 664, 377, 723, 436)
	GROUP_HOTSPOT_Digit(Key_6, Digit, 734, 377, 793, 436)
	GROUP_HOTSPOT_Digit(Key_1, Digit, 594, 447, 653, 506)
	GROUP_HOTSPOT_Digit(Key_2, Digit, 664, 447, 723, 506)
	GROUP_HOTSPOT_Digit(Key_3, Digit, 734, 447, 793, 506)
	GROUP_HOTSPOT_Digit(Key_sign, Digit, 594, 517, 653, 576)
	GROUP_HOTSPOT_Digit(Key_0, Digit, 664, 517, 723, 576)
	GROUP_HOTSPOT_Digit(Key_decimal, Digit, 734, 517, 793, 576)
END_GROUP_Digit(Digit)

#undef START_GROUP_Digit
#undef GROUP_HOTSPOT_Digit
#undef END_GROUP_Digit

#ifndef START_GROUP_ToolLib
#define START_GROUP_ToolLib(a)
#endif
#ifndef GROUP_HOTSPOT_ToolLib
#define GROUP_HOTSPOT_ToolLib(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_ToolLib
#define END_GROUP_ToolLib(a)
#endif

START_GROUP_ToolLib(ToolLib)
	GROUP_HOTSPOT_ToolLib(ToolNumber, ToolLib, 10, 405, 64, 465)
	GROUP_HOTSPOT_ToolLib(ToolDiameter, ToolLib, 64, 405, 134, 465)
	GROUP_HOTSPOT_ToolLib(ToolFlutes, ToolLib, 134, 405, 189, 465)
	GROUP_HOTSPOT_ToolLib(ToolLength, ToolLib, 189, 405, 274, 465)
	GROUP_HOTSPOT_ToolLib(ToolDesc, ToolLib, 274, 405, 503, 465)
	GROUP_HOTSPOT_ToolLib(ToolSfm, ToolLib, 200, 475, 379, 570)
	GROUP_HOTSPOT_ToolLib(ToolChipLoad, ToolLib, 390, 475, 569, 570)
	GROUP_HOTSPOT_ToolLib(ToolSfm, ToolLib, 0, 0, 179, 95)
	GROUP_HOTSPOT_ToolLib(ToolChipLoad, ToolLib, 190, 0, 369, 95)
	GROUP_HOTSPOT_ToolLib(ToolSfm, ToolLib, 371, 0, 550, 95)
	GROUP_HOTSPOT_ToolLib(ToolChipLoad, ToolLib, 561, 0, 740, 95)
	GROUP_HOTSPOT_ToolLib(ToolNumber, ToolLib, 10, 10, 64, 70)
	GROUP_HOTSPOT_ToolLib(ToolDiameter, ToolLib, 64, 10, 134, 70)
	GROUP_HOTSPOT_ToolLib(ToolFlutes, ToolLib, 134, 10, 189, 70)
	GROUP_HOTSPOT_ToolLib(ToolLength, ToolLib, 189, 10, 274, 70)
	GROUP_HOTSPOT_ToolLib(ToolDesc, ToolLib, 274, 10, 803, 70)
	GROUP_HOTSPOT_ToolLib(ToolsDone, ToolLib, 814, 10, 873, 69)
	GROUP_HOTSPOT_ToolLib(ToolDelete, ToolLib, 884, 10, 943, 69)
	GROUP_HOTSPOT_ToolLib(ToolImportExport, ToolLib, 954, 10, 1013, 69)
END_GROUP_ToolLib(ToolLib)

#undef START_GROUP_ToolLib
#undef GROUP_HOTSPOT_ToolLib
#undef END_GROUP_ToolLib

#ifndef START_GROUP_ToolSide
#define START_GROUP_ToolSide(a)
#endif
#ifndef GROUP_HOTSPOT_ToolSide
#define GROUP_HOTSPOT_ToolSide(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_ToolSide
#define END_GROUP_ToolSide(a)
#endif

START_GROUP_ToolSide(ToolSide)
	GROUP_HOTSPOT_ToolSide(ToolLeft, ToolSide, 10, 505, 69, 564)
	GROUP_HOTSPOT_ToolSide(ToolBack, ToolSide, 70, 475, 129, 534)
	GROUP_HOTSPOT_ToolSide(ToolFront, ToolSide, 70, 535, 129, 594)
	GROUP_HOTSPOT_ToolSide(ToolRight, ToolSide, 130, 505, 189, 564)
END_GROUP_ToolSide(ToolSide)

#undef START_GROUP_ToolSide
#undef GROUP_HOTSPOT_ToolSide
#undef END_GROUP_ToolSide

#ifndef START_GROUP_Resolution
#define START_GROUP_Resolution(a)
#endif
#ifndef GROUP_HOTSPOT_Resolution
#define GROUP_HOTSPOT_Resolution(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Resolution
#define END_GROUP_Resolution(a)
#endif

START_GROUP_Resolution(Resolution)
	GROUP_HOTSPOT_Resolution(Xsensor, Resolution, 63, 70, 182, 130)
	GROUP_HOTSPOT_Resolution(Ysensor, Resolution, 63, 130, 182, 190)
	GROUP_HOTSPOT_Resolution(Zsensor, Resolution, 63, 190, 182, 250)
	GROUP_HOTSPOT_Resolution(Qsensor, Resolution, 63, 250, 182, 310)
END_GROUP_Resolution(Resolution)

#undef START_GROUP_Resolution
#undef GROUP_HOTSPOT_Resolution
#undef END_GROUP_Resolution

#ifndef START_GROUP_Invert
#define START_GROUP_Invert(a)
#endif
#ifndef GROUP_HOTSPOT_Invert
#define GROUP_HOTSPOT_Invert(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Invert
#define END_GROUP_Invert(a)
#endif

START_GROUP_Invert(Invert)
	GROUP_HOTSPOT_Invert(Xsensor, Invert, 182, 70, 301, 130)
	GROUP_HOTSPOT_Invert(Ysensor, Invert, 182, 130, 301, 190)
	GROUP_HOTSPOT_Invert(Zsensor, Invert, 182, 190, 301, 250)
	GROUP_HOTSPOT_Invert(Qsensor, Invert, 182, 250, 301, 310)
END_GROUP_Invert(Invert)

#undef START_GROUP_Invert
#undef GROUP_HOTSPOT_Invert
#undef END_GROUP_Invert

#ifndef START_GROUP_Correction
#define START_GROUP_Correction(a)
#endif
#ifndef GROUP_HOTSPOT_Correction
#define GROUP_HOTSPOT_Correction(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Correction
#define END_GROUP_Correction(a)
#endif

START_GROUP_Correction(Correction)
	GROUP_HOTSPOT_Correction(Xsensor, Correction, 301, 70, 420, 130)
	GROUP_HOTSPOT_Correction(Ysensor, Correction, 301, 130, 420, 190)
	GROUP_HOTSPOT_Correction(Zsensor, Correction, 301, 190, 420, 250)
	GROUP_HOTSPOT_Correction(Qsensor, Correction, 301, 250, 420, 310)
END_GROUP_Correction(Correction)

#undef START_GROUP_Correction
#undef GROUP_HOTSPOT_Correction
#undef END_GROUP_Correction

#ifndef START_GROUP_SettingToggle
#define START_GROUP_SettingToggle(a)
#endif
#ifndef GROUP_HOTSPOT_SettingToggle
#define GROUP_HOTSPOT_SettingToggle(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_SettingToggle
#define END_GROUP_SettingToggle(a)
#endif

START_GROUP_SettingToggle(SettingToggle)
	GROUP_HOTSPOT_SettingToggle(HighlightXY, SettingToggle, 10, 320, 354, 379)
	GROUP_HOTSPOT_SettingToggle(OffsetZ, SettingToggle, 10, 390, 354, 449)
	GROUP_HOTSPOT_SettingToggle(CncCoordinates, SettingToggle, 10, 460, 354, 519)
END_GROUP_SettingToggle(SettingToggle)

#undef START_GROUP_SettingToggle
#undef GROUP_HOTSPOT_SettingToggle
#undef END_GROUP_SettingToggle

#ifndef START_GROUP_Keyboard
#define START_GROUP_Keyboard(a)
#endif
#ifndef GROUP_HOTSPOT_Keyboard
#define GROUP_HOTSPOT_Keyboard(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_Keyboard
#define END_GROUP_Keyboard(a)
#endif

START_GROUP_Keyboard(Keyboard)
	GROUP_HOTSPOT_Keyboard(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT_Keyboard(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT_Keyboard(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT_Keyboard(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT_Keyboard(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT_Keyboard(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT_Keyboard(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT_Keyboard(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT_Keyboard(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT_Keyboard(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT_Keyboard(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT_Keyboard(Key_q, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT_Keyboard(Key_w, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT_Keyboard(Key_e, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT_Keyboard(Key_r, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT_Keyboard(Key_t, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT_Keyboard(Key_y, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT_Keyboard(Key_u, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT_Keyboard(Key_i, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT_Keyboard(Key_o, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT_Keyboard(Key_p, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT_Keyboard(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT_Keyboard(Key_a, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT_Keyboard(Key_s, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT_Keyboard(Key_d, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT_Keyboard(Key_f, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT_Keyboard(Key_g, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT_Keyboard(Key_h, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT_Keyboard(Key_j, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT_Keyboard(Key_k, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT_Keyboard(Key_l, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT_Keyboard(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT_Keyboard(Key_z, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT_Keyboard(Key_x, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT_Keyboard(Key_c, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT_Keyboard(Key_v, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT_Keyboard(Key_b, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT_Keyboard(Key_n, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT_Keyboard(Key_m, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT_Keyboard(Key_comma, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT_Keyboard(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT_Keyboard(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT_Keyboard(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT_Keyboard(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT_Keyboard(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT_Keyboard(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT_Keyboard(Key_backspace, Keyboard, 710, 290, 769, 349)
	GROUP_HOTSPOT_Keyboard(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT_Keyboard(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT_Keyboard(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT_Keyboard(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT_Keyboard(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT_Keyboard(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT_Keyboard(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT_Keyboard(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT_Keyboard(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT_Keyboard(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT_Keyboard(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT_Keyboard(Key_Q, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT_Keyboard(Key_W, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT_Keyboard(Key_E, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT_Keyboard(Key_R, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT_Keyboard(Key_T, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT_Keyboard(Key_Y, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT_Keyboard(Key_U, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT_Keyboard(Key_I, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT_Keyboard(Key_O, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT_Keyboard(Key_P, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT_Keyboard(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT_Keyboard(Key_A, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT_Keyboard(Key_S, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT_Keyboard(Key_D, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT_Keyboard(Key_F, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT_Keyboard(Key_G, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT_Keyboard(Key_H, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT_Keyboard(Key_J, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT_Keyboard(Key_K, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT_Keyboard(Key_L, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT_Keyboard(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT_Keyboard(Key_Z, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT_Keyboard(Key_X, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT_Keyboard(Key_C, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT_Keyboard(Key_V, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT_Keyboard(Key_B, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT_Keyboard(Key_N, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT_Keyboard(Key_M, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT_Keyboard(Key_comma, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT_Keyboard(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT_Keyboard(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT_Keyboard(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT_Keyboard(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT_Keyboard(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT_Keyboard(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT_Keyboard(Key_backspace, Keyboard, 710, 290, 769, 349)
	GROUP_HOTSPOT_Keyboard(Key_1, Keyboard, 10, 10, 69, 69)
	GROUP_HOTSPOT_Keyboard(Key_2, Keyboard, 80, 10, 139, 69)
	GROUP_HOTSPOT_Keyboard(Key_3, Keyboard, 150, 10, 209, 69)
	GROUP_HOTSPOT_Keyboard(Key_4, Keyboard, 220, 10, 279, 69)
	GROUP_HOTSPOT_Keyboard(Key_5, Keyboard, 290, 10, 349, 69)
	GROUP_HOTSPOT_Keyboard(Key_6, Keyboard, 360, 10, 419, 69)
	GROUP_HOTSPOT_Keyboard(Key_7, Keyboard, 430, 10, 489, 69)
	GROUP_HOTSPOT_Keyboard(Key_8, Keyboard, 500, 10, 559, 69)
	GROUP_HOTSPOT_Keyboard(Key_9, Keyboard, 570, 10, 629, 69)
	GROUP_HOTSPOT_Keyboard(Key_0, Keyboard, 640, 10, 699, 69)
	GROUP_HOTSPOT_Keyboard(Key_minus, Keyboard, 710, 10, 769, 69)
	GROUP_HOTSPOT_Keyboard(Key_tilde, Keyboard, 40, 80, 99, 139)
	GROUP_HOTSPOT_Keyboard(Key_exclamation, Keyboard, 110, 80, 169, 139)
	GROUP_HOTSPOT_Keyboard(Key_at, Keyboard, 180, 80, 239, 139)
	GROUP_HOTSPOT_Keyboard(Key_hash, Keyboard, 250, 80, 309, 139)
	GROUP_HOTSPOT_Keyboard(Key_dollar, Keyboard, 320, 80, 379, 139)
	GROUP_HOTSPOT_Keyboard(Key_percent, Keyboard, 390, 80, 449, 139)
	GROUP_HOTSPOT_Keyboard(Key_hat, Keyboard, 460, 80, 519, 139)
	GROUP_HOTSPOT_Keyboard(Key_star, Keyboard, 530, 80, 589, 139)
	GROUP_HOTSPOT_Keyboard(Key_lparen, Keyboard, 600, 80, 659, 139)
	GROUP_HOTSPOT_Keyboard(Key_rparen, Keyboard, 670, 80, 729, 139)
	GROUP_HOTSPOT_Keyboard(Key_quote, Keyboard, 740, 80, 799, 139)
	GROUP_HOTSPOT_Keyboard(Key_backApostrophe, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT_Keyboard(Key_ampersand, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT_Keyboard(Key_underscore, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT_Keyboard(Key_plus, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT_Keyboard(Key_equal, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT_Keyboard(Key_lbracket, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT_Keyboard(Key_rbracket, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT_Keyboard(Key_lbrace, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT_Keyboard(Key_rbrace, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT_Keyboard(Key_enter, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 220, 69, 279)
	GROUP_HOTSPOT_Keyboard(Key_vbar, Keyboard, 110, 220, 169, 279)
	GROUP_HOTSPOT_Keyboard(Key_backslash, Keyboard, 180, 220, 239, 279)
	GROUP_HOTSPOT_Keyboard(Key_colon, Keyboard, 250, 220, 309, 279)
	GROUP_HOTSPOT_Keyboard(Key_semi, Keyboard, 320, 220, 379, 279)
	GROUP_HOTSPOT_Keyboard(Key_apostrophe, Keyboard, 390, 220, 449, 279)
	GROUP_HOTSPOT_Keyboard(Key_lessthan, Keyboard, 460, 220, 519, 279)
	GROUP_HOTSPOT_Keyboard(Key_greaterthan, Keyboard, 530, 220, 589, 279)
	GROUP_HOTSPOT_Keyboard(Key_question, Keyboard, 600, 220, 659, 279)
	GROUP_HOTSPOT_Keyboard(Key_period, Keyboard, 670, 220, 729, 279)
	GROUP_HOTSPOT_Keyboard(Key_slash, Keyboard, 740, 220, 799, 279)
	GROUP_HOTSPOT_Keyboard(Key_shift, Keyboard, 10, 290, 69, 349)
	GROUP_HOTSPOT_Keyboard(Key_symbols, Keyboard, 110, 290, 169, 349)
	GROUP_HOTSPOT_Keyboard(Key_space, Keyboard, 210, 290, 269, 349)
	GROUP_HOTSPOT_Keyboard(Key_left, Keyboard, 570, 290, 629, 349)
	GROUP_HOTSPOT_Keyboard(Key_right, Keyboard, 640, 290, 699, 349)
	GROUP_HOTSPOT_Keyboard(Key_backspace, Keyboard, 710, 290, 769, 349)
END_GROUP_Keyboard(Keyboard)

#undef START_GROUP_Keyboard
#undef GROUP_HOTSPOT_Keyboard
#undef END_GROUP_Keyboard

#ifndef START_SCREEN_Overlay
#define START_SCREEN_Overlay(a)
#endif
#ifndef IMAGE_ADDRESS_Overlay
#define IMAGE_ADDRESS_Overlay(a)
#endif
#ifndef IMAGE_SIZE_Overlay
#define IMAGE_SIZE_Overlay(a)
#endif
#ifndef IMAGE_WIDTH_Overlay
#define IMAGE_WIDTH_Overlay(a)
#endif
#ifndef IMAGE_HEIGHT_Overlay
#define IMAGE_HEIGHT_Overlay(a)
#endif
#ifndef IMAGE_STRIDE_Overlay
#define IMAGE_STRIDE_Overlay(a)
#endif
#ifndef IMAGE_DEPTH_Overlay
#define IMAGE_DEPTH_Overlay(a)
#endif
#ifndef END_SCREEN_Overlay
#define END_SCREEN_Overlay(a)
#endif

START_SCREEN_Overlay(Coord)
	IMAGE_ADDRESS_Overlay(1228800)
	IMAGE_SIZE_Overlay(14400)
	IMAGE_WIDTH_Overlay(120)
	IMAGE_HEIGHT_Overlay(60)
	IMAGE_STRIDE_Overlay(120)
	IMAGE_DEPTH_Overlay(Color16bpp)
END_SCREEN_Overlay(Coord)

START_SCREEN_Overlay(InchMetricBtn)
	IMAGE_ADDRESS_Overlay(1243200)
	IMAGE_SIZE_Overlay(14400)
	IMAGE_WIDTH_Overlay(120)
	IMAGE_HEIGHT_Overlay(60)
	IMAGE_STRIDE_Overlay(120)
	IMAGE_DEPTH_Overlay(Color16bpp)
END_SCREEN_Overlay(InchMetricBtn)

START_SCREEN_Overlay(SpeedDisplay)
	IMAGE_ADDRESS_Overlay(1257600)
	IMAGE_SIZE_Overlay(144336)
	IMAGE_WIDTH_Overlay(742)
	IMAGE_HEIGHT_Overlay(97)
	IMAGE_STRIDE_Overlay(744)
	IMAGE_DEPTH_Overlay(Color16bpp)
END_SCREEN_Overlay(SpeedDisplay)

START_SCREEN_Overlay(ToolButtons)
	IMAGE_ADDRESS_Overlay(1693776)
	IMAGE_SIZE_Overlay(128400)
	IMAGE_WIDTH_Overlay(600)
	IMAGE_HEIGHT_Overlay(107)
	IMAGE_STRIDE_Overlay(600)
	IMAGE_DEPTH_Overlay(Color16bpp)
END_SCREEN_Overlay(ToolButtons)

START_SCREEN_Overlay(CheckBox)
	IMAGE_ADDRESS_Overlay(2167776)
	IMAGE_SIZE_Overlay(2048)
	IMAGE_WIDTH_Overlay(64)
	IMAGE_HEIGHT_Overlay(32)
	IMAGE_STRIDE_Overlay(64)
	IMAGE_DEPTH_Overlay(Color8bpp)
END_SCREEN_Overlay(CheckBox)

#undef START_SCREEN_Overlay
#undef IMAGE_ADDRESS_Overlay
#undef IMAGE_SIZE_Overlay
#undef IMAGE_WIDTH_Overlay
#undef IMAGE_HEIGHT_Overlay
#undef IMAGE_STRIDE_Overlay
#undef IMAGE_DEPTH_Overlay
#undef END_SCREEN_Overlay

SCREEN_FILE_LENGTH(3046784)

#undef START_SCREEN
#undef IMAGE_ADDRESS
#undef IMAGE_SIZE
#undef IMAGE_WIDTH
#undef IMAGE_HEIGHT
#undef IMAGE_STRIDE
#undef IMAGE_DEPTH
#undef END_SCREEN
#undef START_HOTSPOTS
#undef DEFINE_HOTSPOT
#undef END_HOTSPOTS
#undef HOTSPOT_COUNT
#undef START_LOCATIONS
#undef DEFINE_LOCATION
#undef END_LOCATIONS
#undef START_AREAS
#undef DEFINE_AREA
#undef END_AREAS
#undef START_GROUP
#undef GROUP_HOTSPOT
#undef END_GROUP
#undef START_VALUES
#undef DEFINE_VALUE
#undef END_VALUES
#undef START_STR_VALUES
#undef DEFINE_STR_VALUE
#undef END_STR_VALUES
#undef DEFINE_COLOR
#undef SCREEN_FILE_LENGTH
