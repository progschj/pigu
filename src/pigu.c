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
   for(i = 0;i<16;++i)
   {
      sprintf(name, "/dev/input/event%d", i);

      PIGU_device_info_t info;
      if(PIGU_detect_device(name, &info) < 0)
	 continue;

      switch(info.type)
      {
      case PIGU_KEYBOARD:
	 if(state.keyboard[0].keyboard.keys == 0)
	 {
	    state.keyboard[0] = info;
	 }
	 break;

      case PIGU_MOUSE:
	 state.mouse[0] = info;
	 break;

      case PIGU_JOYSTICK:
      case PIGU_GAMEPAD:
	 if(state.controller_count >= 4)
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
