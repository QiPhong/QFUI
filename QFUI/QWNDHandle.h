#pragma once

#define DLL_EXPORTS
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/**
 * ==========================================================================================================================================================
 * �ļ�˵����       |   ���ڴ�����ͷ�ļ�����Ҫ��panel���ڽӹܴ��ڹ��̣�����panel�������ƾ����Ǹ����ڣ�������ͬ��Ĭ�ϵ�һ��
 * ----------------+-----------------------------------------------------------------------------------------------------------------------------------------
 * ����ʱ�䣺       |   2023��11��17��
 * ----------------+-----------------------------------------------------------------------------------------------------------------------------------------
 * ��ע��           |   ֻ��TSMaster��ʹ��
 * ==========================================================================================================================================================
*/

#include <windows.h>
#include <windowsx.h>
#include "QFUIWND.h"
#include <mutex>
#include<memory>

/**
 * ��ȡ���ڻ�ͼ���ھ������������
 * ����ֵ���ػ�ͼ�ľ��
 * ����1��TSMaster�������򴰿ھ��
 * ����2��Ҫ��ȡ�Ĵ�������
 * ����3�������ڵľ�����ã����ڷ���ȡ�����ڵľ��
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
    HWND hWndk = NULL;  //�����ڵľ������Ϊ������Ϣ���ܸ�����ȥ��
    HWND hWnd = NULL;   //��ͼ���ڵľ��
    HWND hWin = NULL;   //panel�ľ��
public:
    QWND* pwnd = NULL;

    bool isEraseBK = false;//�Ƿ��������
    bool isDefaultPaintBK = false;//�Ƿ�ʹ��Ĭ�ϻ��Ʊ���

public:



    /**
     * �󶨴���
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
