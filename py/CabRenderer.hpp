#ifndef PyCabRendererHpp
#define PyCabRendererHpp

#include "Train.h"
#include <string>

class TPyCabRenderer
{
  public:
    TPyCabRenderer(TTrain *train, int cabNo, std::string moduleName, std::string className);
    ~TPyCabRenderer();
    void run();

  private:
    std::string _moduleName;
    std::string _className;
    HANDLE _thread;
    DWORD _threadId;
    TTrain *_train;
};

#endif
