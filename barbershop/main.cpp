#include <stdio.h>
#include "glwrap.h"

void drawBarberPole(int x, int y, int width, int height, float stripe_height,
  float stripe_gap, float stripe_slope, float stripe_offset,
  float target_offset)
{
  //stripe_offset = 0.5f;

  glScissor(x, 768-height-y, width, height);
  glEnable(GL_SCISSOR_TEST);

  glBegin(GL_QUADS);
  glColor3f(0.6f, 0.6f, 0.6f);
  glVertex2f(x, y);
  glVertex2f(x, y+height);
  glColor3f(0.9f, 0.9f, 0.9f);
  glVertex2f(x+width, y+height);
  glVertex2f(x+width, y);

  float wavelength = stripe_height+stripe_gap;
  float offset = stripe_offset * wavelength;

  for(int i=offset-wavelength*2; i<height+wavelength*2; i+=stripe_height+stripe_gap)
  {
    glColor3f(0.6f, 0.1f, 0.1f);
    glVertex2f(x, i+stripe_height);
    glVertex2f(x, i);
    glColor3f(1.0f, 0.2f, 0.2f);
    glVertex2f(x+width, i-width*stripe_slope);
    glVertex2f(x+width, i-width*stripe_slope+stripe_height);
  }

  float toffset = target_offset*wavelength;
  glColor4f(0.1f, 0.1f, 0.6f, 0.5f);
  glVertex2f(x, y+height/2+wavelength/2-stripe_height+toffset);
  glVertex2f(x, y+height/2+wavelength/2+toffset);
  glColor4f(0.2f, 0.2f, 1.0f, 0.5f);
  glVertex2f(x+width, y+height/2+wavelength/2-width*stripe_slope+toffset);
  glVertex2f(x+width, y+height/2+wavelength/2-stripe_height-width*stripe_slope+toffset);

  glEnd();
}

int main(int argc, char **argv)
{
  int width = 1024, height = 768;
  createglwindow("Test", width, height, 32, false);
 
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glViewport(0, 0, width, height);

  float offset = 0;

  for(;;)
  {
    MSG msg;
    if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    { 
      if(msg.message==WM_QUIT) break;
      else
      { 
        TranslateMessage(&msg);
        DispatchMessage (&msg);
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawBarberPole(50, 50, 150, 500, 100, 100, 0.7, offset, 0.25);
    offset += 0.01f;
    if(offset > 1) offset -= 1;

    glFinish();
    SwapBuffers(hdc);
  }
}