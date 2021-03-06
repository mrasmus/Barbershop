#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>
#include <windows.h>
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

float notes[12] = { 110, 116.5, 123.5, 130.8, 138.6, 146.8, 155.5, 164.8, 174.6, 185, 196, 207.7 };
int chords[4][4] =
{
  { 0, 3,  7, 10 }, // CM
  { 3, 7, 10,  2 }, // Eb7
  { 8, 0,  3,  7 }, // Ab7
  { 1, 5,  8,  0 }  // Db7
};
const int sequence_length = 4;

static double hires_time()
{
  LARGE_INTEGER c, f;
  QueryPerformanceCounter  (&c);
  QueryPerformanceFrequency(&f);
  return double(c.QuadPart)/double(f.QuadPart);
}

void drawBarberPole(int x, int y, int width, int height, float stripe_height,
  float stripe_gap, float stripe_slope, float stripe_offset,
  float target_offset, int player, int stripe_texture)
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
  float offset = stripe_offset * wavelength - wavelength/2;

  for(int i=offset-wavelength*2; i<height+wavelength*2; i+=stripe_height+stripe_gap)
  {
    glColor3f(0.6f, 0.1f, 0.1f);
    glVertex2f(x, i+stripe_height);
    glVertex2f(x, i);
    glColor3f(1.0f, 0.2f, 0.2f);
    glVertex2f(x+width, i-width*stripe_slope);
    glVertex2f(x+width, i-width*stripe_slope+stripe_height);
  }

  float toffset = target_offset*wavelength - wavelength/2;
  glColor4f(player_color[player][0]*0.6f, player_color[player][1]*0.6f, player_color[player][2]*0.6f, 0.75f);
  glVertex2f(x, y+height/2+wavelength/2-stripe_height+toffset);
  glVertex2f(x, y+height/2+wavelength/2+toffset);
  glColor4f(player_color[player][0], player_color[player][1], player_color[player][2], 0.75f);
  glVertex2f(x+width, y+height/2+wavelength/2-width*stripe_slope+toffset);
  glVertex2f(x+width, y+height/2+wavelength/2-stripe_height-width*stripe_slope+toffset);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, stripe_texture);
  glBegin(GL_QUADS);
  glTexCoord2f(1, 0);
  glVertex2f(x, y+height/2+wavelength/2-stripe_height+toffset);
  glTexCoord2f(1, 1);
  glVertex2f(x, y+height/2+wavelength/2+toffset);
  glTexCoord2f(0, 1);
  glVertex2f(x+width, y+height/2+wavelength/2-width*stripe_slope+toffset);
  glTexCoord2f(0, 0);
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

void drawDog(point p, float angle, int texture, int player)
{
  vect2f xside(128, 0), yside(0, 128);
  xside = xside.rotate(angle);
  yside = yside.rotate(angle);

  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_QUADS);
  glColor4f(1, 1, 1, 1);
  glTexCoord2f(0, 0);
  glVertexVect2(p-xside/2-yside);
  glTexCoord2f(1, 0);
  glVertexVect2(p+xside/2-yside);
  glTexCoord2f(1, 1);
  glVertexVect2(p+xside/2);
  glTexCoord2f(0, 1);
  glVertexVect2(p-xside/2);
  glEnd();

/*  glBindTexture(GL_TEXTURE_2D, dogoverlay);
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
  glEnd();*/
}

struct Calibration
{
  float room_volume;
  int samples;
};

Calibration calibrations[4];

