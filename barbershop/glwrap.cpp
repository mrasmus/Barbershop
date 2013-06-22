#include "glwrap.h"

HGLRC     hrc   = 0;
HDC       hdc   = 0;
HWND      hwnd  = 0;
HINSTANCE hinst = 0;

bool active     = true;
bool fullscreen = true;

GLvoid resizeglscene(GLsizei width, GLsizei height)
{ if(!height) height = 1;
  glViewport(0, 0, width, height);
  gluOrtho2D(0, width, height, 0);
}

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{ switch(msg)
  { case WM_ACTIVATE:
      active = !HIWORD(wp);
      return 0;

    case WM_SYSCOMMAND:
      switch(wp)
      { case SC_SCREENSAVE:
        case SC_MONITORPOWER:
        return 0;
      }
      break;

    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;

    case WM_SIZE:
      resizeglscene(LOWORD(lp), HIWORD(lp));
      return 0;
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

void killglwindow()
{ if(fullscreen)
  { ChangeDisplaySettings(0, 0);
    ShowCursor(true);
  }
  if(hrc)
  { if(!wglMakeCurrent(0, 0))  throw "Release of DC and RC failed";
    if(!wglDeleteContext(hrc)) throw "Release rendering context failed";
  }
  if(hdc && !ReleaseDC(hwnd, hdc)) throw "Release device context failed";
  if(hwnd && !DestroyWindow(hwnd)) throw "Couldn't release hwnd";
  if(!UnregisterClass(L"OpenGL", hinst)) throw "Couldn't unregister class";
}

bool createglwindow(char *title, int w, int h, int bits, bool fs)
{ RECT wr = { 0, 0, w, h };
  fullscreen = fs;
  hinst = GetModuleHandle(NULL);

  WNDCLASS wc;
  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = (WNDPROC)wndproc;
  wc.cbClsExtra		 = 0;
  wc.cbWndExtra		 = 0;
  wc.hInstance		 = hinst;
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = 0;
  wc.lpszMenuName  = 0;
  wc.lpszClassName = L"OpenGL";
  
  if(!RegisterClass(&wc)) throw "Couldn't register class";

  if(fullscreen)
  { DEVMODE ss;
    memset(&ss, 0, sizeof(ss));
    ss.dmSize = sizeof(ss);
    ss.dmPelsWidth  = w;
    ss.dmPelsHeight = h;
    ss.dmBitsPerPel = bits;
    ss.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    if(ChangeDisplaySettings(&ss, CDS_FULLSCREEN) !=
       DISP_CHANGE_SUCCESSFUL)
    { if(MessageBox(NULL,L"Can't go fullscreen.  Run in a window instead?",
                    L"Pop quiz", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
        return false;
      fullscreen = false;
    }
  }
  
  DWORD dwexstyle = fullscreen ? WS_EX_APPWINDOW
                               : (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
  DWORD dwstyle   = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;

  AdjustWindowRectEx(&wr, dwstyle, false, dwexstyle);

  hwnd = CreateWindowEx(dwexstyle, L"OpenGL", L"Test",
                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwstyle,
                        0, 0,
                        wr.right  - wr.left,
                        wr.bottom - wr.top,
                        0, 0, hinst, 0);
  try
  { if(!hwnd) throw "Couldn't create window";

    hdc = GetDC(hwnd);
    if(!hdc) throw "Couldn't create device context.";

    static PIXELFORMATDESCRIPTOR pfd =
    { sizeof(PIXELFORMATDESCRIPTOR), 1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      bits, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0,
      PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    GLuint pixelformat = ChoosePixelFormat(hdc, &pfd);
    if(!pixelformat) throw "Couldn't choose pixel format";

    if(!SetPixelFormat(hdc, pixelformat, &pfd))
      throw "Couldn't set pixel format";

    hrc = wglCreateContext(hdc);
    if(!hrc) throw "Couldn't create rendering context";

    if(!wglMakeCurrent(hdc, hrc))
      throw "Couldn't activate rendering context";

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    resizeglscene(w, h);
  }
  catch(const char *)
  { killglwindow();
    throw;
  }
    
  //if(!fullscreen) ShowCursor(false);

  return true;
}