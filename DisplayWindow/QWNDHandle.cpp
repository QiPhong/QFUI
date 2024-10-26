#include "QWNDHandle.h"
#include <map>
#include <shared_mutex>
AgentWndDLL* AgentWndDLL::Instance = NULL;
std::mutex AgentWndDLL::m_Mutex;
namespace __Mangement {

    __AgentWndPtrMangement<AgentWndDLL> awpm;

}
namespace QWNDHANDLE {
    LONG OldLongWinProck = NULL;
    LONG OldLongWinProc = NULL;
    LONG OldPanelProc = NULL;
    int hWndk = NULL;
    int hWnd = NULL;
    int hWin = NULL;

    bool isEraseBK = false;
    bool isDefaultPaintBK = false;
    HDC wdc = NULL;
    DLLImportQWND* qwm = NULL;
    std::set<QWND*> QWNDs;      //���洴���Ŀؼ�

    std::shared_mutex eventLK;//�����¼���������
    std::map<std::string, QPOINTER> EventFun;//�ؼ���Ӧ�¼�����


}
LRESULT PanelWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int hd = (int)hwnd;
    DLLImportQWND* pwnd = QWNDHANDLE::qwm;
    if (hd != QWNDHANDLE::hWin) {

        if (pwnd) {
            int ret = pwnd->MsgHandle(uMsg, wParam, lParam);
            switch (ret) {
            case 1:
                return TRUE;
            case 2:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }
        if (!QWNDHANDLE::isEraseBK && WM_ERASEBKGND == uMsg)return DefWindowProc(hwnd, uMsg, wParam, lParam);
        if (!QWNDHANDLE::isDefaultPaintBK && WM_PAINT == uMsg)return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    else {
        switch (uMsg) {
        case WM_SIZE:
            if (pwnd) {
                int ret = pwnd->MsgHandle(uMsg, wParam, lParam);
                switch (ret) {
                case 1:
                    return TRUE;
                case 2:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);

                }
            }
            break;

        }

    }

    LONG wp = QWNDHANDLE::OldLongWinProc;
    if (hd == QWNDHANDLE::hWndk) {
        wp = QWNDHANDLE::OldLongWinProck;
    }
    else if (hd == QWNDHANDLE::hWin) {
        wp = QWNDHANDLE::OldPanelProc;
    }
    if (!wp)return 1;
#ifndef STRICT

    return(CallWindowProcA((FARPROC)wp, hwnd, uMsg, wParam, lParam));
#else
    return(CallWindowProcA((WNDPROC)wp, hwnd, uMsg, wParam, lParam));
#endif 




}
LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int hd = (int)hwnd;
    DLLImportQWND* pwnd = QWNDHANDLE::qwm;



    if (pwnd) {
        int ret = pwnd->MsgHandle(uMsg, wParam, lParam);
        switch (ret) {
        case 1:
            return TRUE;
        case 2:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        }
    }





    LONG wp = QWNDHANDLE::OldLongWinProc;

    if (!wp)return 1;
#ifndef STRICT

    return(CallWindowProcA((FARPROC)wp, hwnd, uMsg, wParam, lParam));
#else
    return(CallWindowProcA((WNDPROC)wp, hwnd, uMsg, wParam, lParam));
#endif 




}
QPOINTER FindEventFun(const char* name)
{
    std::shared_lock<std::shared_mutex> lk(QWNDHANDLE::eventLK);
    auto it = QWNDHANDLE::EventFun.find(name);
    if (it == QWNDHANDLE::EventFun.end())return 0;

    return it->second;
}
int RegEventFun(const char* name, QPOINTER fun)
{
    std::unique_lock<std::shared_mutex> lk(QWNDHANDLE::eventLK);
    int ret = 1;
    auto it = QWNDHANDLE::EventFun.find(name);
    if (it != QWNDHANDLE::EventFun.end())ret =2;
    QWNDHANDLE::EventFun[name] = fun;
    return ret;
}
AgentWndDLL::AgentWndDLL(const char* path)
{

    m_hm = LoadLibraryA(path);
    if (!m_hm)DebugOut("����DLLʧ��");

}

