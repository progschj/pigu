#ifndef PIGU_STATE_H
#define PIGU_STATE_H

#include "input.h"

#include "bcm_host.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

// the max device counts that pigu will detect per type
// if more than this amount is present they wills simply
// be ignored
#define MAX_MOUSE_COUNT 4
#define MAX_KEYBOARD_COUNT 4
#define MAX_CONTROLLER_COUNT 4

// this sets how many entries in /dev/input will be checked
// so a value of 16 will result in piguInit checking
// /dev/input/event0 up to /dev/input/event15
#define MAX_EVENT_INDEX 16

typedef struct {
   // firmware related windowing stuff
   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   uint32_t screen_width, screen_height;
   int window_open;
   
   // egl types
   EGL_DISPMANX_WINDOW_T nativewindow;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   EGLConfig config;

   // input devices
   int keyboard_count;   
   PIGU_device_info_t keyboard[MAX_KEYBOARD_COUNT];
   
   int mouse_count;
   PIGU_device_info_t mouse[MAX_MOUSE_COUNT];
   
   int controller_count;
   PIGU_device_info_t controller[MAX_CONTROLLER_COUNT];
} PIGU_global_state_t;

extern PIGU_global_state_t state;

#endif
