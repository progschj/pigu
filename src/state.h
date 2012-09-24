#ifndef PIGU_STATE_H
#define PIGU_STATE_H

#include "input.h"

#include "bcm_host.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define MAX_MOUSE_COUNT 4
#define MAX_KEYBOARD_COUNT 4
#define MAX_CONTROLLER_COUNT 4
#define MAX_EVENT_INDEX 16

typedef struct {
   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   uint32_t screen_width, screen_height;
   int window_open;
   
   EGL_DISPMANX_WINDOW_T nativewindow;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   EGLConfig config;

   int keyboard_count;   
   PIGU_device_info_t keyboard[MAX_KEYBOARD_COUNT];
   
   int mouse_count;
   PIGU_device_info_t mouse[MAX_MOUSE_COUNT];
   
   int controller_count;
   PIGU_device_info_t controller[MAX_CONTROLLER_COUNT];
} PIGU_global_state_t;

extern PIGU_global_state_t state;

#endif
