#include "py/Interpreter.hpp"
#include <stdio.h>

TPyInterpreter *TPyInterpreter::_instance = NULL;

TPyInterpreter::TPyInterpreter()
{
    Py_SetPythonHome("python");
    Py_Initialize();
    PyObject *main = PyImport_ImportModule("__main__");
    if (main == NULL)
    {
        printf("ERROR: Cannot initialize Python Interpreter\n");
    }
    else
    {
        printf("Python interpreter started\n");
        registerStdErr();
    }
}

TPyInterpreter::~TPyInterpreter()
{
    Py_Finalize();
}

PyObject *TPyInterpreter::getClass(char *fileName, char *className,
                                   std::list<std::string> lookupPath)
{
    std::string path;

    for (std::list<std::string>::iterator i = lookupPath.begin(); i != lookupPath.end(); ++i)
    {
        if (i != lookupPath.begin())
        {
            path.append(";");
        }
        path.append(*i);
    }
    char *cpath = new char[path.length() + 1];
    strcpy(cpath, path.c_str());

    PySys_SetPath(cpath);

    PyObject *module = PyImport_ImportModule(fileName);
    if (module != NULL)
    {
        PyObject *classDefiniton = PyObject_GetAttrString(module, className);
        if (classDefiniton != NULL)
        {
            return classDefiniton;
        }
        else
        {
            printf("ERROR: Python - class %s definition not found\n", className);
        }
    }
    else
    {
        printf("ERROR: Python - file %s.py not found\n", fileName);
    }
    return NULL;
}

TPyInterpreter *TPyInterpreter::getInstance()
{
    if (TPyInterpreter::_instance == NULL)
    {
        TPyInterpreter::_instance = new TPyInterpreter();
    }
    return TPyInterpreter::_instance;
}

void TPyInterpreter::registerStdErr()
{
    PyObject *cStringModule = PyImport_ImportModule("cStringIO");
    _stdErr = NULL;
    if (cStringModule == NULL)
        return;
    PyObject *cStringClassName = PyObject_GetAttrString(cStringModule, "StringIO");
    if (cStringClassName == NULL)
        return;
    PyObject *cString = PyObject_CallObject(cStringClassName, NULL);
    if (cString == NULL)
        return;
    if (PySys_SetObject("stderr", cString) != 0)
        return;
}

void TPyInterpreter::logErr()
{
}
