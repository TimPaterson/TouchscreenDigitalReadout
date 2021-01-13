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
#define START_VALUES(a)
#endif
#ifndef DEFINE_VALUE
#define DEFINE_VALUE(a,b)
#endif
#ifndef END_VALUES
#define END_VALUES(a)
#endif
#ifndef START_STR_VALUES
#define START_STR_VALUES(a)
#endif
#ifndef DEFINE_STR_VALUE
#define DEFINE_STR_VALUE(a,b)
#endif
#ifndef END_STR_VALUES
#define END_STR_VALUES(a)
#endif
#ifndef DEFINE_COLOR
#define DEFINE_COLOR(a,b)
#endif
#ifndef SCREEN_FILE_LENGTH
#define SCREEN_FILE_LENGTH(a)
#endif

DEFINE_COLOR(ScreenBackColor, 0xB0D8E8)
DEFINE_COLOR(AxisForeColor, 0x000000)
DEFINE_COLOR(AxisBackColor, 0xFFFF00)
DEFINE_COLOR(CalcBackColor, 0xFFFFFF)
DEFINE_COLOR(MemColorOdd, 0xD0C0D0)
DEFINE_COLOR(MemColorEven, 0x98FC98)
DEFINE_COLOR(BtnBackground, 0x606060)
DEFINE_COLOR(ToggleOn, 0xFFFF00)
DEFINE_COLOR(ToolColor, 0xC00000)
DEFINE_COLOR(NoToolColor, 0x88D0F8)

START_SCREEN(MainScreen)
	IMAGE_ADDRESS(0)
	IMAGE_SIZE(1228800)
	IMAGE_WIDTH(1024)
	IMAGE_HEIGHT(600)
	IMAGE_DEPTH(Color16bpp)
END_SCREEN(MainScreen)

