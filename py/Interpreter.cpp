#include "py/Interpreter.hpp"

TPyInterpreter *TPyInterpreter::_instance = NULL;

TPyInterpreter::TPyInterpreter()
{
    Py_Initialize();
}

TPyInterpreter::~TPyInterpreter()
{
    Py_Finalize();
}

TPyInterpreter *TPyInterpreter::getInstance()
{
    if (TPyInterpreter::_instance == NULL)
    {
        TPyInterpreter::_instance = new TPyInterpreter();
    }
    return TPyInterpreter::_instance;
}
