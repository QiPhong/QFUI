
#include "QFUI3D.h"
#include "QFGraph3D.h"

extern "C" 
__declspec(dllexport) QWND * CreateQWND3D(const char* cName, HDC winddc, GRect rf)
{
    //GRect rf(x,y,w,h);
    std::string nm(cName);
    if ("QF3DCtrl" == nm) {

        QWND* p = new Q3DCtrl(winddc, rf);

        return p;
    }
    else if ("QF3DGraph" == nm) {
        
        return new QGraph3D(winddc, rf);
    }
    
   
    return NULL;
}