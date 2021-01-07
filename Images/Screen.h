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
#ifndef DEFINE_COLOR
#define DEFINE_COLOR(a,b)
#endif
#ifndef SCREEN_FILE_LENGTH
#define SCREEN_FILE_LENGTH(a)
#endif

DEFINE_COLOR(AxisBackColor, 0xFFFF00)
DEFINE_COLOR(CalcBackColor, 0xFFFFFF)
DEFINE_COLOR(MemColorOdd, 0xD0C0D0)
DEFINE_COLOR(MemColorEven, 0x98FC98)

START_SCREEN(MainScreen)
	IMAGE_ADDRESS(0)
	IMAGE_SIZE(1228800)
	IMAGE_WIDTH(1024)
	IMAGE_HEIGHT(600)
	IMAGE_DEPTH(Color16bpp)
END_SCREEN(MainScreen)

START_HOTSPOTS(MainScreen)
	DEFINE_HOTSPOT(Xdisplay, Axis, 0, 15, 409, 125)
	DEFINE_HOTSPOT(Xbutton, , 435, 22, 519, 117)
	DEFINE_HOTSPOT(Ydisplay, Axis, 0, 156, 409, 266)
	DEFINE_HOTSPOT(Ybutton, , 435, 163, 519, 258)
	DEFINE_HOTSPOT(Zdisplay, Axis, 0, 297, 409, 407)
	DEFINE_HOTSPOT(Zbutton, , 435, 304, 519, 399)
	DEFINE_HOTSPOT(InchMetric, , 480, 507, 539, 566)
	DEFINE_HOTSPOT(AbsInc, , 400, 507, 459, 566)
	DEFINE_HOTSPOT(Undo, , 560, 15, 849, 98)
	DEFINE_HOTSPOT(CalcDisplay, , 560, 109, 849, 168)
	DEFINE_HOTSPOT(Key_backSpace, Edit, 570, 227, 629, 286)
	DEFINE_HOTSPOT(Key_clear, Edit, 640, 227, 699, 286)
	DEFINE_HOTSPOT(Key_pi, Edit, 710, 227, 769, 286)
	DEFINE_HOTSPOT(Key_plus, Operator, 780, 227, 839, 286)
	DEFINE_HOTSPOT(Mem1, Memory, 850, 227, 999, 286)
	DEFINE_HOTSPOT(Key_7, Digit, 570, 297, 629, 356)
	DEFINE_HOTSPOT(Key_8, Digit, 640, 297, 699, 356)
	DEFINE_HOTSPOT(Key_9, Digit, 710, 297, 769, 356)
	DEFINE_HOTSPOT(Key_minus, Operator, 780, 297, 839, 356)
	DEFINE_HOTSPOT(Mem2, Memory, 850, 297, 999, 356)
	DEFINE_HOTSPOT(Key_4, Digit, 570, 367, 629, 426)
	DEFINE_HOTSPOT(Key_5, Digit, 640, 367, 699, 426)
	DEFINE_HOTSPOT(Key_6, Digit, 710, 367, 769, 426)
	DEFINE_HOTSPOT(Key_mult, Operator, 780, 367, 839, 426)
	DEFINE_HOTSPOT(Mem3, Memory, 850, 367, 999, 426)
	DEFINE_HOTSPOT(Key_1, Digit, 570, 437, 629, 496)
	DEFINE_HOTSPOT(Key_2, Digit, 640, 437, 699, 496)
	DEFINE_HOTSPOT(Key_3, Digit, 710, 437, 769, 496)
	DEFINE_HOTSPOT(Key_divide, Operator, 780, 437, 839, 496)
	DEFINE_HOTSPOT(Mem4, Memory, 850, 437, 999, 496)
	DEFINE_HOTSPOT(Key_sign, Digit, 570, 507, 629, 566)
	DEFINE_HOTSPOT(Key_0, Digit, 640, 507, 699, 566)
	DEFINE_HOTSPOT(Key_decimal, Digit, 710, 507, 769, 566)
	DEFINE_HOTSPOT(Key_equal, Operator, 780, 507, 839, 566)
	DEFINE_HOTSPOT(Mem5, Memory, 850, 507, 999, 566)
END_HOTSPOTS(MainScreen)

HOTSPOT_COUNT(MainScreen, 35)

START_LOCATIONS(MainScreen)
	DEFINE_LOCATION(Xaxis, 0, 15)
	DEFINE_LOCATION(Yaxis, 0, 156)
	DEFINE_LOCATION(Zaxis, 0, 297)
	DEFINE_LOCATION(Undo3, 560, 15)
	DEFINE_LOCATION(Undo2, 560, 43)
	DEFINE_LOCATION(Undo1, 560, 71)
END_LOCATIONS(MainScreen)

