#include "QFUIRenderer.h"







//******************************.CPP file***********************************************
QRenderer* QRenderer::Instance = NULL;
std::mutex QRenderer::m_QRMutex;


QRenderer::QRenderer()
{



}



QRenderer* QRenderer::GetInstance()
{
    if (Instance == nullptr)
    {
        std::unique_lock<std::mutex> lock(m_QRMutex); // ����
        if (Instance == nullptr)
        {
            volatile auto temp = new (std::nothrow) QRenderer();
            Instance = temp;
            // __Mangement::qrpm.setMange(temp,&QRenderer::__del);
        }
    }

    return Instance;

}

void QRenderer::__del()
{

    __threadIsrun = false;

    DebugOut("void QRenderer::__del()");
    if (__qrThread.joinable())
        __qrThread.join();
    // if(pqwm->qwm)
    //     delete pqwm->qwm;

    DebugOut("void QRenderer::__del() delete pqwm;");
    ReleaseDC(0, __dc);


}

QRenderer::~QRenderer()
{
    // __del();

}
int QRenderer::startDraw()
{
    if (!pqwm)return 3;
    if (__threadIsrun)return 2;
    __threadIsrun = true;
    std::thread t(&QRenderer::__Draw, this);

    __qrThread = std::move(t);

    return 1;

}

void QRenderer::__Draw()
{

    while (__threadIsrun) {
        pqwm->Draw(__dc);
        std::this_thread::sleep_for(std::chrono::microseconds(DrawCyc_us));

    }


}

void QRenderer::setWindow(HWND hw)
{

    __hwnd = hw;
    __dc = GetDC(hw);
    GetWindowRect(hw, &__rect);

    if (__threadIsrun)return;
    if (pqwm) delete pqwm;
    int width = __rect.right - __rect.left;
    int height = __rect.bottom - __rect.top;

    pqwm = new QWMange(__dc, QRect(0, 0, width, height));


}

namespace QFUSE {

    /**
     * ###################################################################
     * ��������BindPanelWindow
     * ���ã���ʼ���Ͱ�panel
     * ����1��panel���ڵ�����
     * ����ֵ:����1Ϊ����������0Ϊ���ڹ���������Ⱦ����ָ��Ϊ�գ�����4Ϊ�Ҳ���ָ�����ֵ�panel
     * ###################################################################
    */
    int BindPanelWindow(const char* panelName)
    {
#ifdef __START_TSMASTER



        char* pPath = NULL;
        app.get_configuration_file_path(&pPath);
        std::string strPath(pPath);
        strPath += "libs\\common\\QFUI\\QWNDHandle.dll";
#ifdef __QFUIDLL_PATH
        // strPath="C:\\Users\\Administrator\\Desktop\\QFUI\\Release\\QWNDHandle.dll";
        strPath = __QFUIDLL_PATH;
#endif
        AgentWndDLL* p = AgentWndDLL::GetInstance(strPath.c_str());

        if (p) {


            QRenderer* pqr = QRenderer::GetInstance();
            if (pqr) {
                // app.ui_show_window(const_cast<char*>(panelName), true);

                int res = p->BindPanel(panelName);
                if (5 == res && NULL != QWNDHANDLE::hWin) {
                    app.ui_show_window(const_cast<char*>(panelName), true);
                    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                    res = p->BindPanel(panelName);
                    // std::this_thread::sleep_for(std::chrono::microseconds(3000));
                    app.ui_show_window(const_cast<char*>(panelName), false);

                }

                if (1 == res) {
                    HWND hwnd = HWND(QWNDHANDLE::hWnd);
                    if (NULL == hwnd) {
                        return 5;
                    }

                    pqr->setWindow(hwnd);
                    pqr->pqwm = p->CreateQWMange();
                    p->BindQwnd(pqr->pqwm);
                    int ret = pqr->startDraw();
                    return ret;
                }
                else return 4;
            }
            else return 0;

        }
        else return 0;
#endif 
        return 0;
    }

    int BindPanelWindow(HWND hwnd)
    {



        std::string strPath = "QWNDHandle.dll";
#ifdef __QFUIDLL_PATH
        // strPath="C:\\Users\\Administrator\\Desktop\\QFUI\\Release\\QWNDHandle.dll";
        strPath = __QFUIDLL_PATH;
#endif
        AgentWndDLL* p = AgentWndDLL::GetInstance(strPath.c_str());

        if (p) {

            QRenderer* pqr = QRenderer::GetInstance();
            if (pqr) {
                // app.ui_show_window(const_cast<char*>(panelName), true);

                int res = p->BindPanel(hwnd);


                if (1 == res) {

                    pqr->setWindow(hwnd);
                    pqr->pqwm = p->CreateQWMange();

                    p->BindQwnd(pqr->pqwm);
                    int ret = pqr->startDraw();
                    return ret;
                }
                else return 4;
            }
            else return 0;

        }
        else return 0;

        return 0;
    }



