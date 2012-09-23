#include "state.h"
#include "pigu.h"

#include <assert.h>

int piguCreateWindow(int width, int height, int red, int green,int blue, int alpha, int depth, int stencil)
{
   EGLBoolean result;
   EGLint num_config;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   EGLint attribute_list[15];
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
   attribute_list[14] = EGL_NONE;
   
   EGLint context_attributes[3]; 
   context_attributes[0] = EGL_CONTEXT_CLIENT_VERSION;
   context_attributes[1] = 2;
   context_attributes[2] = EGL_NONE;

   EGLConfig config;

   // get an EGL display connection
   state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state.display!=EGL_NO_DISPLAY);


   // initialize the EGL display connection
   result = eglInitialize(state.display, NULL, NULL);
   assert(EGL_FALSE != result);


   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state.display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);


   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);


   // create an EGL rendering context
   state.context = eglCreateContext(state.display, config, EGL_NO_CONTEXT, context_attributes);
   assert(state.context!=EGL_NO_CONTEXT);


   // create an EGL window surface
   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = width;
   dst_rect.height = height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = width << 16;
   src_rect.height = height << 16;        

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
					       &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);
      
   state.nativewindow.element = dispman_element;
   state.nativewindow.width = width;
   state.nativewindow.height = height;
   vc_dispmanx_update_submit_sync( dispman_update );
      


   state.surface = eglCreateWindowSurface( state.display, config, &state.nativewindow, NULL );
   assert(state.surface != EGL_NO_SURFACE);


   // connect the context to the surface
   result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
   assert(EGL_FALSE != result);

   return 0;
}    

void piguSwapBuffers()
{
   eglSwapBuffers(state.display, state.surface);
}

int piguGetScreenSize(int *width, int *height)
{
   int32_t success = 0;
   uint32_t screen_width = 0, screen_height = 0;

   success = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);

   *width = screen_width;
   *height = screen_height;
   return success;
}