START_AREAS(MainScreen)
	DEFINE_AREA(Xdisplay, 0, 15, 410, 111)
	DEFINE_AREA(Ydisplay, 0, 156, 410, 111)
	DEFINE_AREA(Zdisplay, 0, 297, 410, 111)
	DEFINE_AREA(InchMetric, 480, 507, 60, 60)
	DEFINE_AREA(AbsInc, 400, 507, 60, 60)
	DEFINE_AREA(Undo, 560, 15, 290, 84)
	DEFINE_AREA(CalcDisplay, 560, 109, 290, 60)
	DEFINE_AREA(CalcText, 560, 179, 290, 28)
	DEFINE_AREA(Mem1, 860, 242, 140, 30)
	DEFINE_AREA(Mem2, 860, 312, 140, 30)
	DEFINE_AREA(Mem3, 860, 382, 140, 30)
	DEFINE_AREA(Mem4, 860, 452, 140, 30)
	DEFINE_AREA(Mem5, 860, 522, 140, 30)
END_AREAS(MainScreen)

START_SCREEN(KeyLower)
	IMAGE_ADDRESS(1258112)
	IMAGE_SIZE(306720)
	IMAGE_WIDTH(852)
	IMAGE_HEIGHT(360)
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
	IMAGE_ADDRESS(1564832)
	IMAGE_SIZE(306720)
	IMAGE_WIDTH(852)
	IMAGE_HEIGHT(360)
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
	GROUP_HOTSPOT(Xdisplay, Axis, 0, 15, 409, 125)
	GROUP_HOTSPOT(Ydisplay, Axis, 0, 156, 409, 266)
	GROUP_HOTSPOT(Zdisplay, Axis, 0, 297, 409, 407)
END_GROUP(Axis)

START_GROUP()
	GROUP_HOTSPOT(Xbutton, , 435, 22, 519, 117)
	GROUP_HOTSPOT(Ybutton, , 435, 163, 519, 258)
	GROUP_HOTSPOT(Zbutton, , 435, 304, 519, 399)
	GROUP_HOTSPOT(InchMetric, , 480, 507, 539, 566)
	GROUP_HOTSPOT(AbsInc, , 400, 507, 459, 566)
	GROUP_HOTSPOT(Undo, , 560, 15, 849, 98)
	GROUP_HOTSPOT(CalcDisplay, , 560, 109, 849, 168)
END_GROUP()

START_GROUP(Edit)
	GROUP_HOTSPOT(Key_backSpace, Edit, 570, 227, 629, 286)
	GROUP_HOTSPOT(Key_clear, Edit, 640, 227, 699, 286)
	GROUP_HOTSPOT(Key_pi, Edit, 710, 227, 769, 286)
END_GROUP(Edit)

START_GROUP(Operator)
	GROUP_HOTSPOT(Key_plus, Operator, 780, 227, 839, 286)
	GROUP_HOTSPOT(Key_minus, Operator, 780, 297, 839, 356)
	GROUP_HOTSPOT(Key_mult, Operator, 780, 367, 839, 426)
	GROUP_HOTSPOT(Key_divide, Operator, 780, 437, 839, 496)
	GROUP_HOTSPOT(Key_equal, Operator, 780, 507, 839, 566)
END_GROUP(Operator)

START_GROUP(Memory)
	GROUP_HOTSPOT(Mem1, Memory, 850, 227, 999, 286)
	GROUP_HOTSPOT(Mem2, Memory, 850, 297, 999, 356)
	GROUP_HOTSPOT(Mem3, Memory, 850, 367, 999, 426)
	GROUP_HOTSPOT(Mem4, Memory, 850, 437, 999, 496)
	GROUP_HOTSPOT(Mem5, Memory, 850, 507, 999, 566)
END_GROUP(Memory)

START_GROUP(Digit)
	GROUP_HOTSPOT(Key_7, Digit, 570, 297, 629, 356)
	GROUP_HOTSPOT(Key_8, Digit, 640, 297, 699, 356)
	GROUP_HOTSPOT(Key_9, Digit, 710, 297, 769, 356)
	GROUP_HOTSPOT(Key_4, Digit, 570, 367, 629, 426)
	GROUP_HOTSPOT(Key_5, Digit, 640, 367, 699, 426)
	GROUP_HOTSPOT(Key_6, Digit, 710, 367, 769, 426)
	GROUP_HOTSPOT(Key_1, Digit, 570, 437, 629, 496)
	GROUP_HOTSPOT(Key_2, Digit, 640, 437, 699, 496)
	GROUP_HOTSPOT(Key_3, Digit, 710, 437, 769, 496)
	GROUP_HOTSPOT(Key_sign, Digit, 570, 507, 629, 566)
	GROUP_HOTSPOT(Key_0, Digit, 640, 507, 699, 566)
	GROUP_HOTSPOT(Key_decimal, Digit, 710, 507, 769, 566)
