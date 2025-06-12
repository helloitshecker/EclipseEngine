#pragma once

/* Printable keys */
#define E_KEY_SPACE              32
#define E_KEY_APOSTROPHE         39  /* ' */
#define E_KEY_COMMA              44  /* , */
#define E_KEY_MINUS              45  /* - */
#define E_KEY_PERIOD             46  /* . */
#define E_KEY_SLASH              47  /* / */
#define E_KEY_0                  48
#define E_KEY_1                  49
#define E_KEY_2                  50
#define E_KEY_3                  51
#define E_KEY_4                  52
#define E_KEY_5                  53
#define E_KEY_6                  54
#define E_KEY_7                  55
#define E_KEY_8                  56
#define E_KEY_9                  57
#define E_KEY_SEMICOLON          59  /* ; */
#define E_KEY_EQUAL              61  /* = */
#define E_KEY_A                  65
#define E_KEY_B                  66
#define E_KEY_C                  67
#define E_KEY_D                  68
#define E_KEY_E                  69
#define E_KEY_F                  70
#define E_KEY_G                  71
#define E_KEY_H                  72
#define E_KEY_I                  73
#define E_KEY_J                  74
#define E_KEY_K                  75
#define E_KEY_L                  76
#define E_KEY_M                  77
#define E_KEY_N                  78
#define E_KEY_O                  79
#define E_KEY_P                  80
#define E_KEY_Q                  81
#define E_KEY_R                  82
#define E_KEY_S                  83
#define E_KEY_T                  84
#define E_KEY_U                  85
#define E_KEY_V                  86
#define E_KEY_W                  87
#define E_KEY_X                  88
#define E_KEY_Y                  89
#define E_KEY_Z                  90
#define E_KEY_LEFT_BRACKET       91  /* [ */
#define E_KEY_BACKSLASH          92  /* \ */
#define E_KEY_RIGHT_BRACKET      93  /* ] */
#define E_KEY_GRAVE_ACCENT       96  /* ` */
#define E_KEY_WORLD_1            161 /* non-US #1 */
#define E_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define E_KEY_ESCAPE             256
#define E_KEY_ENTER              257
#define E_KEY_TAB                258
#define E_KEY_BACKSPACE          259
#define E_KEY_INSERT             260
#define E_KEY_DELETE             261
#define E_KEY_RIGHT              262
#define E_KEY_LEFT               263
#define E_KEY_DOWN               264
#define E_KEY_UP                 265
#define E_KEY_PAGE_UP            266
#define E_KEY_PAGE_DOWN          267
#define E_KEY_HOME               268
#define E_KEY_END                269
#define E_KEY_CAPS_LOCK          280
#define E_KEY_SCROLL_LOCK        281
#define E_KEY_NUM_LOCK           282
#define E_KEY_PRINT_SCREEN       283
#define E_KEY_PAUSE              284
#define E_KEY_F1                 290
#define E_KEY_F2                 291
#define E_KEY_F3                 292
#define E_KEY_F4                 293
#define E_KEY_F5                 294
#define E_KEY_F6                 295
#define E_KEY_F7                 296
#define E_KEY_F8                 297
#define E_KEY_F9                 298
#define E_KEY_F10                299
#define E_KEY_F11                300
#define E_KEY_F12                301
#define E_KEY_F13                302
#define E_KEY_F14                303
#define E_KEY_F15                304
#define E_KEY_F16                305
#define E_KEY_F17                306
#define E_KEY_F18                307
#define E_KEY_F19                308
#define E_KEY_F20                309
#define E_KEY_F21                310
#define E_KEY_F22                311
#define E_KEY_F23                312
#define E_KEY_F24                313
#define E_KEY_F25                314
#define E_KEY_KP_0               320
#define E_KEY_KP_1               321
#define E_KEY_KP_2               322
#define E_KEY_KP_3               323
#define E_KEY_KP_4               324
#define E_KEY_KP_5               325
#define E_KEY_KP_6               326
#define E_KEY_KP_7               327
#define E_KEY_KP_8               328
#define E_KEY_KP_9               329
#define E_KEY_KP_DECIMAL         330
#define E_KEY_KP_DIVIDE          331
#define E_KEY_KP_MULTIPLY        332
#define E_KEY_KP_SUBTRACT        333
#define E_KEY_KP_ADD             334
#define E_KEY_KP_ENTER           335
#define E_KEY_KP_EQUAL           336
#define E_KEY_LEFT_SHIFT         340
#define E_KEY_LEFT_CONTROL       341
#define E_KEY_LEFT_ALT           342
#define E_KEY_LEFT_SUPER         343
#define E_KEY_RIGHT_SHIFT        344
#define E_KEY_RIGHT_CONTROL      345
#define E_KEY_RIGHT_ALT          346
#define E_KEY_RIGHT_SUPER        347
#define E_KEY_MENU               348

#define E_KEY_LAST               E_KEY_MENU