AgentWndDLL::~AgentWndDLL()
{
    if (m_hm) {

        SETPANELPRO sp = (SETPANELPRO)GetProcAddress(m_hm, "HookPanelPro");
        if (NULL != sp) {

            //sp(QWNDHANDLE::hWndk,(LONG)PanelWndProc);
            //sp(QWNDHANDLE::hWnd,(LONG)PanelWndProc);
            //sp(QWNDHANDLE::hWin,(LONG)PanelWndProc);

            if (QWNDHANDLE::hWin) {
                if (QWNDHANDLE::OldPanelProc) {
                    sp(QWNDHANDLE::hWin,
                        QWNDHANDLE::OldPanelProc);

                }
                QWNDHANDLE::hWin = NULL;

            }

            if (QWNDHANDLE::hWndk) {
                if (QWNDHANDLE::OldLongWinProck) {
                    sp(QWNDHANDLE::hWndk,
                        QWNDHANDLE::OldLongWinProck);
                }

                QWNDHANDLE::hWndk = NULL;
            }
            if (QWNDHANDLE::hWnd) {
                if (QWNDHANDLE::OldLongWinProc) {
                    sp(QWNDHANDLE::hWnd,
                        QWNDHANDLE::OldLongWinProc);
                }

                QWNDHANDLE::hWnd = NULL;
            }
        }
        typedef void(__stdcall* DestroyAWnd)(int);
        DestroyAWnd daw = (DestroyAWnd)GetProcAddress(m_hm, "DestroyAgentWnd");
        if (m_awi && daw) {
            daw((int)m_awi);
        }

    }
    void* dfun = NULL;
    typedef void(*DQWND)(QWND*);
    if (m_hm) {

        dfun = GetProcAddress(m_hm, "DeleteQWND");

    }

    if (QWNDHANDLE::qwm) {
        //if (dfun) {
        //    DQWND dwnd = (DQWND)dfun;
        //    dwnd(QWNDHANDLE::qwm->qwm);
        //}
        delete QWNDHANDLE::qwm;
    }
    //����ؼ�


    for (auto it : QWNDHANDLE::QWNDs) {

        if (dfun) {
            DQWND dwnd = (DQWND)dfun;
            dwnd(((DLLImportQWND*)it)->qwm);
        }

        delete it;
    }


    if (QWNDHANDLE::wdc)ReleaseDC(0, QWNDHANDLE::wdc);
    if (m_hm) {
        FreeLibrary(m_hm);
        m_hm = NULL;
    }

}
AgentWndDLL* AgentWndDLL::GetInstance(const char* path)
{
    if (Instance == nullptr)
    {
        if (NULL == path) {
            DebugOut("AgentWndDLL::GetInstance: ʧ�ܣ��봫��DLL·��");
            return NULL;
        }
        std::unique_lock<std::mutex> lock(m_Mutex); // ����
        if (Instance == nullptr)
        {
            volatile auto temp = new (std::nothrow) AgentWndDLL(path);
            Instance = temp;
            __Mangement::awpm.ptr = Instance;

        }
    }

    return Instance;

}

int AgentWndDLL::BindPanel(const char* wName)
{
#ifdef __START_TSMASTER
    if (m_hm) {
        static CreaterAW caw = (CreaterAW)GetProcAddress(m_hm, "CreaterAgentWnd");
        if (NULL == caw)return 2;
        typedef BOOL __stdcall (*BINDPANEL)(AgentWndImport*, HWND, const char*, LONG);
        m_awi = caw();
        if (NULL == m_awi)return 3;
        GETPANELH gph = (GETPANELH)GetProcAddress(m_hm, "GetPanelHandle");
        if (NULL == gph)return 4;
        s32 h;
        app.get_main_window_handle(&h);
        QWNDHANDLE::hWnd = gph(h, wName, QWNDHANDLE::hWndk, QWNDHANDLE::hWin);

        if (NULL == QWNDHANDLE::hWnd) {
            DebugOut("��panelʧ��5");
            return 5;
        }


        GETPANELPRO gp = (GETPANELPRO)GetProcAddress(m_hm, "GetPanelPro");
        if (NULL == gp)return 6;
        QWNDHANDLE::OldLongWinProck = gp(QWNDHANDLE::hWndk);
        QWNDHANDLE::OldLongWinProc = gp(QWNDHANDLE::hWnd);
        QWNDHANDLE::OldPanelProc = gp(QWNDHANDLE::hWin);


        SETPANELPRO sp = (SETPANELPRO)GetProcAddress(m_hm, "HookPanelPro");
        if (NULL == sp)return 7;

        sp(QWNDHANDLE::hWndk, (LONG)PanelWndProc);
        sp(QWNDHANDLE::hWnd, (LONG)PanelWndProc);
        sp(QWNDHANDLE::hWin, (LONG)PanelWndProc);

        return 1;
    }
    else return 0;

#else

#endif

    return 0;
}
int AgentWndDLL::BindPanel(HWND mainHwnd)
{
    if (m_hm) {
        static CreaterAW caw = (CreaterAW)GetProcAddress(m_hm, "CreaterAgentWnd");
        if (NULL == caw)return 2;
        m_awi = caw();
        if (NULL == m_awi)return 3;

        QWNDHANDLE::hWnd = (int)mainHwnd;

        GETPANELPRO gp = (GETPANELPRO)GetProcAddress(m_hm, "GetPanelPro");
        if (NULL == gp)return 6;
        QWNDHANDLE::OldLongWinProc = gp(QWNDHANDLE::hWnd);



        SETPANELPRO sp = (SETPANELPRO)GetProcAddress(m_hm, "HookPanelPro");
        if (NULL == sp)return 7;
        sp(QWNDHANDLE::hWnd, (LONG)WndProc);


        return 1;
    }
    else return 0;

}

