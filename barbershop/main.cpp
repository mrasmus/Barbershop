#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>
#include "glwrap.h"
#include "listener.h"
#include "vect2.h"

float player_color[4][4] =
{
  {1.0, 0.5, 1.0, 1.0},
  {0.75, 1.0, 0.75, 1.0},
  {0.5, 1.0, 1.0, 1.0},
  {1.0, 1.0, 0.5, 1.0}
};

void drawBarberPole(int x, int y, int width, int height, float stripe_height,
  float stripe_gap, float stripe_slope, float stripe_offset,
  float target_offset, int player)
{
  glBindTexture(GL_TEXTURE_2D, 0);
  
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
  glColor4f(player_color[player][0]*0.6f, player_color[player][1]*0.6f, player_color[player][2]*0.6f, 0.75f);
  glVertex2f(x, y+height/2+wavelength/2-stripe_height+toffset);
  glVertex2f(x, y+height/2+wavelength/2+toffset);
  glColor4f(player_color[player][0], player_color[player][1], player_color[player][2], 0.75f);
  glVertex2f(x+width, y+height/2+wavelength/2-width*stripe_slope+toffset);
  glVertex2f(x+width, y+height/2+wavelength/2-stripe_height-width*stripe_slope+toffset);

  glEnd();

  glDisable(GL_SCISSOR_TEST);
}

int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width,
  unsigned long& image_height, const unsigned char* in_png, size_t in_size,
  bool convert_to_rgba32 = true);

int loadTexture(char *png_filename)
{
  FILE *in = fopen(png_filename, "rb");
  fseek(in, 0, SEEK_END);
  int size = ftell(in);
  fseek(in, 0, SEEK_SET);
  void *data = malloc(size);
  fread(data, size, 1, in);
  fclose(in);

  std::vector<unsigned char> rgba;
  unsigned long width, height;
  decodePNG(rgba, width, height, (const unsigned char *)data, size);
  free(data);

  int id;

  glGenTextures(1, (GLuint *)&id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    &(rgba.front()));

  return id;
}

void glVertexVect2(vect2f p)
{
  glVertex2f(p.x, p.y);
}

void drawDog(point p, float angle, int dogfur, int dogoverlay, int player)
{
  vect2f xside(128, 0), yside(0, 128);
  xside = xside.rotate(angle);
  yside = yside.rotate(angle);

  glBindTexture(GL_TEXTURE_2D, dogfur);
  glBegin(GL_QUADS);
  glColor4f(player_color[player][0], player_color[player][1], player_color[player][2], 1);
  glTexCoord2f(0, 0);
  glVertexVect2(p-xside/2-yside/2);
  glTexCoord2f(1, 0);
  glVertexVect2(p+xside/2-yside/2);
  glTexCoord2f(1, 1);
  glVertexVect2(p+xside/2+yside/2);
  glTexCoord2f(0, 1);
  glVertexVect2(p-xside/2+yside/2);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, dogoverlay);
  glBegin(GL_QUADS);
  glColor4f(1, 1, 1, 1);
  glTexCoord2f(0, 0);
  glVertexVect2(p-xside/2-yside/2);
  glTexCoord2f(1, 0);
  glVertexVect2(p+xside/2-yside/2);
  glTexCoord2f(1, 1);
  glVertexVect2(p+xside/2+yside/2);
  glTexCoord2f(0, 1);
  glVertexVect2(p-xside/2+yside/2);
  glEnd();
}

int main(int argc, char **argv)
{
  //fakeListenerInit();

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

  int dogfur = loadTexture("dogfur.png");
  int dogoverlay = loadTexture("dogoverlay.png");

  float position[4] = {50, 50, 50, 50};

  int time = 0;
  for(;;)
  {
    time++;

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

/*    float freq[4], amp[4];
    fakeListen(freq, amp);
    
    while(freq >= 220) freq /= 2;
    while(freq <  110) freq *= 2;

    printf("%f %f", freq, amp);*/

    offset = 55.0/110.0;

    for(int i=0; i<4; i++) position[i] += sin((float)i);

    drawBarberPole( 50, 50, 150, 500, 100, 100, 0.7, offset, 0.25, 0);
    drawBarberPole(250, 50, 150, 500, 100, 100, 0.7, offset, 0.25, 1);
    drawBarberPole(450, 50, 150, 500, 100, 100, 0.7, offset, 0.25, 2);
    drawBarberPole(650, 50, 150, 500, 100, 100, 0.7, offset, 0.25, 3);
    //offset += 0.01f;
    if(offset > 1) offset -= 1;

    drawDog(vect2f(position[0], 520), sin(time/10.0)/3, dogfur, dogoverlay, 0);
    drawDog(vect2f(position[1], 560), sin(time/8.0)/3, dogfur, dogoverlay, 1);
    drawDog(vect2f(position[2], 600), sin(time/12.0)/3, dogfur, dogoverlay, 2);
    drawDog(vect2f(position[3], 640), sin(time/11.0)/3, dogfur, dogoverlay, 3);

    glFinish();
    SwapBuffers(hdc);
  }
}