#define E_MOD_SHIFT           0x0001
#define E_MOD_CONTROL         0x0002
#define E_MOD_ALT             0x0004
#define E_MOD_SUPER           0x0008
#define E_MOD_CAPS_LOCK       0x0010
#define E_MOD_NUM_LOCK        0x0020
#define E_MOUSE_BUTTON_1         0
#define E_MOUSE_BUTTON_2         1
#define E_MOUSE_BUTTON_3         2
#define E_MOUSE_BUTTON_4         3
#define E_MOUSE_BUTTON_5         4
#define E_MOUSE_BUTTON_6         5
#define E_MOUSE_BUTTON_7         6
#define E_MOUSE_BUTTON_8         7
#define E_MOUSE_BUTTON_LAST      E_MOUSE_BUTTON_8
#define E_MOUSE_BUTTON_LEFT      E_MOUSE_BUTTON_1
#define E_MOUSE_BUTTON_RIGHT     E_MOUSE_BUTTON_2
#define E_MOUSE_BUTTON_MIDDLE    E_MOUSE_BUTTON_3
#define E_JOYSTICK_1             0
#define E_JOYSTICK_2             1
#define E_JOYSTICK_3             2
#define E_JOYSTICK_4             3
#define E_JOYSTICK_5             4
#define E_JOYSTICK_6             5
#define E_JOYSTICK_7             6
#define E_JOYSTICK_8             7
#define E_JOYSTICK_9             8
#define E_JOYSTICK_10            9
#define E_JOYSTICK_11            10
#define E_JOYSTICK_12            11
#define E_JOYSTICK_13            12
#define E_JOYSTICK_14            13
#define E_JOYSTICK_15            14
#define E_JOYSTICK_16            15
#define E_JOYSTICK_LAST          E_JOYSTICK_16
#define E_GAMEPAD_BUTTON_A               0
#define E_GAMEPAD_BUTTON_B               1
#define E_GAMEPAD_BUTTON_X               2
#define E_GAMEPAD_BUTTON_Y               3
#define E_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define E_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define E_GAMEPAD_BUTTON_BACK            6
#define E_GAMEPAD_BUTTON_START           7
#define E_GAMEPAD_BUTTON_GUIDE           8
#define E_GAMEPAD_BUTTON_LEFT_THUMB      9
#define E_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define E_GAMEPAD_BUTTON_DPAD_UP         11
#define E_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define E_GAMEPAD_BUTTON_DPAD_DOWN       13
#define E_GAMEPAD_BUTTON_DPAD_LEFT       14
#define E_GAMEPAD_BUTTON_LAST            E_GAMEPAD_BUTTON_DPAD_LEFT

#define E_GAMEPAD_BUTTON_CROSS       E_GAMEPAD_BUTTON_A
#define E_GAMEPAD_BUTTON_CIRCLE      E_GAMEPAD_BUTTON_B
#define E_GAMEPAD_BUTTON_SQUARE      E_GAMEPAD_BUTTON_X
#define E_GAMEPAD_BUTTON_TRIANGLE    E_GAMEPAD_BUTTON_Y
#define E_GAMEPAD_AXIS_LEFT_X        0
#define E_GAMEPAD_AXIS_LEFT_Y        1
#define E_GAMEPAD_AXIS_RIGHT_X       2
#define E_GAMEPAD_AXIS_RIGHT_Y       3
#define E_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define E_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define E_GAMEPAD_AXIS_LAST          E_GAMEPAD_AXIS_RIGHT_TRIGGER

#define E_RELEASE                0
#define E_PRESS                  1
#define E_REPEAT                 2
#define E_HAT_CENTERED           0
#define E_HAT_UP                 1
#define E_HAT_RIGHT              2
#define E_HAT_DOWN               4
#define E_HAT_LEFT               8
#define E_HAT_RIGHT_UP           (E_HAT_RIGHT | E_HAT_UP)
#define E_HAT_RIGHT_DOWN         (E_HAT_RIGHT | E_HAT_DOWN)
#define E_HAT_LEFT_UP            (E_HAT_LEFT  | E_HAT_UP)
#define E_HAT_LEFT_DOWN          (E_HAT_LEFT  | E_HAT_DOWN)
#define E_KEY_UNKNOWN            -1

#include <GLFW/glfw3.h>
#include <EclipseUtils/Defines.hpp>
#include <functional>

namespace et {
	typedef GLFWwindow* window;
	typedef i32 key;
	typedef i32 keystate;
	typedef i32 msaasamples;
};

namespace e {
	class window {
	public:
		et::i32 window_width = 0;
		et::i32 window_height = 0;
		et::window pwindow = nullptr;

		struct WindowCreateInfo {
			char* title;
			et::i32 width;
			et::i32 height;
			bool resizable;
			bool fullscreen;
			bool vsync;
			et::msaasamples samples;
			GLFWframebuffersizefun resize_callback;
			GLFWcursorposfun cursor_pos_callback;
		};

		window(WindowCreateInfo& window_create_info);
		void update() const;
		bool shouldClose() const;
		~window();

		static GLFWglproc getProcAddress(const char* procname);

		double getTime() const;
		double getIntervalToPreviousCall() const;
		
		et::keystate getKeyState(et::key key) const;

		void setVsync(bool state) const;
	};
};