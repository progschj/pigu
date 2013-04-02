#include "pigu.h"
#include "state.h"
#include "input.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

int piguInit()
{
   int i;
   bcm_host_init();

   memset(&state, 0, sizeof(state));
   state.keyboard[0].fd = -1;
   state.mouse[0].fd = -1;
   for(i = 0;i<4;++i)
      state.controller[i].fd = -1;
   
   char name[64];
   for(i = 0;i<MAX_EVENT_INDEX;++i)
   {
      sprintf(name, "/dev/input/event%d", i);

      PIGU_device_info_t info;
      if(PIGU_detect_device(name, &info) < 0)
	 continue;

      switch(info.type)
      {
      case PIGU_KEYBOARD:
	 if(state.keyboard_count >= MAX_KEYBOARD_COUNT)
	    break;
	 state.keyboard[state.keyboard_count] = info;
	 state.keyboard_count++;
	 break;

      case PIGU_MOUSE:
      	 if(state.mouse_count >= MAX_MOUSE_COUNT)
	    break;
	 state.mouse[state.mouse_count] = info;
	 state.mouse_count++;
	 break;

      case PIGU_JOYSTICK:
      case PIGU_GAMEPAD:
	 if(state.controller_count >= MAX_CONTROLLER_COUNT)
	    break;
	 state.controller[state.controller_count] = info;
	 state.controller_count++;
	 break;

      default:
	 break;
      }
   }
   return 0;
}

void piguTerminate()
{
   piguCloseWindow();

   if(state.keyboard[0].fd >= 0)
      close(state.keyboard[0].fd);
   if(state.mouse[0].fd >= 0)
      close(state.mouse[0].fd);
   
   int i;
   for(i = 0;i<4;++i)
      if(state.controller[i].fd >= 0)
	 close(state.controller[i].fd);

   bcm_host_deinit();
}