    /**
     * ###################################################################
     * ��������UnBindPanelWindow
     * ���ã������panel����
     * ����ֵ:����1Ϊ����
     * ###################################################################
    */

    void UnBindPanelWindow()
    {

        QRenderer* pqr = QRenderer::GetInstance();
        if (pqr) {
            pqr->__del();
        }
#ifndef __START_TSMASTER
        __Mangement::awpm.~__AgentWndPtrMangement();

#endif
    }

    /**
     * ###################################################################
     * ��������GetPanelDC
     * ���ã�ȡ�ð󶨵�panel��DC
     * ����ֵ:panel��DC
     * ##################################################################
    */
    HDC GetPanelDC()
    {
        QRenderer* pqr = QRenderer::GetInstance();
        if (pqr) {
            return pqr->getDC();
        }
        return NULL;
    }

    /**
     * ###################################################################
     * ��������AttachCtrl
     * ���ã�����(����)һ���ؼ���panel��
     * ����1�� �ؼ�ָ��
     * ����ֵ:��
     * ##################################################################
    */
    void AttachCtrl(QWND* qw)
    {
        DLLImportQWND* diq = (DLLImportQWND*)qw;
        QRenderer* pqr = QRenderer::GetInstance();
        if (pqr) {
            pqr->AttachCtrl(diq->qwm);
        }
    }
    /**
     * ###################################################################
     * ��������RemoveCtrl
     * ���ã��ӵ�panel��ȡ������(�Ƴ�)һ���ؼ�
     * ����1�� �ؼ�ָ��
     * ����ֵ:��
     * ##################################################################
    */
    void RemoveCtrl(QWND* qw)
    {
        DLLImportQWND* diq = (DLLImportQWND*)qw;
        QRenderer* pqr = QRenderer::GetInstance();
        if (pqr) {
            pqr->RemoveCtrl(diq->qwm);
        }
    }


    QWND* CreateQWND(const char* name, const QRect& rf)
    {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            QRenderer* pqr = QRenderer::GetInstance();
            HDC dc = NULL;
            if (pqr) {
                dc = pqr->getDC();
            }
            if (NULL == dc) {
                DebugOut("QFUSE::CreateQWND,ʧ�ܣ���Ϊ���������ĳ���ָ��Ϊ��");
                return NULL;
            }
            return p->CreateQWND(name, dc, rf);
        }


        return NULL;
    }

    QWND* GetQWManger()
    {
        QRenderer* pqr = QRenderer::GetInstance();
        if (pqr) {
            return pqr->GetQWManger();
        }
        return NULL;

    }

    QBrush* CreateSolidBrush(unsigned char A, unsigned char R, unsigned char G, unsigned char B)
    {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            return p->CreateSolidBrush(A, R, G, B);
        }
        return NULL;
    }
    QBrush* CreateTextureBrush(const wchar_t* path)
    {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            return p->CreateTextureBrush(path);
        }
        return NULL;
    }
    QBrush* CreateLinearGradientBrush(float x1, float y1, unsigned char A1, unsigned char R1, unsigned char G1, unsigned char B1
        , float x2, float y2, unsigned char A2, unsigned char R2, unsigned char G2, unsigned char B2)
    {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            return p->CreateLinearGradientBrush(x1, y1, A1, R1, G1, B1, x2, y2, A2, R2, G2, B2);
        }
        return NULL;
    }

    void QDeleteBrush(QBrush* brush)
    {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            p->QDeleteBrush(brush);
        }

    }

    void QPreventScreenshot() {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            p->PreventScreenshot();
        }

    }
    void QAllowScreenshot() {
        AgentWndDLL* p = AgentWndDLL::GetInstance();
        if (p) {
            p->AllowScreenshot();
        }

    }

    void GainLifeCyclePower(QWND* qw)
    {
        QWNDHANDLE::QWNDs.erase(qw);
    }

    QWND* FindCtrl(const char* name)
    {
        for (auto& it : QWNDHANDLE::QWNDs) {
            if (name == ((DLLImportQWND*)it)->GetQWNDName()) {
                return it;
            }
        }
        return nullptr;
    }








}