QWMange* AgentWndDLL::CreateQWMange()
{
    if (QWNDHANDLE::qwm)return (QWMange*)QWNDHANDLE::qwm;

    if (m_hm) {
        typedef QWND* (*CQWDM)(HDC winddc, QRect  rf);
        CQWDM cqwdm = (CQWDM)GetProcAddress(m_hm, "CreateQWMange");

        HWND hwnd = HWND(QWNDHANDLE::hWnd);
        QWNDHANDLE::wdc = GetDC(hwnd);
        RECT rect;
        GetClientRect(hwnd, &rect);
        QWND* p = cqwdm(QWNDHANDLE::wdc, QRect(0, 0, rect.right - rect.left, rect.bottom - rect.top));
        QWNDHANDLE::qwm = new QWMange(QWNDHANDLE::wdc, QRect(0, 0, rect.right - rect.left, rect.bottom - rect.top));

        QWMange* pm = (QWMange*)QWNDHANDLE::qwm;


        pm->qwm = p;
        pm->init(m_hm);
        pm->SetHostHand(hwnd);
        return pm;

    }


    return NULL;
}
#include "QFProgressBarCar.h"
#include "QFButton.h"
#include "QFLineChart.h"
#include "QFTabs.h"
#include "QF3DCtrl.h"
#include "QFCATLCtrl.h"
#include "QFFrequentlyCtrl.h"
QWND* AgentWndDLL::CreateQWND(const char* name, HDC dc, const QRect& rf)
{

    if (m_hm) {
        typedef QWND* (*CQWND)(const char*, HDC, QRect);
        CQWND cwnd = (CQWND)GetProcAddress(m_hm, "CreateQWND");
        if (NULL == cwnd) {
            DebugOut("AgentWndDLL::CreateQWND �����ں��� <CreateQWND> ��ַ�Ҳ����������ؼ�ʧ�ܣ� ");
            return NULL;
        }

        HWND hwnd = HWND(QWNDHANDLE::hWnd);
        if (NULL == QWNDHANDLE::wdc)
            QWNDHANDLE::wdc = GetDC(hwnd);

        DLLImportQWND* dq = NULL;
        std::string cn = name;
        if ("QFButton" == cn) {
            dq = new QButton(QWNDHANDLE::wdc, rf);

        }
        else if ("QFReportForm" == cn) {
            dq = new QReportForm(QWNDHANDLE::wdc, rf);
        }
        else if ("QFPictureBox" == cn) {
            dq = new QPictureBox(QWNDHANDLE::wdc, rf);
        }
        else if ("QFEdit" == cn) {
            dq = new QEdit(QWNDHANDLE::wdc, rf);
        }
        else if ("QFPanelDial" == cn) {

        }
        else if ("QFProgressBarCar" == cn) {
            dq = new QProgressBarCar(QWNDHANDLE::wdc, rf);
        }
        else if ("QFLineChart" == cn) {
            dq = new QLineChart(QWNDHANDLE::wdc, rf);
        }
        else if ("QFTabs" == cn) {
            dq = new QTabs(QWNDHANDLE::wdc, rf);
        }
        else if ("QFCATLBarChart" == cn) {
            dq = new QCATLBarChart(QWNDHANDLE::wdc, rf);
        }
        else if ("QFCATLTempeChart" == cn) {
            dq = new QCATLTempeChart(QWNDHANDLE::wdc, rf);
        }
        else if ("QFDrawer" == cn) {
            dq = new QDrawer(QWNDHANDLE::wdc, rf);
        }
        else if ("QFHostCtrl" == cn) {

            dq = new QHost(QWNDHANDLE::wdc, rf);

        }
        else if ("QFHLayout" == cn) {

            dq = new QHLayout(QWNDHANDLE::wdc, rf);

        }
        else if ("QFVLayout" == cn) {

            dq = new QVLayout(QWNDHANDLE::wdc, rf);


        }
        else if ("QFTLayout" == cn) {

            dq = new QTLayout(QWNDHANDLE::wdc, rf);

        }
        else if ("QFStaticText" == cn) {

            dq = new QStaticText(QWNDHANDLE::wdc, rf);

        }
        else if ("QFSwitch" == cn) {
        
            dq = new QSwitch(QWNDHANDLE::wdc, rf);
        }
        else if ("QFPButton" == cn) {

            dq = new QButton2(QWNDHANDLE::wdc, rf);
        }
        else if ("QFSwitch" == cn) {

            dq = new QSwitch(QWNDHANDLE::wdc, rf);
        }
        else if ("QFSlider" == cn) {

            dq = new QSlider(QWNDHANDLE::wdc, rf);
        }
        else if ("QFProgress" == cn) {

            dq = new QProgress(QWNDHANDLE::wdc, rf);
        }
        else if ("QFColorBox" == cn) {

            dq = new QColorBox(QWNDHANDLE::wdc, rf);
        }
        else if ("QFSelect" == cn) {

            dq = new QSelect(QWNDHANDLE::wdc, rf);
        }
        else {
            dq = new DLLImportQWND(QWNDHANDLE::wdc, rf);
        
        }

        

        if ("QF3D" == cn.substr(0, 4)) {

            if ("QF3DCtrl" == cn) {
                dq = new Q3DCtrl(QWNDHANDLE::wdc, rf);
            }
            else if ("QF3DGraph" == cn) {
                dq = new QGraph3D(QWNDHANDLE::wdc, rf);
            }
        }
        else if ("QFEDR" == cn.substr(0, 5)) {//ר��ΪUI�༭������
            if ("QFEDR_CtrlToolBox" == cn) {

                dq = new QCtrlToolbox(QWNDHANDLE::wdc, rf);
            }
            else if ("QFEDR_EditorWin" == cn) {
                dq = new QEditorWin(QWNDHANDLE::wdc, rf);

            }

        }


        if (NULL == dq) {
            DebugOut("AgentWndDLL::CreateQWND �������ؼ�ʧ��,���� ��%s���ؼ���֧�ֵ����� ", name);
            return NULL;
        }

        QWND* p = cwnd(name, QWNDHANDLE::wdc, rf);
        if (NULL == p) {
            DebugOut("AgentWndDLL::CreateQWND �������ؼ�ʧ��,���� ��%s���ؼ������� ", name);
            return NULL;
        }

        dq->qwm = p;
        dq->init(m_hm);
        QWNDHANDLE::QWNDs.insert(dq);

        return dq;
    }

    DebugOut("AgentWndDLL::CreateQWND ������ <QFUI> DLL��ģ����Ϊ0�������ؼ�ʧ�ܣ� ");
    return NULL;
}


