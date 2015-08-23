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

TPyCabRenderer::TPyCabRenderer(TTrain *train, int cabNo, char *rendererName)
{
    _rendererName = rendererName;
    printf("Starting cab renderer %s\n", _rendererName);
    _thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScreenRendererThread, this,
                           CREATE_SUSPENDED, &_threadId);
    if (_thread != NULL)
    {
        ResumeThread(_thread);
    }
}

TPyCabRenderer::~TPyCabRenderer()
{
    printf("Destroing cab renderer %s\n", _rendererName);
    if (_thread != NULL)
    {
        TerminateThread(_thread, 0);
    }
}

void TPyCabRenderer::run()
{
    TPyInterpreter *instance = TPyInterpreter::getInstance();
    std::list<std::string> lookupPath;
    lookupPath.push_back(std::string("python"));
    PyObject *rendererClass = instance->getClass("osiem", "Testowa", lookupPath);
    if (rendererClass == NULL)
    {
        _thread = NULL;
        ExitThread(0);
    }
    PyObject *renderer = PyInstance_New(rendererClass, NULL, NULL);
    if (renderer == NULL)
    {
        printf("ERROR: Python - cannot instantiate renderer class\n");
        _thread = NULL;
        ExitThread(0);
    }
    int i = 0;
    while (1)
    {
        printf("Hello from thead %s for %d time\n", _rendererName, i++);
        PyObject_CallMethod(renderer, "hello", NULL);
        Sleep(1000);
    }
}
