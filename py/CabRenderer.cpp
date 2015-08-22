#include "py/CabRenderer.hpp"

#include <stdio.h>
#include <process.h>
#include <windows.h>

DWORD WINAPI ScreenRendererThread(LPVOID lpParam)
{
    TPyCabRenderer *renderer = (TPyCabRenderer *)lpParam;
    renderer->run();
    return true;
}

TPyCabRenderer::TPyCabRenderer(TTrain* train, int cabNo, char* rendererName) {
    _rendererName = rendererName;
    printf("Starting cab renderer %s\n", _rendererName);
    _thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScreenRendererThread, this, CREATE_SUSPENDED, &_threadId);
    if (_thread != NULL) {
        ResumeThread(_thread);
    }
}

TPyCabRenderer::~TPyCabRenderer() {
    printf("Destroing cab renderer %s\n", _rendererName);
    if(_thread != NULL) {
        TerminateThread(_thread, 1);
    }
}

void TPyCabRenderer::run() {
    int i = 0;
    while(1) {
        printf("Hello from thead %s for %d time\n", _rendererName, i++);
        Sleep(1000);
    }
}