QWND* AgentWndDLL::BindQwnd(DLLImportQWND* qwnd)
{
    QWND* ret = QWNDHANDLE::qwm;
    qwnd->SetHostHand((HWND)QWNDHANDLE::hWnd);
    QWNDHANDLE::qwm = qwnd;
    return ret;


}

QBrush* AgentWndDLL::CreateSolidBrush(unsigned char A, unsigned char R, unsigned char G, unsigned char B)
{
    typedef QBrush* (*CREATESOLIDBRUSH)(unsigned char A, unsigned char R, unsigned char G, unsigned char B);
    if (m_hm) {
        CREATESOLIDBRUSH cs = (CREATESOLIDBRUSH)GetProcAddress(m_hm, "?QCreateSolidBrush@@YAPAVBrush@Gdiplus@@EEEE@Z");
        if (NULL == cs) {
            DebugOut("AgentWndDLL::CreateSolidBrush �����ں��� <CreateSolidBrush> ��ַ�Ҳ�����������ˢʧ�ܣ� ");
            return NULL;
        }
        return cs(A, R, G, B);

    }
    DebugOut("AgentWndDLL::CreateSolidBrush ������ <QFUI> DLL��ģ����Ϊ0�������ؼ�ʧ�ܣ� ");
    return NULL;
}
QBrush* AgentWndDLL::CreateLinearGradientBrush(float x1, float y1, unsigned char A1, unsigned char R1, unsigned char G1, unsigned char B1
    , float x2, float y2, unsigned char A2, unsigned char R2, unsigned char G2, unsigned char B2)
{
    typedef QBrush* (*CREATELINEARGRADIENTBRUSH)(float x1, float y1, unsigned char A1, unsigned char R1, unsigned char G1, unsigned char B1
        , float x2, float y2, unsigned char A2, unsigned char R2, unsigned char G2, unsigned char B2);
    if (m_hm) {
        CREATELINEARGRADIENTBRUSH cLs = (CREATELINEARGRADIENTBRUSH)GetProcAddress(m_hm, "?QCreateLinearGradientBrush@@YAPAVBrush@Gdiplus@@MMEEEEMMEEEE@Z");
        if (NULL == cLs) {
            DebugOut("AgentWndDLL::CreateLinearGradientBrush �����ں��� <CreateLinearGradientBrush> ��ַ�Ҳ�����������ˢʧ�ܣ� ");
            return NULL;
        }
        return cLs(x1, y1, A1, R1, G1, B1, x2, y2, A2, R2, G2, B2);

    }
    DebugOut("AgentWndDLL::CreateLinearGradientBrush ������ <QFUI> DLL��ģ����Ϊ0�������ؼ�ʧ�ܣ� ");
    return NULL;
}
void AgentWndDLL::QDeleteBrush(QBrush* brush)
{
    typedef void(*DELETEBRUSH)(QBrush*);
    if (m_hm) {
        DELETEBRUSH ds = (DELETEBRUSH)GetProcAddress(m_hm, "?QDeleteBrush@@YAXPAVBrush@Gdiplus@@@Z");
        if (NULL == ds) {
            DebugOut("AgentWndDLL::QDeleteBrush �����ں��� <DeleteBrush> ��ַ�Ҳ�����ɾ����ˢʧ�ܣ� ");
        }
        return ds(brush);

    }
    DebugOut("AgentWndDLL::QDeleteBrush ������ <QFUI> DLL��ģ����Ϊ0��ɾ����ˢʧ�� ");
}