START_HOTSPOTS(MainScreen)
	DEFINE_HOTSPOT(Xdisplay, Axis, 0, 12, 409, 122)
	DEFINE_HOTSPOT(Xbutton, , 435, 19, 519, 114)
	DEFINE_HOTSPOT(Ydisplay, Axis, 0, 148, 409, 258)
	DEFINE_HOTSPOT(Ybutton, , 435, 155, 519, 250)
	DEFINE_HOTSPOT(Zdisplay, Axis, 0, 284, 409, 394)
	DEFINE_HOTSPOT(Zbutton, , 435, 291, 519, 386)
	DEFINE_HOTSPOT(Undo, , 580, 12, 869, 95)
	DEFINE_HOTSPOT(CalcDisplay, , 580, 119, 869, 178)
	DEFINE_HOTSPOT(InchMetric, , 880, 517, 939, 576)
	DEFINE_HOTSPOT(AbsInc, , 950, 517, 1009, 576)
	DEFINE_HOTSPOT(Key_backSpace, Edit, 590, 237, 649, 296)
	DEFINE_HOTSPOT(Key_clear, Edit, 660, 237, 719, 296)
	DEFINE_HOTSPOT(Key_pi, Edit, 730, 237, 789, 296)
	DEFINE_HOTSPOT(Key_plus, Operator, 800, 237, 859, 296)
	DEFINE_HOTSPOT(Mem1, Memory, 870, 237, 1019, 296)
	DEFINE_HOTSPOT(Key_7, Digit, 590, 307, 649, 366)
	DEFINE_HOTSPOT(Key_8, Digit, 660, 307, 719, 366)
	DEFINE_HOTSPOT(Key_9, Digit, 730, 307, 789, 366)
	DEFINE_HOTSPOT(Key_minus, Operator, 800, 307, 859, 366)
	DEFINE_HOTSPOT(Mem2, Memory, 870, 307, 1019, 366)
	DEFINE_HOTSPOT(Key_4, Digit, 590, 377, 649, 436)
	DEFINE_HOTSPOT(Key_5, Digit, 660, 377, 719, 436)
	DEFINE_HOTSPOT(Key_6, Digit, 730, 377, 789, 436)
	DEFINE_HOTSPOT(Key_mult, Operator, 800, 377, 859, 436)
	DEFINE_HOTSPOT(Mem3, Memory, 870, 377, 1019, 436)
	DEFINE_HOTSPOT(Key_1, Digit, 590, 447, 649, 506)
	DEFINE_HOTSPOT(Key_2, Digit, 660, 447, 719, 506)
	DEFINE_HOTSPOT(Key_3, Digit, 730, 447, 789, 506)
	DEFINE_HOTSPOT(Key_divide, Operator, 800, 447, 859, 506)
	DEFINE_HOTSPOT(Mem4, Memory, 870, 447, 1019, 506)
	DEFINE_HOTSPOT(Key_sign, Digit, 590, 517, 649, 576)
	DEFINE_HOTSPOT(Key_0, Digit, 660, 517, 719, 576)
	DEFINE_HOTSPOT(Key_decimal, Digit, 730, 517, 789, 576)
	DEFINE_HOTSPOT(Key_equal, Operator, 800, 517, 859, 576)
	DEFINE_HOTSPOT(ToolNumber, ToolInfo, 10, 405, 79, 464)
	DEFINE_HOTSPOT(ToolDiameter, ToolInfo, 80, 405, 149, 464)
	DEFINE_HOTSPOT(ToolFlutes, ToolInfo, 150, 405, 204, 464)
	DEFINE_HOTSPOT(ToolLength, ToolInfo, 205, 405, 289, 464)
	DEFINE_HOTSPOT(ToolMenu, , 510, 405, 569, 464)
	DEFINE_HOTSPOT(ToolLeft, ToolSide, 10, 505, 69, 564)
	DEFINE_HOTSPOT(ToolBack, ToolSide, 70, 475, 129, 534)
	DEFINE_HOTSPOT(ToolFront, ToolSide, 70, 535, 129, 594)
	DEFINE_HOTSPOT(ToolRight, ToolSide, 130, 505, 189, 564)
	DEFINE_HOTSPOT(ToolSfm, ToolInfo, 200, 475, 379, 570)
	DEFINE_HOTSPOT(ToolChipLoad, ToolInfo, 390, 475, 569, 570)
END_HOTSPOTS(MainScreen)

HOTSPOT_COUNT(MainScreen, 45)

START_LOCATIONS(MainScreen)
	DEFINE_LOCATION(Undo3, 580, 25)
	DEFINE_LOCATION(Undo2, 580, 53)
	DEFINE_LOCATION(Undo1, 580, 81)
	DEFINE_LOCATION(ToolLeft, 25, 535)
	DEFINE_LOCATION(ToolRight, 175, 535)
	DEFINE_LOCATION(ToolBack, 100, 490)
	DEFINE_LOCATION(ToolFront, 100, 580)
END_LOCATIONS(MainScreen)

START_AREAS(MainScreen)
	DEFINE_AREA(Xdisplay, 0, 12, 410, 111)
	DEFINE_AREA(Ydisplay, 0, 148, 410, 111)
	DEFINE_AREA(Zdisplay, 0, 284, 410, 111)
	DEFINE_AREA(Undo, 580, 12, 290, 84)
	DEFINE_AREA(CalcDisplay, 580, 119, 290, 60)
	DEFINE_AREA(CalcText, 580, 189, 290, 28)
	DEFINE_AREA(InchMetric, 880, 517, 60, 60)
	DEFINE_AREA(AbsInc, 950, 517, 60, 60)
	DEFINE_AREA(Mem1, 880, 252, 140, 30)
	DEFINE_AREA(Mem2, 880, 322, 140, 30)
	DEFINE_AREA(Mem3, 880, 392, 140, 30)
	DEFINE_AREA(Mem4, 880, 462, 140, 30)
	DEFINE_AREA(ToolNumber, 22, 437, 57, 28)
	DEFINE_AREA(ToolDiameter, 85, 437, 64, 28)
	DEFINE_AREA(ToolFlutes, 159, 437, 45, 28)
	DEFINE_AREA(ToolLength, 211, 437, 78, 28)
	DEFINE_AREA(ToolDesc, 295, 437, 204, 28)
	DEFINE_AREA(SpeedDisplay, 200, 475, 371, 97)
	DEFINE_AREA(Sfm, 305, 509, 74, 28)
	DEFINE_AREA(ChipLoad, 488, 509, 81, 28)
	DEFINE_AREA(Rpm, 305, 541, 74, 28)
	DEFINE_AREA(FeedRate, 495, 541, 74, 28)
