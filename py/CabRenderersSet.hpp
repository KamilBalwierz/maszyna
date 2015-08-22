#ifndef PyCabRenderersSetHpp
#define PyCabRenderersSetHpp

class TTrain;
class TPyCabRenderer;

#include <list>
#include "QueryParserComp.hpp"

class TPyCabRenderersSet {
public:
    TPyCabRenderersSet();
    ~TPyCabRenderersSet();
    void reInit(TTrain* train, int cabNo);
    void add(TQueryParserComp* parser);
private:
    void destroyRenderers();
    std::list<TPyCabRenderer*> _renderes;
    TTrain* _train;
    int _cabNo;
};

#endif
