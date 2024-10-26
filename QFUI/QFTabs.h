#pragma once
#include "QFCtrlBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
//#define __SAMPLE_DISPLAY__
class QTabsProperties 
{
public:
    std::wstring text;
    std::vector<QWND*> m_QWNDs;     //����ǩ�µĿؼ�
    std::mutex lock;                //ҳ��
    ID2D1Bitmap* pBitmap = NULL;    //��ǩͼ��
    std::wstring mapPath;

    double width;//�ַ������õ��Ŀ��
    double hight;//�ַ������õ��ĸ߶�
    bool isUpdateMetrics=true;//�Ƿ����²���
    D2D1_ROUNDED_RECT roundedRect;//��ǩ������
    bool isFocus = false;       //�Ƿ񽹵�
    QTabsProperties() {};
    QTabsProperties(const QTabsProperties& qtp)
    {
        text = qtp.text;
        m_QWNDs = qtp.m_QWNDs;
        pBitmap = qtp.pBitmap;
        width = qtp.width;
        hight = qtp.hight;
        isUpdateMetrics = qtp.isUpdateMetrics;
        roundedRect = qtp.roundedRect;
        isFocus = qtp.isFocus;
        mapPath = qtp.mapPath;
    }

};


class QTabsOverview
{
public:
    std::vector<QTabsProperties*> m_tabs;//��ǩ
    //int pos = -1;//ѡ�еı�ǩ��-1Ϊ��û��ѡ��
    double maxHeight=0.0f;//���߶�
    QTabsProperties* m_FocusTab =NULL;
    std::shared_mutex m_lock;//���б�ǩ�����ǰѶ�д��
};

class QTabs :public QContainer
{
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//����һ��ͼ��
    IWICImagingFactory* pWICFactory = NULL;
    QWND* FocusCtrl = NULL;           //����ؼ�����Ҫ��Ӧ���̵�
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
    //ҳDC
    HDC m_pageDC = NULL;                 //��������dc
    HBITMAP m_pageHB = NULL;              //
    std::mutex m_AttributeLock;         //�������Ե���
#ifdef __SAMPLE_DISPLAY__
    GSolidBrush* __SDbrush = NULL;
#endif 

    HCURSOR m_hCursorBuff = NULL;
    std::chrono::steady_clock::time_point __ct;
public:
    QTabs(HDC winddc, const GRect& rf);
    virtual ~QTabs();

    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//������������Ϣ��
    virtual int LButtonUp(int x, int y, WPARAM wParam);//������˫����Ϣ��

    virtual int RButtonDown(int x, int y, WPARAM wParam);//����Ҽ�������Ϣ��
    virtual int RButtonUp(int x, int y, WPARAM wParam);//����Ҽ�������Ϣ��
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//����Ҽ�˫����Ϣ��

    virtual int MButtonDown(int x, int y, WPARAM wParam);//����м�������Ϣ��
    virtual int MButtonUp(int x, int y, WPARAM wParam);//����м�������Ϣ��
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//����м�˫����Ϣ��
    virtual int MouseWheel(int x, int y, WPARAM wParam);//������ת��

    virtual int MouseMoveOut();                         //����Ƴ�

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//���̼���һ���ַ�
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//���뷨����һ���ַ�����
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//���̰���ĳ��
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//���̵���ĳ��
    virtual void killFocus()override;

  
    ///////////////////////////////////////////////////////////////////////////
    virtual QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
private:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x,int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
   
private:
    std::mutex m_drawLock;
    GBrush* m_bkBrush = NULL;//������ˢ
    double m_mousex=-1, m_mousey=-1;
    
    //std::vector<QTabsProperties> m_tabs;//��ǩ
    QTabsOverview m_qto;//��ǩ

    std::wstring m_fontName = L"Arial";
    double m_fontSize = 16.0f;
    unsigned long m_fontColor = 0xff000000;
    unsigned long __NormalTabColor = 0xffffffff;

    void __AddTab(QTabsProperties& qtp);
    void __AddTab(QTabsProperties& qtp, int index);

private:
    //һЩ��ʱ�趨��û���ŵ�����
    double __DistancLeft = 5.0f;//��ǩ������ߴ�С
   


public:
    //һЩ����Ľӿ�
    int AttachCtrl(QWND* w,int index);       //����һ���ؼ�
    int RemoveCtrl(QWND* w,int index);       //�Ƴ�һ���ؼ�
    int AddTab(const wchar_t* name,const wchar_t* pBPath=NULL);//����һ����ǩ
    int RemoveTab(int index);//�Ƴ�һ����ǩ
    int SetBKBrush(GBrush* brush);
    void SetFontName(const wchar_t* name);//������������
    void SetFontSize(double size);//���������С
    void SetFontColor(unsigned long color);//����������ɫ
    void SetTabIco(int index,const wchar_t* path);
    void SetTabName(int index, const wchar_t* name);
    void SetTabBKColor(unsigned long color);
    void AttachCtrlToCurrentTabe(QWND* w);
    int GetTabeCount();
    int GetTabeCtrlCount(int index);
    QWND* GetCtrl(int ti,int index);
    int InsertTab(const wchar_t* name, int index = -1, const wchar_t* pBPath = NULL);//����һ����ǩ
    const wchar_t* GetTabeBMPPath(int index);
    const wchar_t* GetTabeText(int index);
    virtual int GetSubCtrlCount();//ȡ�ӿؼ���
    virtual QWND* GetSubCtrl(int index);//ȡ�ӿؼ�
    virtual const wchar_t* GetCurrentTabeText();
    virtual const wchar_t* GetCurrentTabeIcoPath();
    virtual int SetCurrentTabeText(const wchar_t* txt);
    virtual int SetCurrentTabeIcoPath(const wchar_t* path);
    virtual void Redraw() override;
};

