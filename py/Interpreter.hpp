#ifndef PyInterpreterHpp
#define PyInterpreterHpp

#include <Python.h>
#include <list>
#include <string>
#include <system.hpp>

class TPyInterpreter
{
  public:
    static TPyInterpreter *getInstance();
    static std::string getString(AnsiString string);
    PyObject *getClass(const char *fileName, const char *className,
                       std::list<std::string> lookupPath);

  protected:
    TPyInterpreter();
    ~TPyInterpreter();
    void registerStdErr();
    void logErr();
    PyObject *_stdErr;
    static TPyInterpreter *_instance;
};

#endif
