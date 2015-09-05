#include "py/CabRenderersSet.hpp"
#include "py/CabRenderer.hpp"
#include "py/Interpreter.hpp"
#include "Train.h"

TPyCabRenderersSet::TPyCabRenderersSet()
{
}

TPyCabRenderersSet::~TPyCabRenderersSet()
{
    destroyRenderers();
}

void TPyCabRenderersSet::destroyRenderers()
{
    while (!_renderes.empty())
    {
        TPyCabRenderer *renderer = _renderes.front();
        delete renderer;
        _renderes.pop_front();
    }
}

void TPyCabRenderersSet::reInit(TTrain *train, int cabNo)
{
    destroyRenderers();
    _train = train;
    _cabNo = cabNo;
}

void TPyCabRenderersSet::add(TQueryParserComp *parser)
{
    std::string modelName = TPyInterpreter::getString(parser->GetNextSymbol());
    std::string moduleName = TPyInterpreter::getString(parser->GetNextSymbol());
    std::string className = TPyInterpreter::getString(parser->GetNextSymbol());
    // TODO: use parser to find and initialize proper renderer
    TPyCabRenderer *renderer = new TPyCabRenderer(_train, _cabNo, moduleName, className);
    _renderes.push_back(renderer);
}