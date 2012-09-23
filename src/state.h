#ifndef PIGU_STATE_H
#define PIGU_STATE_H

#include "input.h"

#include "bcm_host.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

typedef struct {
   EGL_DISPMANX_WINDOW_T nativewindow;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;

   PIGU_device_info_t keyboard[1];
   PIGU_device_info_t mouse[1];
   
   int controller_count;
   PIGU_device_info_t controller[4];
} PIGU_global_state_t;

extern PIGU_global_state_t state;

#endif
