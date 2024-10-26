#pragma once

#define DLL_EXPORTS
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/**
 * ==========================================================================================================================================================
 * 文件说明：       |   窗口处理函数头文件，主要从panel窗口接管窗口过程，根据panel窗口名称决定那个窗口，名称相同的默认第一个
 * ----------------+-----------------------------------------------------------------------------------------------------------------------------------------
 * 创建时间：       |   2023年11月17日
 * ----------------+-----------------------------------------------------------------------------------------------------------------------------------------
 * 备注：           |   只能TSMaster中使用
 * ==========================================================================================================================================================
*/

#include <windows.h>
#include <windowsx.h>
#include "QFUIWND.h"
#include <mutex>
#include<memory>

/**
 * 获取窗口绘图窗口句柄和器父窗口
 * 返回值返回绘图的句柄
 * 参数1：TSMaster的主程序窗口句柄
 * 参数2：要获取的窗口名称
 * 参数3：父窗口的句柄引用，用于返回取父窗口的句柄
*/
HWND GetPanelHandle(HWND& tsMainWin, const char* wName, HWND& paraHand, HWND& wHand);

class  AgentWndImport
{

    virtual BOOL BindPanel(HWND tsMw, const char* wName, LONG WndProc)=0;
    virtual HWND GetPanelHWND() = 0;
private:
    virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

public:
    virtual void UnBind() = 0;
    virtual inline QWND* BindQwnd(QWND* qw) = 0;

};



class  AgentWnd : public AgentWndImport
{
protected:
    LRESULT OldLongWinProck = NULL;
    LRESULT OldLongWinProc = NULL;
    LRESULT OldPanelProc = NULL;
    HWND hWndk = NULL;  //父窗口的句柄，因为按键消息都跑父窗口去了
    HWND hWnd = NULL;   //绘图窗口的句柄
    HWND hWin = NULL;   //panel的句柄
public:
    QWND* pwnd = NULL;

    bool isEraseBK = false;//是否擦除背景
    bool isDefaultPaintBK = false;//是否使用默认绘制背景

public:



    /**
     * 绑定窗口
     *
     *
     *
    */
     virtual BOOL BindPanel(HWND tsMw, const char* wName, LONG WndProc);
     virtual HWND GetPanelHWND() { return hWnd; }

    virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   
public:
    virtual void UnBind();
    virtual QWND* BindQwnd(QWND* qw);
    AgentWnd()
    {
        //MessageBoxA(NULL,"sadsdf","",0);
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
    }
    virtual ~AgentWnd()
    {

        UnBind();
        CoUninitialize();
    }

};




extern "C"
__declspec(dllexport) AgentWnd * __stdcall CreaterAgentWnd()
{
    return new AgentWnd();
}





extern "C"
__declspec(dllexport) BOOL __stdcall BindPanel(AgentWnd* aw,HWND tsMw, const char* wName, LONG WndProc)
{

    return aw->BindPanel(tsMw, wName, WndProc);
}