int main(int argc, char **argv)
{
  //fakeListenerInit();

  initListener();

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

  int textures[4];

  textures[0] = loadTexture("hulk.png");
  textures[1] = loadTexture("soup.png");
  textures[2] = loadTexture("oneway.png");
  textures[3] = loadTexture("cat.png");

  float position[4] = {50, 50, 50, 50};
  float target_offsets[4] = {0, 0, 0, 0};
  float pitches[4] = { 0, 0, 0, 0 }, amplitudes[4] = { 0, 0, 0, 0 };  
  float pitch_sans_octave[4] = { 110, 110, 110, 110 };

  float time = 0;

  bool running = true;
  while(running)
  {
    time = hires_time();

#ifndef __APPLE__
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
#endif

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width,height,0,-1,1);  //equiv to below gluortho2d call
    //gluOrtho2D(0, width, height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bool singing[4] = { false, false, false, false };

    for(int i=0; i<4; i++)
    {
      /*if(calibrations[i].samples < 50)
      {
        calibrations[i].room_volume += playerData[i].amplitude;
        if(++calibrations[i].samples >= 50)
          calibrations[i].room_volume /= 50;
      }
      else*/
      float thresholds[4] = { 0.08, 0.005, 0.05, 0.005 };
      {
        amplitudes[i] = playerData[i].amplitude; // / calibrations[i].room_volume;
        if(amplitudes[i] > thresholds[i])
        {
          singing[i] = true;
          float new_pitch = playerData[i].frequency;
          while(fabs(new_pitch/2-pitches[i]) < fabs(new_pitch-pitches[i])) new_pitch/=2;
          while(fabs(new_pitch*2-pitches[i]) < fabs(new_pitch-pitches[i])) new_pitch*=2;
          pitches[i] = new_pitch*0.1 + pitches[i]*0.9;
          printf("%2.1f %2.1f %2.1f %2.1f\t", pitches[0], pitches[1], pitches[2], pitches[3]);

          pitch_sans_octave[i] = pitches[i];
          if(pitch_sans_octave[i] <= 0) pitch_sans_octave[i] = 110;
          while(pitch_sans_octave[i] <  110) pitch_sans_octave[i] *= 2;
          while(pitch_sans_octave[i] >= 220) pitch_sans_octave[i] /= 2;
          printf("%2.1f %2.1f %2.1f %2.1f\n", pitch_sans_octave[0], pitch_sans_octave[1], pitch_sans_octave[2], pitch_sans_octave[3]);
        }
      }
    }

    float offsets[4];
    for(int i=0; i<4; i++)
    {
      offsets[i] = (pitch_sans_octave[i]-110)/110;
    }

    for(int i=0; i<4; i++)
    {
      int chord = (int)(time / 5.0)%sequence_length;
      target_offsets[i] = target_offsets[i]*0.9 + ((notes[chords[chord][i]] - 110) / 110)*0.1;
    }

    float distances[4];
    for(int i=0; i<4; i++)
    {
      float a = fabs(offsets[i]-target_offsets[i]+0.5);
      float b = fabs(offsets[i]-target_offsets[i]+1.5);
      float c = fabs(offsets[i]-target_offsets[i]-0.5);
      distances[i] = min(a, min(b, c));
    }
    for(int i=0; i<4; i++) if(singing[i]) position[i] += (1-distances[i])/2;

    drawBarberPole(150, 50, 150, 500, 100, 100, 0.7, offsets[0], target_offsets[0], 0, textures[0]);
    drawBarberPole(350, 50, 150, 500, 100, 100, 0.7, offsets[1], target_offsets[1], 1, textures[1]);
    drawBarberPole(550, 50, 150, 500, 100, 100, 0.7, offsets[2], target_offsets[2], 2, textures[2]);
    drawBarberPole(750, 50, 150, 500, 100, 100, 0.7, offsets[3], target_offsets[3], 3, textures[3]);
    //offset += 0.01f;
    if(offset > 1) offset -= 1;

    drawDog(vect2f(position[0], 620-fabs(sin(time*6))*10), singing[0]?(sin(time*4.0)/2):0, textures[0], 0);
    drawDog(vect2f(position[1], 660-fabs(sin(time*9))*10), singing[1]?(sin(time*4.0)/2):0, textures[1], 1);
    drawDog(vect2f(position[2], 700-fabs(sin(time*8))*10), singing[2]?(sin(time*4.0)/2):0, textures[2], 2);
    drawDog(vect2f(position[3], 740-fabs(sin(time*12))*10), singing[3]?(sin(time*4.0)/2):0, textures[3], 3);

    //printf("Test");
    glFinish();
#ifdef __APPLE__
    glfwSwapBuffers();
    if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS 
      || !glfwGetWindowParam(GLFW_OPENED))
    {
      running = false;
    }
#elif WIN32
    SwapBuffers(hdc);
#endif
  }
  killglwindow();
}
