#pragma once
#include "QFUIWND.h"
#include "QFReportForm.h"
#include "QFPictureBox.h"
#include "QFEdit.h"
#include "QFProgressBarCar.h"
#include "QFCATLCtrl.h"
#include "QFNormallyCtrl.h"
#include "QFPButton.h"


/**
*3D¿Ø¼þ¼ÓÔØ
* 
*/
//HMODULE g_hQFUI3D = NULL;
class RallHM
{
    HMODULE g_hm = NULL;
public:
    RallHM(const HMODULE& hm) { g_hm = hm; }
    RallHM() { }
    void operator =(HMODULE hm)
    {
        g_hm = hm;
    }
    bool operator ==(HMODULE hm)
    {
        return g_hm == hm;
    }
    operator HMODULE()
    {
        return g_hm ;
    }

    ~RallHM() 
    {
        if (g_hm)FreeLibrary(g_hm);
    
    }


};

static RallHM g_hQFUI3D = NULL;

void LoadQFUI3D();



#include "QFButton.h"
#include "QFLineChart.h"
#include "QFTabs.h"
//extern "C" __declspec(dllexport) QWND* CreateQWND(const char* cName,HDC winddc,int x,int y,int w,int h)




