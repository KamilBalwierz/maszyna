#ifndef PyCabRendererHpp
#define PyCabRendererHpp

#include "Train.h"

class TPyCabRenderer
{
  public:
    TPyCabRenderer(TTrain *train, int cabNo, char *rendererName);
    ~TPyCabRenderer();
    void run();

  private:
    char *_rendererName;
    HANDLE _thread;
    DWORD _threadId;
};

#endif
