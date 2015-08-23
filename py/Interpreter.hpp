#ifndef PyInterpreterHpp
#define PyInterpreterHpp

#include <Python.h>
#include <list>
#include <string>

class TPyInterpreter
{
  public:
    static TPyInterpreter *getInstance();
    PyObject *getClass(char *fileName, char *className, std::list<std::string> lookupPath);

  protected:
    TPyInterpreter();
    ~TPyInterpreter();
    void registerStdErr();
    void logErr();
    PyObject *_stderr;
    static TPyInterpreter *_instance;
};

#endif
