#include "input.h"
#include "state.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>

#include "pigu_util.h"

void PIGU_init_axis_data(PIGU_axis_data_t *data)
{
   memset(data, 0, sizeof(PIGU_axis_data_t));
   int i;
   for(i = 0;i<32;++i)
      data->map[i] = -1;
}
void PIGU_init_button_data(PIGU_button_data_t *data)
{
   memset(data, 0, sizeof(PIGU_button_data_t));
   int i;
   for(i = 0;i<16;++i)
      data->map[i] = -1;
}

int PIGU_detect_device(const char *device, PIGU_device_info_t *info)
{
   uint32_t types[EV_MAX];
   uint32_t events[(KEY_MAX-1)/32+1];
   
   int fd = open(device, O_RDONLY | O_NONBLOCK);

   if(fd<0)
      return -1;

   memset(info, 0, sizeof(PIGU_device_info_t));
   info->fd = fd;
   
   // get device name
   ioctl(fd, EVIOCGNAME(sizeof(info->name)), info->name);

   // query supported event types
   memset(types, 0, sizeof(types));
   ioctl(fd, EVIOCGBIT(0, EV_MAX), types);

   int key_count = 0;
   int mouse_button_count = 0;
   int joystick_button_count = 0;
   int gamepad_button_count = 0;
   PIGU_axis_data_t axes;
   PIGU_init_axis_data(&axes);
   PIGU_button_data_t buttons;
   PIGU_init_button_data(&buttons);

   if(PIGU_get_bit(types, EV_KEY))
   {
      // count events
      memset(events, 0, sizeof(events));
      ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), events);
      int j = 0;
      for(;j<BTN_MISC;++j)
	 if(PIGU_get_bit(events, j))
	    key_count++;

      j = BTN_MOUSE; // skip misc buttons
      
      for(;j<BTN_JOYSTICK;++j)
	 if(PIGU_get_bit(events, j))
	 {
	    mouse_button_count++;
	    buttons.map[buttons.count] = j-BTN_MOUSE;
	    buttons.count++;
	 }

      for(;j<BTN_GAMEPAD;++j)
	 if(PIGU_get_bit(events, j))
	 {
	    joystick_button_count++;
	    buttons.map[buttons.count] = j-BTN_JOYSTICK;
	    buttons.count++;
	 }

      for(;j<BTN_DIGI;++j)
	 if(PIGU_get_bit(events, j))
	 {
	    gamepad_button_count++;
	    buttons.map[buttons.count] = j-BTN_GAMEPAD;
	    buttons.count++;
	 }
   }


   if(PIGU_get_bit(types, EV_ABS))
   {
      struct input_absinfo abs;
      memset(events, 0, sizeof(events));
      ioctl(fd, EVIOCGBIT(EV_ABS, KEY_MAX), events);
      
      int j = 0;
      for(;j<32;++j)
	 if(PIGU_get_bit(events, j))
	 {
	    axes.map[axes.count] = j;
	    ioctl(fd, EVIOCGABS(j), &abs);

	    axes.position[j] = abs.value;
	    axes.min[j] = abs.minimum;
	    axes.max[j] = abs.maximum;
	    axes.count++;
	 }
   }

   info->type = PIGU_UNKNOWN;
   if(gamepad_button_count > 0)
   {
      info->type = PIGU_GAMEPAD;
      info->controller.buttons = buttons;
      info->controller.axes = axes;
   }
   else if(joystick_button_count > 0)
   {
      info->type = PIGU_JOYSTICK;
      info->controller.buttons = buttons;
      info->controller.axes = axes;
   }
   else  if(mouse_button_count > 0)
   {
      info->type = PIGU_MOUSE;
      info->mouse.buttons = buttons;
   }
   else if(key_count > 0)
   {
      info->keyboard.keys = key_count;
      info->type = PIGU_KEYBOARD;
   }
   else
   {
      close(info->fd);
   }

   return 0;
}

int PIGU_poll_events_device(PIGU_device_info_t *info)
{
   struct input_event event;   
   ssize_t readsize = -1;
   int eventcount = 0;

   if(info->fd < 0)
      return -1;

   readsize = read(info->fd, &event, sizeof(event));
   while(readsize >= 0)
   {
      int button_idx;
      if(event.type == EV_KEY)
      {
	 switch(info->type)
	 {
	 case PIGU_KEYBOARD:
	    //info->keyboard.key_state[event.code] = event.value;
	    PIGU_set_bit(info->keyboard.key_state, event.code, event.value);
	    break;

	 case PIGU_GAMEPAD:
	    button_idx = event.code-BTN_GAMEPAD;
	    if(button_idx<16)
	       info->controller.buttons.state[button_idx] = event.value;
	    break;
	 case PIGU_JOYSTICK:
	    button_idx = event.code-BTN_JOYSTICK;
	    if(button_idx<16)
	       info->controller.buttons.state[button_idx] = event.value;
	    break;

	 case PIGU_MOUSE:
	    button_idx = event.code-BTN_MOUSE;
	    if(button_idx<16)
	       info->mouse.buttons.state[button_idx] = event.value;
	    break;
 
	 default:
	    break;
	 }
      }	
      else if(event.type == EV_ABS && (info->type == PIGU_GAMEPAD || info->type == PIGU_JOYSTICK))
      {
	 info->controller.axes.position[event.code] = event.value;
      }
      else if(event.type == EV_REL && info->type == PIGU_MOUSE)
      {
	 if(event.code < 2)
	    info->mouse.position[event.code] += event.value;
	 else if(event.code == REL_WHEEL)
	    info->mouse.wheel += event.value;
      }

      eventcount++;
      readsize = read(info->fd, &event, sizeof(event));
   }

   return eventcount;
}

int piguPollEvents()
{
   int event_count = 0;
   int i;
   event_count += PIGU_poll_events_device(state.keyboard);
   event_count += PIGU_poll_events_device(state.mouse);
   for(i = 0;i<state.controller_count;++i)
	  event_count += PIGU_poll_events_device(state.controller+i);

   return event_count;
}

int piguIsKeyDown(int keycode)
{
   return keycode < KEY_MAX ? PIGU_get_bit(state.keyboard[0].keyboard.key_state, keycode) : 0;
}
 
int piguIsControllerButtonDown(int controller, int button)
{
   if(controller >= state.controller_count)
      return 0;
   if(button >= state.controller[controller].controller.buttons.count)
      return 0;
   int idx = state.controller[controller].controller.buttons.map[button];
   return state.controller[controller].controller.buttons.state[idx];
}

void piguGetMousePosition(int *x, int *y)
{
   *x = state.mouse[0].mouse.position[0];
   *y = state.mouse[0].mouse.position[1];
}

int piguGetMouseWheelPosition()
{
  return state.mouse[0].mouse.wheel;
}

float piguGetAxis(int controller, int axis)
{
   if(controller >= state.controller_count)
      return 0;
   if(axis >= state.controller[controller].controller.axes.count)
      return 0;
   int idx = state.controller[controller].controller.axes.map[axis];

   int v = state.controller[controller].controller.axes.position[idx];
   
   if(v >= 0) 
      return v/(float)state.controller[controller].controller.axes.max[idx];
   else
      return -(v/(float)state.controller[controller].controller.axes.min[idx]);
}     
