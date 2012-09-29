#include "state.h"
#include "pigu.h"

int piguGetScreenSize(int *width, int *height)
{
   int32_t success = 0;
   uint32_t screen_width = 0, screen_height = 0;

   // this function is just a wrapper around this bcm api function
   success = graphics_get_display_size(0, &screen_width, &screen_height);

   *width = screen_width;
   *height = screen_height;
   return success;
}


int piguCreateWindow(int width, int height, int red, int green,int blue, int alpha, int depth, int stencil, int samples)
{
   // this pretty much follows the example from the "sdk".
   // See hello_triangle2/triangle.c for the original code
   EGLBoolean result;
   EGLint num_config;

   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   if(0 > graphics_get_display_size(0, &state.screen_width, &state.screen_height))
      return -1;

   EGLint attribute_list[20];
   int i;
   for(i = 0;i<20;++i)
      attribute_list[i] = EGL_NONE;

   attribute_list[ 0] = EGL_RED_SIZE;
   attribute_list[ 1] = red;
   attribute_list[ 2] = EGL_GREEN_SIZE;
   attribute_list[ 3] = green;
   attribute_list[ 4] = EGL_BLUE_SIZE;
   attribute_list[ 5] = blue;
   attribute_list[ 6] = EGL_ALPHA_SIZE;
   attribute_list[ 7] = alpha;
   attribute_list[ 8] = EGL_DEPTH_SIZE;
   attribute_list[ 9] = depth;
   attribute_list[10] = EGL_STENCIL_SIZE;    
   attribute_list[11] = stencil;
   attribute_list[12] = EGL_SURFACE_TYPE;    
   attribute_list[13] = EGL_WINDOW_BIT;

   // only set these if we actually want multisampling. It seems the moment
   // a sample count is set it automatically ends up doing 4x MSAA
   if(samples>2)
   {
      attribute_list[14] = EGL_SAMPLES;
      attribute_list[15] = samples;
      attribute_list[16] = EGL_SAMPLE_BUFFERS;
      attribute_list[17] = 1;
   }
   
   EGLint context_attributes[3]; 
   context_attributes[0] = EGL_CONTEXT_CLIENT_VERSION;
   context_attributes[1] = 2;
   context_attributes[2] = EGL_NONE;

   state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

   if(state.display==EGL_NO_DISPLAY)
      return -1;

   result = eglInitialize(state.display, NULL, NULL);
   if(EGL_FALSE == result)
      return -1;

   result = eglChooseConfig(state.display, attribute_list, &state.config, 1, &num_config);
   if(EGL_FALSE == result)
      return -1;

   result = eglBindAPI(EGL_OPENGL_ES_API);
   if(EGL_FALSE == result)
      return -1;

   state.context = eglCreateContext(state.display, state.config, EGL_NO_CONTEXT, context_attributes);
   if(state.context==EGL_NO_CONTEXT)
      return -1;

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state.screen_width;
   dst_rect.height = state.screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = width << 16;
   src_rect.height = height << 16;        

   state.dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );

   VC_DISPMANX_ALPHA_T alpha_flags;
   alpha_flags.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
   alpha_flags.opacity = 255;
   alpha_flags.mask = 0;

   state.dispman_element = vc_dispmanx_element_add ( dispman_update, state.dispman_display, 0/*layer*/, &dst_rect, 0/*src*/, &src_rect, DISPMANX_PROTECTION_NONE, &alpha_flags /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);
      
   state.nativewindow.element = state.dispman_element;
   state.nativewindow.width = width;
   state.nativewindow.height = height;
   vc_dispmanx_update_submit_sync( dispman_update );

   state.surface = eglCreateWindowSurface( state.display, state.config, &state.nativewindow, NULL );
   if(state.surface == EGL_NO_SURFACE)
      return -1;

   result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
   if(EGL_FALSE == result)
      return -1;

   state.window_open = 1;

   return 0;
}    

void piguSwapBuffers()
{
   if(state.window_open)
      eglSwapBuffers(state.display, state.surface);
}

void piguSwapInterval(int interval)
{
   if(state.window_open)
      eglSwapInterval(state.display, interval);
}

int piguChangeResolution(int width, int height)
{
   EGLBoolean result;

   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;


   if(!state.window_open)
      return 0;

   eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

   eglDestroySurface(state.display, state.surface);

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state.screen_width;
   dst_rect.height = state.screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = width << 16;
   src_rect.height = height << 16;        

   DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start( 0 );

   vc_dispmanx_element_remove(dispman_update, state.dispman_element);
      
   state.dispman_element = vc_dispmanx_element_add ( dispman_update, state.dispman_display, 0/*layer*/, &dst_rect, 0/*src*/, &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);
      
   state.nativewindow.element = state.dispman_element;
   state.nativewindow.width = width;
   state.nativewindow.height = height;
   vc_dispmanx_update_submit_sync( dispman_update );

   state.surface = eglCreateWindowSurface( state.display, state.config, &state.nativewindow, NULL );
   if(state.surface == EGL_NO_SURFACE)
      return -1;

   result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
   if(EGL_FALSE == result)
      return -1;

   return 0;
}


void piguCloseWindow()
{
   if(!state.window_open)
      return;

   eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

   eglDestroySurface(state.display, state.surface);

   DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start( 0 );
   vc_dispmanx_element_remove(dispman_update, state.dispman_element);
   vc_dispmanx_update_submit_sync( dispman_update );

   vc_dispmanx_display_close(state.dispman_display);

   eglDestroyContext(state.display, state.context);

   state.window_open = 0;
}
