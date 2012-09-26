#ifndef PIGU_H
#define PIGU_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
   PIGU_UNKNOWN,
   PIGU_KEYBOARD, 
   PIGU_MOUSE, 
   PIGU_GAMEPAD, 
   PIGU_JOYSTICK
} PIGU_device_type_t;

/*
 * piguInit initializes the internal state and has to be called
 * before any of the other pigu functions. Calls to other
 * pigu functions before piguInit have undefined behavior.
 */
int piguInit();

/*
 * piguTerminate should be called at the end of the program
 * to free all ressources.
 */
void piguTerminate();

/*
 * piguPollEvents polls all input devices and updates their state
 */
int piguPollEvents();

/*
 * piguIsKeyDown returns 1 if the key is pressed 0 otherwise
 * for keycodes see linux/input.h
 */
int piguIsKeyDown(int keycode);

/*
 * piguGetControllerCount returns the amount of game
 * controllers (gamepads & joysticks) found during init.
 */
int piguGetControllerCount();

/*
 * piguGetControllerType returns the detected type for a
 * controller (PIGU_JOYSTICK or PIGU_GAMEPAD). if
 * no controller with that number is available
 * PIGU_UNKNOWN is returned.
 */
PIGU_device_type_t piguGetControllerType(int controller);

/*
 * piguIsControllerButtonDown returns 1 if the button is pressed
 * otherwise 0. Buttons are numbered from 0.
 */
int piguIsControllerButtonDown(int controller, int button);

/*
 * piguGetAxis returns the position of some analog axis of
 * the controller. The range is [-1,1] or [0,1] depending
 * on axis type.
 */
float piguGetAxis(int controller, int axis);

/*
 * piguIsMouseButtonDown returns 1 if the button is pressed
 * otherwise 0. Buttons are numbered from 0.
 */
 int piguIsMouseButtonDown(int button);

/*
 * piguGetMousePosition returns the absolute mouse position
 * as accumulated since the beginning of the program
 * (not constrained to the screen).
 */
void piguGetMousePosition(int *x, int *y);

/*
 * piguGetMouseWheelPosition returns the absolute wheel
 * position as accumulated since the beginning of the program.
 */
int piguGetMouseWheelPosition();

/*
 * piguGetScreenSize returns the "physical" screen size as
 * reported by the firmware api can be called before
 * piguCreateWindow.
 */
int piguGetScreenSize(int *width, int *height);

/*
 * piguCreateWindow creates a windows with a GLES2 context
 * all parameters have to be supplied
 * red, gree, blue, alpha, depth and stencil are the
 * minimal amount of bits for each component
 * to get the native resolution of the screen call
 * piguGetScreenSize first.
 */
int piguCreateWindow(int width, int height, int red, int green, int blue, int alpha, int depth, int stencil, int samples);

/*
 * piguChangeResolution changes the resolution of the
 * GLES framebuffer while preserving the context.
 * Does not update Viewport and Scissor!
 */
int piguChangeResolution(int width, int height);

/*
 * piguCloseWindow closes the window and destroys the GLES
 * context
 */
void piguCloseWindow();

/*
 * piguSwapBuffers swaps the buffers ;)
 */
void piguSwapBuffers();

/*
 * piguSwapInterval set the swap interval (vsync)
 * 1 = one frame per refresh
 * 2 = one frame every second refresh
 * ...
 */
void piguSwapInterval(int interval);

/*
 * piguGetTime returns the time in seconds as measured from
 * the first call to piguGetTime
 */
double piguGetTime();

#ifdef __cplusplus
}
#endif

#endif
