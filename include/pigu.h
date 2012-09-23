#ifndef PIGU_H
#define PIGU_H

#ifdef __cplusplus
extern "C" {
#endif

int piguPollEvents();

int piguIsKeyDown(int keycode);

int piguIsControllerButtonDown(int controller, int button);

float piguGetAxis(int controller, int axis);

void piguGetMousePosition(int *x, int *y);

int piguGetMouseWheelPosition();

int piguInit();

int piguCreateWindow(int width, int height, int red, int green, int blue, int alpha, int depth, int stencil);

void piguSwapBuffers();

int piguGetScreenSize(int *width, int *height);

#ifdef __cplusplus
}
#endif

#endif