END_GROUP(Digit)

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
	GROUP_HOTSPOT_Axis(Xdisplay, Axis, 0, 15, 409, 125)
	GROUP_HOTSPOT_Axis(Ydisplay, Axis, 0, 156, 409, 266)
	GROUP_HOTSPOT_Axis(Zdisplay, Axis, 0, 297, 409, 407)
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
	GROUP_HOTSPOT_(Xbutton, , 435, 22, 519, 117)
	GROUP_HOTSPOT_(Ybutton, , 435, 163, 519, 258)
	GROUP_HOTSPOT_(Zbutton, , 435, 304, 519, 399)
	GROUP_HOTSPOT_(InchMetric, , 480, 507, 539, 566)
	GROUP_HOTSPOT_(AbsInc, , 400, 507, 459, 566)
	GROUP_HOTSPOT_(Undo, , 560, 15, 849, 98)
	GROUP_HOTSPOT_(CalcDisplay, , 560, 109, 849, 168)
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
	GROUP_HOTSPOT_Edit(Key_backSpace, Edit, 570, 227, 629, 286)
	GROUP_HOTSPOT_Edit(Key_clear, Edit, 640, 227, 699, 286)
	GROUP_HOTSPOT_Edit(Key_pi, Edit, 710, 227, 769, 286)
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
	GROUP_HOTSPOT_Operator(Key_plus, Operator, 780, 227, 839, 286)
	GROUP_HOTSPOT_Operator(Key_minus, Operator, 780, 297, 839, 356)
	GROUP_HOTSPOT_Operator(Key_mult, Operator, 780, 367, 839, 426)
	GROUP_HOTSPOT_Operator(Key_divide, Operator, 780, 437, 839, 496)
	GROUP_HOTSPOT_Operator(Key_equal, Operator, 780, 507, 839, 566)
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
	GROUP_HOTSPOT_Memory(Mem1, Memory, 850, 227, 999, 286)
	GROUP_HOTSPOT_Memory(Mem2, Memory, 850, 297, 999, 356)
	GROUP_HOTSPOT_Memory(Mem3, Memory, 850, 367, 999, 426)
	GROUP_HOTSPOT_Memory(Mem4, Memory, 850, 437, 999, 496)
	GROUP_HOTSPOT_Memory(Mem5, Memory, 850, 507, 999, 566)
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
	GROUP_HOTSPOT_Digit(Key_7, Digit, 570, 297, 629, 356)
	GROUP_HOTSPOT_Digit(Key_8, Digit, 640, 297, 699, 356)
	GROUP_HOTSPOT_Digit(Key_9, Digit, 710, 297, 769, 356)
	GROUP_HOTSPOT_Digit(Key_4, Digit, 570, 367, 629, 426)
	GROUP_HOTSPOT_Digit(Key_5, Digit, 640, 367, 699, 426)
	GROUP_HOTSPOT_Digit(Key_6, Digit, 710, 367, 769, 426)
	GROUP_HOTSPOT_Digit(Key_1, Digit, 570, 437, 629, 496)
	GROUP_HOTSPOT_Digit(Key_2, Digit, 640, 437, 699, 496)
	GROUP_HOTSPOT_Digit(Key_3, Digit, 710, 437, 769, 496)
	GROUP_HOTSPOT_Digit(Key_sign, Digit, 570, 507, 629, 566)
	GROUP_HOTSPOT_Digit(Key_0, Digit, 640, 507, 699, 566)
	GROUP_HOTSPOT_Digit(Key_decimal, Digit, 710, 507, 769, 566)
END_GROUP_Digit(Digit)

#undef START_GROUP_Digit
#undef GROUP_HOTSPOT_Digit
#undef END_GROUP_Digit

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

START_SCREEN_Key(AbsCoord)
	IMAGE_ADDRESS_Key(1228800)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(AbsCoord)

START_SCREEN_Key(IncCoord)
	IMAGE_ADDRESS_Key(1236000)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(IncCoord)

START_SCREEN_Key(Inch)
	IMAGE_ADDRESS_Key(1243200)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(Inch)

START_SCREEN_Key(Metric)
	IMAGE_ADDRESS_Key(1250400)
	IMAGE_SIZE_Key(7200)
	IMAGE_WIDTH_Key(60)
	IMAGE_HEIGHT_Key(60)
	IMAGE_DEPTH_Key(Color16bpp)
END_SCREEN_Key(Metric)

START_SCREEN_Key(Pattern16)
	IMAGE_ADDRESS_Key(1257600)
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

SCREEN_FILE_LENGTH(1871552)

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
#undef DEFINE_COLOR
#undef SCREEN_FILE_LENGTH
