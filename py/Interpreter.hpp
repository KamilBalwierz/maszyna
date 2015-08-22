#ifndef PyInterpreterHpp
#define PyInterpreterHpp

#include <Python.h>

class TPyInterpreter
{
  public:
    static TPyInterpreter *getInstance();

  protected:
    TPyInterpreter();
    ~TPyInterpreter();
    static TPyInterpreter *_instance;
};

#endif