END_AREAS(MainScreen)

START_VALUES(MainScreen)
	DEFINE_VALUE(ToolImageRadius, 15)
END_VALUES(MainScreen)

START_STR_VALUES(MainScreen)
END_STR_VALUES(MainScreen)

START_HOTSPOTS(InchSpeedDisplay)
	DEFINE_HOTSPOT(ToolSfm, ToolInfo, 0, 0, 179, 95)
	DEFINE_HOTSPOT(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
END_HOTSPOTS(InchSpeedDisplay)

HOTSPOT_COUNT(InchSpeedDisplay, 2)

START_AREAS(InchSpeedDisplay)
	DEFINE_AREA(Sfm, 105, 34, 74, 28)
	DEFINE_AREA(ChipLoad, 288, 34, 81, 28)
	DEFINE_AREA(Rpm, 105, 66, 74, 28)
	DEFINE_AREA(FeedRate, 295, 66, 74, 28)
END_AREAS(InchSpeedDisplay)

START_HOTSPOTS(MetricSpeedDisplay)
	DEFINE_HOTSPOT(ToolSfm, ToolInfo, 0, 0, 179, 95)
	DEFINE_HOTSPOT(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
END_HOTSPOTS(MetricSpeedDisplay)

HOTSPOT_COUNT(MetricSpeedDisplay, 2)

START_AREAS(MetricSpeedDisplay)
	DEFINE_AREA(Sfm, 105, 34, 74, 28)
	DEFINE_AREA(ChipLoad, 288, 34, 81, 28)
	DEFINE_AREA(Rpm, 105, 66, 74, 28)
	DEFINE_AREA(FeedRate, 295, 66, 74, 28)
END_AREAS(MetricSpeedDisplay)

START_SCREEN(KeyLower)
	IMAGE_ADDRESS(1402448)
	IMAGE_SIZE(1024)
	IMAGE_WIDTH(32)
	IMAGE_HEIGHT(32)
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
	DEFINE_HOTSPOT(Key_a, Keyboard, 70, 150, 129, 209)
	DEFINE_HOTSPOT(Key_s, Keyboard, 140, 150, 199, 209)
	DEFINE_HOTSPOT(Key_d, Keyboard, 210, 150, 269, 209)
	DEFINE_HOTSPOT(Key_f, Keyboard, 280, 150, 339, 209)
	DEFINE_HOTSPOT(Key_g, Keyboard, 350, 150, 409, 209)
	DEFINE_HOTSPOT(Key_h, Keyboard, 420, 150, 479, 209)
	DEFINE_HOTSPOT(Key_j, Keyboard, 490, 150, 549, 209)
	DEFINE_HOTSPOT(Key_k, Keyboard, 560, 150, 619, 209)
	DEFINE_HOTSPOT(Key_l, Keyboard, 630, 150, 689, 209)
	DEFINE_HOTSPOT(Key_semi, Keyboard, 700, 150, 759, 209)
	DEFINE_HOTSPOT(Key_z, Keyboard, 100, 220, 159, 279)
	DEFINE_HOTSPOT(Key_x, Keyboard, 170, 220, 229, 279)
	DEFINE_HOTSPOT(Key_c, Keyboard, 240, 220, 299, 279)
	DEFINE_HOTSPOT(Key_v, Keyboard, 310, 220, 369, 279)
	DEFINE_HOTSPOT(Key_b, Keyboard, 380, 220, 439, 279)
	DEFINE_HOTSPOT(Key_n, Keyboard, 450, 220, 509, 279)
	DEFINE_HOTSPOT(Key_m, Keyboard, 520, 220, 579, 279)
	DEFINE_HOTSPOT(Key_comma, Keyboard, 590, 220, 649, 279)
	DEFINE_HOTSPOT(Key_period, Keyboard, 660, 220, 719, 279)
END_HOTSPOTS(KeyLower)

HOTSPOT_COUNT(KeyLower, 39)

START_SCREEN(KeyUpper)
	IMAGE_ADDRESS(1403472)
	IMAGE_SIZE(1024)
	IMAGE_WIDTH(32)
	IMAGE_HEIGHT(32)
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
	DEFINE_HOTSPOT(Key_A, Keyboard, 70, 150, 129, 209)
	DEFINE_HOTSPOT(Key_S, Keyboard, 140, 150, 199, 209)
	DEFINE_HOTSPOT(Key_D, Keyboard, 210, 150, 269, 209)
	DEFINE_HOTSPOT(Key_F, Keyboard, 280, 150, 339, 209)
	DEFINE_HOTSPOT(Key_G, Keyboard, 350, 150, 409, 209)
	DEFINE_HOTSPOT(Key_H, Keyboard, 420, 150, 479, 209)
	DEFINE_HOTSPOT(Key_J, Keyboard, 490, 150, 549, 209)
	DEFINE_HOTSPOT(Key_K, Keyboard, 560, 150, 619, 209)
	DEFINE_HOTSPOT(Key_L, Keyboard, 630, 150, 689, 209)
	DEFINE_HOTSPOT(Key_semi, Keyboard, 700, 150, 759, 209)
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
	DEFINE_HOTSPOT(Key_Space, Keyboard, 220, 290, 279, 349)
END_HOTSPOTS(KeyUpper)

HOTSPOT_COUNT(KeyUpper, 41)

START_GROUP(Axis)
	GROUP_HOTSPOT(Xdisplay, Axis, 0, 12, 409, 122)
	GROUP_HOTSPOT(Ydisplay, Axis, 0, 148, 409, 258)
	GROUP_HOTSPOT(Zdisplay, Axis, 0, 284, 409, 394)
END_GROUP(Axis)

START_GROUP()
	GROUP_HOTSPOT(Xbutton, , 435, 19, 519, 114)
	GROUP_HOTSPOT(Ybutton, , 435, 155, 519, 250)
	GROUP_HOTSPOT(Zbutton, , 435, 291, 519, 386)
	GROUP_HOTSPOT(Undo, , 580, 12, 869, 95)
	GROUP_HOTSPOT(CalcDisplay, , 580, 119, 869, 178)
	GROUP_HOTSPOT(InchMetric, , 880, 517, 939, 576)
	GROUP_HOTSPOT(AbsInc, , 950, 517, 1009, 576)
	GROUP_HOTSPOT(ToolMenu, , 510, 405, 569, 464)
END_GROUP()

START_GROUP(Edit)
	GROUP_HOTSPOT(Key_backSpace, Edit, 590, 237, 649, 296)
	GROUP_HOTSPOT(Key_clear, Edit, 660, 237, 719, 296)
	GROUP_HOTSPOT(Key_pi, Edit, 730, 237, 789, 296)
END_GROUP(Edit)

START_GROUP(Operator)
	GROUP_HOTSPOT(Key_plus, Operator, 800, 237, 859, 296)
	GROUP_HOTSPOT(Key_minus, Operator, 800, 307, 859, 366)
	GROUP_HOTSPOT(Key_mult, Operator, 800, 377, 859, 436)
	GROUP_HOTSPOT(Key_divide, Operator, 800, 447, 859, 506)
	GROUP_HOTSPOT(Key_equal, Operator, 800, 517, 859, 576)
END_GROUP(Operator)

START_GROUP(Memory)
	GROUP_HOTSPOT(Mem1, Memory, 870, 237, 1019, 296)
	GROUP_HOTSPOT(Mem2, Memory, 870, 307, 1019, 366)
	GROUP_HOTSPOT(Mem3, Memory, 870, 377, 1019, 436)
	GROUP_HOTSPOT(Mem4, Memory, 870, 447, 1019, 506)
END_GROUP(Memory)

START_GROUP(Digit)
	GROUP_HOTSPOT(Key_7, Digit, 590, 307, 649, 366)
	GROUP_HOTSPOT(Key_8, Digit, 660, 307, 719, 366)
	GROUP_HOTSPOT(Key_9, Digit, 730, 307, 789, 366)
	GROUP_HOTSPOT(Key_4, Digit, 590, 377, 649, 436)
	GROUP_HOTSPOT(Key_5, Digit, 660, 377, 719, 436)
	GROUP_HOTSPOT(Key_6, Digit, 730, 377, 789, 436)
	GROUP_HOTSPOT(Key_1, Digit, 590, 447, 649, 506)
	GROUP_HOTSPOT(Key_2, Digit, 660, 447, 719, 506)
	GROUP_HOTSPOT(Key_3, Digit, 730, 447, 789, 506)
	GROUP_HOTSPOT(Key_sign, Digit, 590, 517, 649, 576)
	GROUP_HOTSPOT(Key_0, Digit, 660, 517, 719, 576)
	GROUP_HOTSPOT(Key_decimal, Digit, 730, 517, 789, 576)
END_GROUP(Digit)

START_GROUP(ToolInfo)
	GROUP_HOTSPOT(ToolNumber, ToolInfo, 10, 405, 79, 464)
	GROUP_HOTSPOT(ToolDiameter, ToolInfo, 80, 405, 149, 464)
	GROUP_HOTSPOT(ToolFlutes, ToolInfo, 150, 405, 204, 464)
	GROUP_HOTSPOT(ToolLength, ToolInfo, 205, 405, 289, 464)
	GROUP_HOTSPOT(ToolSfm, ToolInfo, 200, 475, 379, 570)
	GROUP_HOTSPOT(ToolChipLoad, ToolInfo, 390, 475, 569, 570)
	GROUP_HOTSPOT(ToolSfm, ToolInfo, 0, 0, 179, 95)
	GROUP_HOTSPOT(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
	GROUP_HOTSPOT(ToolSfm, ToolInfo, 0, 0, 179, 95)
	GROUP_HOTSPOT(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
END_GROUP(ToolInfo)

START_GROUP(ToolSide)
	GROUP_HOTSPOT(ToolLeft, ToolSide, 10, 505, 69, 564)
	GROUP_HOTSPOT(ToolBack, ToolSide, 70, 475, 129, 534)
	GROUP_HOTSPOT(ToolFront, ToolSide, 70, 535, 129, 594)
	GROUP_HOTSPOT(ToolRight, ToolSide, 130, 505, 189, 564)
END_GROUP(ToolSide)

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
	GROUP_HOTSPOT(Key_a, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT(Key_s, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT(Key_d, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT(Key_f, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT(Key_g, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT(Key_h, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT(Key_j, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT(Key_k, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT(Key_l, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT(Key_semi, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT(Key_z, Keyboard, 100, 220, 159, 279)
	GROUP_HOTSPOT(Key_x, Keyboard, 170, 220, 229, 279)
	GROUP_HOTSPOT(Key_c, Keyboard, 240, 220, 299, 279)
	GROUP_HOTSPOT(Key_v, Keyboard, 310, 220, 369, 279)
	GROUP_HOTSPOT(Key_b, Keyboard, 380, 220, 439, 279)
	GROUP_HOTSPOT(Key_n, Keyboard, 450, 220, 509, 279)
	GROUP_HOTSPOT(Key_m, Keyboard, 520, 220, 579, 279)
	GROUP_HOTSPOT(Key_comma, Keyboard, 590, 220, 649, 279)
	GROUP_HOTSPOT(Key_period, Keyboard, 660, 220, 719, 279)
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
	GROUP_HOTSPOT(Key_A, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT(Key_S, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT(Key_D, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT(Key_F, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT(Key_G, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT(Key_H, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT(Key_J, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT(Key_K, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT(Key_L, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT(Key_semi, Keyboard, 700, 150, 759, 209)
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
	GROUP_HOTSPOT(Key_Space, Keyboard, 220, 290, 279, 349)
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
	GROUP_HOTSPOT_Axis(Xdisplay, Axis, 0, 12, 409, 122)
	GROUP_HOTSPOT_Axis(Ydisplay, Axis, 0, 148, 409, 258)
	GROUP_HOTSPOT_Axis(Zdisplay, Axis, 0, 284, 409, 394)
END_GROUP_Axis(Axis)

#undef START_GROUP_Axis
#undef GROUP_HOTSPOT_Axis
#undef END_GROUP_Axis

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
	GROUP_HOTSPOT_(Xbutton, , 435, 19, 519, 114)
	GROUP_HOTSPOT_(Ybutton, , 435, 155, 519, 250)
	GROUP_HOTSPOT_(Zbutton, , 435, 291, 519, 386)
	GROUP_HOTSPOT_(Undo, , 580, 12, 869, 95)
	GROUP_HOTSPOT_(CalcDisplay, , 580, 119, 869, 178)
	GROUP_HOTSPOT_(InchMetric, , 880, 517, 939, 576)
	GROUP_HOTSPOT_(AbsInc, , 950, 517, 1009, 576)
	GROUP_HOTSPOT_(ToolMenu, , 510, 405, 569, 464)
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
	GROUP_HOTSPOT_Edit(Key_backSpace, Edit, 590, 237, 649, 296)
	GROUP_HOTSPOT_Edit(Key_clear, Edit, 660, 237, 719, 296)
	GROUP_HOTSPOT_Edit(Key_pi, Edit, 730, 237, 789, 296)
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
	GROUP_HOTSPOT_Operator(Key_plus, Operator, 800, 237, 859, 296)
	GROUP_HOTSPOT_Operator(Key_minus, Operator, 800, 307, 859, 366)
	GROUP_HOTSPOT_Operator(Key_mult, Operator, 800, 377, 859, 436)
	GROUP_HOTSPOT_Operator(Key_divide, Operator, 800, 447, 859, 506)
	GROUP_HOTSPOT_Operator(Key_equal, Operator, 800, 517, 859, 576)
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
	GROUP_HOTSPOT_Memory(Mem1, Memory, 870, 237, 1019, 296)
	GROUP_HOTSPOT_Memory(Mem2, Memory, 870, 307, 1019, 366)
	GROUP_HOTSPOT_Memory(Mem3, Memory, 870, 377, 1019, 436)
	GROUP_HOTSPOT_Memory(Mem4, Memory, 870, 447, 1019, 506)
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
	GROUP_HOTSPOT_Digit(Key_7, Digit, 590, 307, 649, 366)
	GROUP_HOTSPOT_Digit(Key_8, Digit, 660, 307, 719, 366)
	GROUP_HOTSPOT_Digit(Key_9, Digit, 730, 307, 789, 366)
	GROUP_HOTSPOT_Digit(Key_4, Digit, 590, 377, 649, 436)
	GROUP_HOTSPOT_Digit(Key_5, Digit, 660, 377, 719, 436)
	GROUP_HOTSPOT_Digit(Key_6, Digit, 730, 377, 789, 436)
	GROUP_HOTSPOT_Digit(Key_1, Digit, 590, 447, 649, 506)
	GROUP_HOTSPOT_Digit(Key_2, Digit, 660, 447, 719, 506)
	GROUP_HOTSPOT_Digit(Key_3, Digit, 730, 447, 789, 506)
	GROUP_HOTSPOT_Digit(Key_sign, Digit, 590, 517, 649, 576)
	GROUP_HOTSPOT_Digit(Key_0, Digit, 660, 517, 719, 576)
	GROUP_HOTSPOT_Digit(Key_decimal, Digit, 730, 517, 789, 576)
END_GROUP_Digit(Digit)

#undef START_GROUP_Digit
#undef GROUP_HOTSPOT_Digit
#undef END_GROUP_Digit

#ifndef START_GROUP_ToolInfo
#define START_GROUP_ToolInfo(a)
#endif
#ifndef GROUP_HOTSPOT_ToolInfo
#define GROUP_HOTSPOT_ToolInfo(a,b,c,d,e,f)
#endif
#ifndef END_GROUP_ToolInfo
#define END_GROUP_ToolInfo(a)
#endif

START_GROUP_ToolInfo(ToolInfo)
	GROUP_HOTSPOT_ToolInfo(ToolNumber, ToolInfo, 10, 405, 79, 464)
	GROUP_HOTSPOT_ToolInfo(ToolDiameter, ToolInfo, 80, 405, 149, 464)
	GROUP_HOTSPOT_ToolInfo(ToolFlutes, ToolInfo, 150, 405, 204, 464)
	GROUP_HOTSPOT_ToolInfo(ToolLength, ToolInfo, 205, 405, 289, 464)
	GROUP_HOTSPOT_ToolInfo(ToolSfm, ToolInfo, 200, 475, 379, 570)
	GROUP_HOTSPOT_ToolInfo(ToolChipLoad, ToolInfo, 390, 475, 569, 570)
	GROUP_HOTSPOT_ToolInfo(ToolSfm, ToolInfo, 0, 0, 179, 95)
	GROUP_HOTSPOT_ToolInfo(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
	GROUP_HOTSPOT_ToolInfo(ToolSfm, ToolInfo, 0, 0, 179, 95)
	GROUP_HOTSPOT_ToolInfo(ToolChipLoad, ToolInfo, 190, 0, 369, 95)
END_GROUP_ToolInfo(ToolInfo)

#undef START_GROUP_ToolInfo
#undef GROUP_HOTSPOT_ToolInfo
#undef END_GROUP_ToolInfo

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
	GROUP_HOTSPOT_Keyboard(Key_a, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT_Keyboard(Key_s, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT_Keyboard(Key_d, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT_Keyboard(Key_f, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT_Keyboard(Key_g, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT_Keyboard(Key_h, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT_Keyboard(Key_j, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT_Keyboard(Key_k, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT_Keyboard(Key_l, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT_Keyboard(Key_semi, Keyboard, 700, 150, 759, 209)
	GROUP_HOTSPOT_Keyboard(Key_z, Keyboard, 100, 220, 159, 279)
	GROUP_HOTSPOT_Keyboard(Key_x, Keyboard, 170, 220, 229, 279)
	GROUP_HOTSPOT_Keyboard(Key_c, Keyboard, 240, 220, 299, 279)
	GROUP_HOTSPOT_Keyboard(Key_v, Keyboard, 310, 220, 369, 279)
	GROUP_HOTSPOT_Keyboard(Key_b, Keyboard, 380, 220, 439, 279)
	GROUP_HOTSPOT_Keyboard(Key_n, Keyboard, 450, 220, 509, 279)
	GROUP_HOTSPOT_Keyboard(Key_m, Keyboard, 520, 220, 579, 279)
	GROUP_HOTSPOT_Keyboard(Key_comma, Keyboard, 590, 220, 649, 279)
	GROUP_HOTSPOT_Keyboard(Key_period, Keyboard, 660, 220, 719, 279)
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
	GROUP_HOTSPOT_Keyboard(Key_A, Keyboard, 70, 150, 129, 209)
	GROUP_HOTSPOT_Keyboard(Key_S, Keyboard, 140, 150, 199, 209)
	GROUP_HOTSPOT_Keyboard(Key_D, Keyboard, 210, 150, 269, 209)
	GROUP_HOTSPOT_Keyboard(Key_F, Keyboard, 280, 150, 339, 209)
	GROUP_HOTSPOT_Keyboard(Key_G, Keyboard, 350, 150, 409, 209)
	GROUP_HOTSPOT_Keyboard(Key_H, Keyboard, 420, 150, 479, 209)
	GROUP_HOTSPOT_Keyboard(Key_J, Keyboard, 490, 150, 549, 209)
	GROUP_HOTSPOT_Keyboard(Key_K, Keyboard, 560, 150, 619, 209)
	GROUP_HOTSPOT_Keyboard(Key_L, Keyboard, 630, 150, 689, 209)
	GROUP_HOTSPOT_Keyboard(Key_semi, Keyboard, 700, 150, 759, 209)
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
	GROUP_HOTSPOT_Keyboard(Key_Space, Keyboard, 220, 290, 279, 349)
END_GROUP_Keyboard(Keyboard)

#undef START_GROUP_Keyboard
#undef GROUP_HOTSPOT_Keyboard
#undef END_GROUP_Keyboard

#ifndef START_SCREEN_Key
#define START_SCREEN_Key(a)
#endif
#ifndef IMAGE_ADDRESS_Key
#define IMAGE_ADDRESS_Key(a)
#endif
#ifndef IMAGE_SIZE_Key
#define IMAGE_SIZE_Key(a)
#endif
#ifndef IMAGE_WIDTH_Key
#define IMAGE_WIDTH_Key(a)
#endif
#ifndef IMAGE_HEIGHT_Key
#define IMAGE_HEIGHT_Key(a)
#endif
#ifndef IMAGE_DEPTH_Key
#define IMAGE_DEPTH_Key(a)
#endif
#ifndef END_SCREEN_Key
#define END_SCREEN_Key(a)
#endif

START_SCREEN_Key(InchSpeedDisplay)
	IMAGE_ADDRESS_Key(1228800)
	IMAGE_SIZE_Key(72168)
	IMAGE_WIDTH_Key(372)
	IMAGE_HEIGHT_Key(97)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(InchSpeedDisplay)

START_SCREEN_Key(MetricSpeedDisplay)
	IMAGE_ADDRESS_Key(1300968)
	IMAGE_SIZE_Key(72168)
	IMAGE_WIDTH_Key(372)
	IMAGE_HEIGHT_Key(97)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(MetricSpeedDisplay)

START_SCREEN_Key(AbsCoord)
	IMAGE_ADDRESS_Key(1373136)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(AbsCoord)

START_SCREEN_Key(IncCoord)
	IMAGE_ADDRESS_Key(1380336)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(IncCoord)

START_SCREEN_Key(Inch)
	IMAGE_ADDRESS_Key(1387536)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(Inch)

START_SCREEN_Key(Metric)
	IMAGE_ADDRESS_Key(1394736)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(Metric)

START_SCREEN_Key(Pattern16)
	IMAGE_ADDRESS_Key(1401936)
	IMAGE_SIZE_Key(512)
	IMAGE_WIDTH_Key(16)
	IMAGE_HEIGHT_Key(16)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(Pattern16)

#undef START_SCREEN_Key
#undef IMAGE_ADDRESS_Key
#undef IMAGE_SIZE_Key
#undef IMAGE_WIDTH_Key
#undef IMAGE_HEIGHT_Key
#undef IMAGE_DEPTH_Key
#undef END_SCREEN_Key

SCREEN_FILE_LENGTH(1404496)

#undef START_SCREEN
#undef IMAGE_ADDRESS
#undef IMAGE_SIZE
#undef IMAGE_WIDTH
#undef IMAGE_HEIGHT
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
