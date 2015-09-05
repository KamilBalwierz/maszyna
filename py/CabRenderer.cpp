#include "py/CabRenderer.hpp"
#include "py/Interpreter.hpp"

#include <stdio.h>
#include <process.h>
#include <windows.h>

DWORD WINAPI ScreenRendererThread(LPVOID lpParam)
{
    TPyCabRenderer *renderer = (TPyCabRenderer *)lpParam;
    renderer->run();
    return true;
}

TPyCabRenderer::TPyCabRenderer(TTrain *train, int cabNo, std::string moduleName,
                               std::string className)
{
    _className = className;
    _moduleName = moduleName;
    _train = train;
    printf("Starting cab renderer %s.%s\n", _moduleName.c_str(), _className.c_str());
    _thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScreenRendererThread, this,
                           CREATE_SUSPENDED, &_threadId);
    if (_thread != NULL)
    {
        ResumeThread(_thread);
    }
}

TPyCabRenderer::~TPyCabRenderer()
{
    printf("Destroing cab renderer %s.%s\n", _moduleName.c_str(), _className.c_str());
    if (_thread != NULL)
    {
        TerminateThread(_thread, 0);
    }
}

void TPyCabRenderer::run()
{
    TPyInterpreter *instance = TPyInterpreter::getInstance();
    std::list<std::string> lookupPath;

    lookupPath.push_back(TPyInterpreter::getString(_train->Dynamic()->asBaseDir));
    PyObject *rendererClass =
        instance->getClass(_moduleName.c_str(), _className.c_str(), lookupPath);
    if (rendererClass == NULL)
    {
        _thread = NULL;
        ExitThread(0);
    }
    PyObject *renderer = PyInstance_New(rendererClass, NULL, NULL);
    if (renderer == NULL)
    {
        printf("ERROR: Python - cannot instantiate renderer class %s.%s\n", _moduleName.c_str(),
               _className.c_str());
        _thread = NULL;
        ExitThread(0);
    }
    int i = 0;
    while (1)
    {
        printf("Hello from thead %s.%s for %d time\n", _moduleName.c_str(), _className.c_str(),
               i++);
        PyObject_CallMethod(renderer, "hello", NULL);
        Sleep(1000);
    }
}
