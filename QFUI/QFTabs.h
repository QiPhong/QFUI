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
    std::vector<QWND*> m_QWNDs;     //本标签下的控件
    std::mutex lock;                //页锁
    ID2D1Bitmap* pBitmap = NULL;    //标签图标
    std::wstring mapPath;

    double width;//字符测量得到的宽度
    double hight;//字符测量得到的高度
    bool isUpdateMetrics=true;//是否重新测量
    D2D1_ROUNDED_RECT roundedRect;//标签的区域
    bool isFocus = false;       //是否焦点
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
    std::vector<QTabsProperties*> m_tabs;//标签
    //int pos = -1;//选中的标签，-1为都没有选中
    double maxHeight=0.0f;//最大高度
    QTabsProperties* m_FocusTab =NULL;
    std::shared_mutex m_lock;//所有标签锁，是把读写锁
};

class QTabs :public QContainer
{
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//创建一个图层
    IWICImagingFactory* pWICFactory = NULL;
    QWND* FocusCtrl = NULL;           //焦点控件，主要响应键盘的
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
    //页DC
    HDC m_pageDC = NULL;                 //函数绘制dc
    HBITMAP m_pageHB = NULL;              //
    std::mutex m_AttributeLock;         //设置属性的锁
#ifdef __SAMPLE_DISPLAY__
    GSolidBrush* __SDbrush = NULL;
#endif 

    HCURSOR m_hCursorBuff = NULL;
    std::chrono::steady_clock::time_point __ct;
public:
    QTabs(HDC winddc, const GRect& rf);
    virtual ~QTabs();

    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
    virtual void killFocus()override;

  
    ///////////////////////////////////////////////////////////////////////////
    virtual QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
private:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x,int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
   
private:
    std::mutex m_drawLock;
    GBrush* m_bkBrush = NULL;//背景画刷
    double m_mousex=-1, m_mousey=-1;
    
    //std::vector<QTabsProperties> m_tabs;//标签
    QTabsOverview m_qto;//标签

    std::wstring m_fontName = L"Arial";
    double m_fontSize = 16.0f;
    unsigned long m_fontColor = 0xff000000;
    unsigned long __NormalTabColor = 0xffffffff;

    void __AddTab(QTabsProperties& qtp);
    void __AddTab(QTabsProperties& qtp, int index);

private:
    //一些暂时设定，没开放的属性
    double __DistancLeft = 5.0f;//标签距离左边大小
   


public:
    //一些对外的接口
    int AttachCtrl(QWND* w,int index);       //关联一个控件
    int RemoveCtrl(QWND* w,int index);       //移除一个控件
    int AddTab(const wchar_t* name,const wchar_t* pBPath=NULL);//增加一个标签
    int RemoveTab(int index);//移除一个标签
    int SetBKBrush(GBrush* brush);
    void SetFontName(const wchar_t* name);//设置字体名称
    void SetFontSize(double size);//设置字体大小
    void SetFontColor(unsigned long color);//设置字体颜色
    void SetTabIco(int index,const wchar_t* path);
    void SetTabName(int index, const wchar_t* name);
    void SetTabBKColor(unsigned long color);
    void AttachCtrlToCurrentTabe(QWND* w);
    int GetTabeCount();
    int GetTabeCtrlCount(int index);
    QWND* GetCtrl(int ti,int index);
    int InsertTab(const wchar_t* name, int index = -1, const wchar_t* pBPath = NULL);//插入一个标签
    const wchar_t* GetTabeBMPPath(int index);
    const wchar_t* GetTabeText(int index);
    virtual int GetSubCtrlCount();//取子控件数
    virtual QWND* GetSubCtrl(int index);//取子控件
    virtual const wchar_t* GetCurrentTabeText();
    virtual const wchar_t* GetCurrentTabeIcoPath();
    virtual int SetCurrentTabeText(const wchar_t* txt);
    virtual int SetCurrentTabeIcoPath(const wchar_t* path);
    virtual void Redraw() override;
};

