#ifndef PIGU_INPUT_H
#define PIGU_INPUT_H

#include <inttypes.h>
#include <fcntl.h>
#include <linux/input.h>

typedef enum 
{
   PIGU_UNKNOWN,
   PIGU_KEYBOARD, 
   PIGU_MOUSE, 
   PIGU_GAMEPAD, 
   PIGU_JOYSTICK
} PIGU_device_type_t;

typedef struct {
   uint8_t count;
   uint8_t map[32];
   int16_t position[32];
   int16_t min[32];
   int16_t max[32];
} PIGU_axis_data_t;

typedef struct {
   uint8_t count;
   uint8_t map[16];
   uint8_t state[16];	 
} PIGU_button_data_t;


typedef struct {
   int fd;
   char name[256];
   PIGU_device_type_t type;
   union {
      struct {
	 uint16_t keys;
	 uint32_t key_state[(KEY_MAX-1)/32+1];
      } keyboard;
      struct {
	 int32_t position[2];
	 int32_t wheel;
	 PIGU_button_data_t buttons;
      } mouse;
      struct {
     	 PIGU_button_data_t buttons;	 
	 PIGU_axis_data_t axes;
      } controller;
   };
} PIGU_device_info_t;


void PIGU_init_axis_data(PIGU_axis_data_t *data);

void PIGU_init_button_data(PIGU_button_data_t *data);

int PIGU_detect_device(const char *device, PIGU_device_info_t *info);

int PIGU_poll_events_device(PIGU_device_info_t *info);


#endif