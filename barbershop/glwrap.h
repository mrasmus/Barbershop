#ifndef _GLWRAP_H
#define _GLWRAP_H


#ifdef __APPLE__
#include <GL/glfw.h>
#elif
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

extern HGLRC     hrc;
extern HDC       hdc;
extern HWND      hwnd;
extern HINSTANCE hinst;
#endif

extern bool active;

bool createglwindow(char *title, int w, int h, int bits, bool fs);
void killglwindow();

#endif