QBrush* AgentWndDLL::CreateTextureBrush(const wchar_t* path)
{
    typedef QBrush* (*CREATETEXTUREBRUSH)(const wchar_t*);
    if (m_hm) {
        CREATETEXTUREBRUSH ct = (CREATETEXTUREBRUSH)GetProcAddress(m_hm, "?QCreateTextureBrush@@YAPAVBrush@Gdiplus@@PB_W@Z");
        if (NULL == ct) {
            DebugOut("AgentWndDLL::CreateTextureBrush �����ں��� <CreateTextureBrush> ��ַ�Ҳ�����������ˢʧ�ܣ� ");
            return NULL;
        }
        return ct(path);

    }
    DebugOut("AgentWndDLL::CreateTextureBrush ������ <QFUI> DLL��ģ����Ϊ0�������ؼ�ʧ�ܣ� ");
    return NULL;
}
void AgentWndDLL::PreventScreenshot()
{
#ifdef __START_TSMASTER
    typedef void(*PREVENTSCREENSHOT)(HWND);
    if (m_hm) {
        PREVENTSCREENSHOT ps = (PREVENTSCREENSHOT)GetProcAddress(m_hm, "PreventScreenshot");
        if (NULL == ps) {
            DebugOut("AgentWndDLL::PreventScreenshot �����ں��� <PreventScreenshot> ��ַ�Ҳ�����������ͼ����ʧ�ܣ�");
            return;
        }
        int hwnd = 0;
        app.get_main_window_handle(&hwnd);
        ps(HWND(hwnd));

    }
    DebugOut("AgentWndDLL::PreventScreenshot ������ <QFUI> DLL��ģ����Ϊ0��������ͼ����ʧ�ܣ�");
#else
#endif
}
void AgentWndDLL::AllowScreenshot()
{
#ifdef __START_TSMASTER
    typedef void(*ALLOWSCREENSHOT)(HWND);
    if (m_hm) {
        ALLOWSCREENSHOT as = (ALLOWSCREENSHOT)GetProcAddress(m_hm, "AllowScreenshot");
        if (NULL == as) {
            DebugOut("AgentWndDLL::PreventScreenshot �����ں��� <PreventScreenshot> ��ַ�Ҳ������رս�ͼ����ʧ�ܣ�");
            return;
        }
        int hwnd = 0;
        app.get_main_window_handle(&hwnd);
        as(HWND(hwnd));

    }
    DebugOut("AgentWndDLL::PreventScreenshot ������ <QFUI> DLL��ģ����Ϊ0���رս�ͼ����ʧ�ܣ�");
#else

#endif

}
