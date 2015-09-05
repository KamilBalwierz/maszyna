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

std::string TPyInterpreter::getString(AnsiString string)
{
    return std::string(string.c_str());
}

PyObject *TPyInterpreter::getClass(const char *fileName, const char *className,
                                   std::list<std::string> lookupPath)
{
    std::string path;
    lookupPath.push_front(std::string("python\\local"));
    lookupPath.push_front(std::string("python"));
    lookupPath.push_front(std::string("python\\Lib\\site-packages"));
    lookupPath.push_front(std::string("python\\Lib"));
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
        for (std::list<std::string>::iterator i = lookupPath.begin(); i != lookupPath.end(); ++i)
        {
            printf("\tin %s\n", (*i).c_str());
        }
        logErr();
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
    {
        printf("Python error handling init: cannot find moudule cStringIO\n");
        return;
    }
    PyObject *cStringClassName = PyObject_GetAttrString(cStringModule, "StringIO");
    if (cStringClassName == NULL)
    {
        printf("Python error handling init: cannot find class cStringIO.cStringIO\n");
        return;
    }
    PyObject *cString = PyObject_CallObject(cStringClassName, NULL);
    if (cString == NULL)
    {
        printf("Python error handling init: cannot instantiate class cStringIO.cStringIO\n");
        return;
    }
    if (PySys_SetObject("stderr", cString) != 0)
    {
        printf("Python error handling init: cannot set StdErr\n");
        return;
    }
    _stdErr = cString;
}

void TPyInterpreter::logErr()
{
    if (_stdErr != NULL)
    {
        printf("Buffered python error output\n");
        PyErr_Print();
        PyObject *bufferContent = PyObject_CallMethod(_stdErr, "getvalue", NULL);
        PyObject_CallMethod(_stdErr, "truncate", "i", 0);
        printf("Python error:\n%s", PyString_AsString(bufferContent));
    }
    else
    {
        printf("Unbuffered python error output\n");
        if (PyErr_Occurred() != NULL)
        {
            PyObject *ptype, *pvalue, *ptraceback;
            PyErr_Fetch(&ptype, &pvalue, &ptraceback);
            if (ptype == NULL)
            {
                printf("Python: Don't konw how to handle NULL exception\n");
                return;
            }
            PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
            if (ptype == NULL)
            {
                printf("Python: Don't konw how to handle NULL exception");
            }
            PyObject *pStrType = PyObject_Str(ptype);
            if (pStrType != NULL)
            {
                printf("%s\n", PyString_AsString(pStrType));
            }
            printf("%s\n", PyString_AsString(pvalue));
            PyObject *pStrTraceback = PyObject_Str(ptraceback);
            if (pStrTraceback != NULL)
            {
                printf("%s\n", PyString_AsString(pStrTraceback));
            }
        }
    }
}
