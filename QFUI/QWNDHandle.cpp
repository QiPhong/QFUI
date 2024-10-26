
#include "QWNDHandle.h"
#include <winuser.h>
/**
 * 获取窗口绘图窗口句柄和器父窗口
 * 返回值返回绘图的句柄
 * 参数1：要获取的窗口名称
 * 参数2：父窗口的句柄引用，用于返回取父窗口的句柄
*/
HWND GetPanelHandle(HWND& tsMainWin,const char* wName, HWND& paraHand, HWND& wHand)
{
    

    //从主窗口拿到 MDIClient句柄，这个窗口可能有多个
    HWND hMDIClient = NULL;
    while (true) {
        hMDIClient = FindWindowExA(tsMainWin, hMDIClient, "MDIClient", "");

        if (0 != hMDIClient) {
            //查找 TfrmPanel
            HWND hTfrmPanel = FindWindowExA(hMDIClient, 0, "TfrmPanel", wName);

            if (0 != hTfrmPanel) {
                wHand = hTfrmPanel;
                //查找TcxPageControl
                HWND hTcxPageControl = NULL;
                
                while (true) {
                    hTcxPageControl = FindWindowExA(hTfrmPanel, hTcxPageControl, "TcxPageControl", "");

                    if (0 != hTcxPageControl) {
                        //查找hTcxTabSheet
                        HWND hTcxTabSheet = FindWindowExA(hTcxPageControl, 0, "TcxTabSheet", "面板界面");
       
                        if (NULL != hTcxTabSheet) {
                            //查找TFireMonkeyContainer
                            HWND hTFireMonkeyContainer = FindWindowExA(hTcxTabSheet, 0, "TFireMonkeyContainer", "");
                          
                            if (NULL != hTFireMonkeyContainer) {
                                //查找FMTfrmPanelInterface
                                HWND hFMTfrmPanelInterface = FindWindowExA(hTFireMonkeyContainer, 0, "FMTfrmPanelInterface", "Panel");
                                if(NULL == hFMTfrmPanelInterface)hFMTfrmPanelInterface = FindWindowExA(hTFireMonkeyContainer, 0, "FMTfrmPanelInterface", wName);
                                paraHand = hTFireMonkeyContainer;
                     /*           std::string str = "hFMTfrmPanelInterface=" + std::to_string((int)hFMTfrmPanelInterface);
                                MessageBoxA(0, str.c_str(), "BindPanel", 0);*/
                                return hFMTfrmPanelInterface;
                            }
                            else return 0;

                        }
                        else continue;

                    }
                    else break;
                }
            }
            else break;


        }
        else break;

    }
    
    return HWND(0);
}




/**
 * 下边这个类是窗口接管的处理类，想要多个窗口在同一个小程序就多写几个一样的
 *
 *
*/


LRESULT AgentWnd::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

if (hwnd != hWin) {
    //非窗口消息
    if (pwnd) {
        int ret = pwnd->MsgHandle(uMsg, wParam, lParam);
        switch (ret) {//返回1截取消息
        case 1:
            return TRUE;
        case 2:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        }

    }
    if (!isEraseBK && WM_ERASEBKGND == uMsg)return DefWindowProc(hwnd, uMsg, wParam, lParam);
    if (!isDefaultPaintBK && WM_PAINT == uMsg)return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
else {
    //窗口消息
    switch (uMsg) {
        //要监听的消息
    case WM_SIZE:
        if (pwnd) {
            int ret = pwnd->MsgHandle(uMsg, wParam, lParam);
            switch (ret) {//返回1截取消息
            case 1:
                return TRUE;
            case 2:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);

            }
        }
        break;

    }

}

LRESULT wp = OldLongWinProc;
if (hwnd == hWndk) {
    wp = OldLongWinProck;
}
else if (hwnd == hWin) {
    wp = OldPanelProc;
}
if (!wp)return 1;
return(CallWindowProcA((WNDPROC)wp, hwnd, uMsg, wParam, lParam));

}

inline QWND* AgentWnd::BindQwnd(QWND* qw)
{
    QWND* ret = pwnd;
    qw->m_HostHwnd = hWnd;
    pwnd = qw;
    return ret;

}
void AgentWnd::UnBind()
{
    
    if (hWin) {
        
        if (OldPanelProc) {
            
            SetWindowLong(hWin,
                GWL_WNDPROC,
                (LONG)OldPanelProc);
        }
        hWin = NULL;

    }
   
    if (hWndk) {
        if (OldLongWinProck) {
            SetWindowLong(hWndk,
                GWL_WNDPROC,
                (LONG)OldLongWinProck);
        }

        hWndk = NULL;
    }
    
    if (hWnd) {
        if (OldLongWinProc) {
            SetWindowLong(hWnd,
                GWL_WNDPROC,
                (LONG)OldLongWinProc);
        }

        hWnd = NULL;
    }
   
}
extern "C"
__declspec(dllexport) void __stdcall DestroyAgentWnd(AgentWnd* aw)
{

    delete aw;
}
BOOL AgentWnd::BindPanel(HWND tsMw,const char* wName, LONG WndProc)
{
    AgentWnd::UnBind();
    MessageBoxA(0, " str.c_str()", "BindPanel", 0);
    hWnd = GetPanelHandle(tsMw,wName, hWndk, hWin);
 
   
    std::string str = "hWnd=" + std::to_string((int)hWnd) + ";hWndk=" + std::to_string((int)hWndk) + ";hWin=" + std::to_string((int)hWin);
    MessageBoxA(0, str.c_str(), "BindPanel", 0);
    if (!hWnd || !hWndk) {

        return false;

    }
 
    OldPanelProc = GetWindowLongPtr(this->hWin, GWL_WNDPROC);
   
    OldLongWinProc = GetWindowLongPtr(this->hWnd, GWL_WNDPROC);
    OldLongWinProck = GetWindowLongPtr(this->hWndk, GWL_WNDPROC);
   
    SetWindowLong(this->hWin,
        GWL_WNDPROC,
        WndProc);
    SetWindowLong(this->hWnd,
        GWL_WNDPROC,
        WndProc);

    SetWindowLong(this->hWndk,
        GWL_WNDPROC,
        WndProc);
    
    return true;

}
extern "C"
__declspec(dllexport) LONG __stdcall HookPanelPro(HWND hwnd, LONG WndProc)
{

    return  SetWindowLong(hwnd,
        GWL_WNDPROC,
        WndProc);
}
extern "C"
__declspec(dllexport) LONG __stdcall GetPanelPro(HWND hwnd)
{

    return  GetWindowLongPtr(hwnd,
        GWL_WNDPROC);
}
#include "pch.h"