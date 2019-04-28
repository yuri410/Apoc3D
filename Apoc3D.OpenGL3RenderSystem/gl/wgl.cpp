#include "wgl.h"

WGLCREATECONTEXTATTRIBSARB wglCreateContextAttribsARB;
WGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;
WGLSWAPINTERVALEXT wglSwapIntervalEXT;

bool wglInit()
{
	wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARB)gl3wGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARB)gl3wGetProcAddress("wglChoosePixelFormatARB");
	wglSwapIntervalEXT = (WGLSWAPINTERVALEXT)gl3wGetProcAddress("wglSwapIntervalEXT");

	return wglCreateContextAttribsARB && wglChoosePixelFormatARB;
}