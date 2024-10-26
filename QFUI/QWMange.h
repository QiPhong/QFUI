#pragma once
#ifndef __QWMANGE_H
#define __QWMANGE_H

#include "QFUIWND.h"
#include <vector>
#include <mutex>
#include <queue>
#include <list>
#include <condition_variable>
#include "QFCtrlListener.h"
#include <chrono>
#include "QFMsgModule.h"
#include "QFPopDialog.h"
#include <atomic>
//测试标志
//#define __TEST_CODE_QWM

typedef Gdiplus::Brush GBrush;
class QWMangeListener
{
protected:


public:
    QLSizeChangeListener* __sizeChangeListen = NULL;//改变前的
    QLSizeChangeListener* __sizeChangedListen = NULL;//改变之后的



};
struct QFMSG {
    QFMSG()
    {

    }
    QFMSG(HWND h, UINT m, WPARAM w, LPARAM l)
    {
        hwnd = h;
        msg = m;
        wParam = w;
        lParam = l;
    }
    HWND hwnd = HWND(0);
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;


};
#define __QWMANGE_MSG_MAX   128
class QWMange :public QWND
{
    std::chrono::steady_clock::time_point __createrTime = std::chrono::high_resolution_clock::now();
    std::shared_mutex m_drawLK;//绘制锁
private:
    std::vector<QWND*> m_QWNDs;     //保存控件
    std::shared_mutex m_shared_lock_ctrl;//操作控件的锁
    
    std::list<QMsgModule*> m_msgModule;
    void LoadMsgModule();
    void UnLoadMsgModule();
    //std::mutex m_clock;
    std::mutex m_clock_queue;
    std::condition_variable m_cv_queue;
    bool isRunThread = true;
    std::queue<QFMSG> m_msg_queue;
    //消息线程退出
    std::condition_variable m_cv_msgThread_Exit;
    std::mutex m_clock_msgThread_Exit;

    std::thread m_msg_thread;
    void msgThreadFun();
    void pushQueue(const QFMSG& msg);
    QFMSG popQueue();


    QWND* FocusCtrl = NULL;           //焦点控件，主要响应键盘的
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    HDC m_bdc = NULL;                 //缓冲dc,控件更新时往这个dc输出内容
    HBITMAP m_bhB = NULL;
    HDC m_bkdc = NULL;                //背景dc,保存背景
    HBITMAP m_bkhB = NULL;
    bool isChange = true;

    GBrush* m_bkBrush = NULL;       //背景画刷
    QWMangeListener __listener;    //事件监听器
    //
    //控件背景图片
    Gdiplus::ImageAttributes* imageAttr=NULL;
    GImage* m_pImage = NULL;


    std::atomic <QPopDialogBase*> m_popDialog = nullptr;//
    //切换弹窗锁
    std::shared_mutex m_invertPopDLK;

public:
    QWMange(HDC winddc, const GRect& rf);
    virtual ~QWMange();

    void AttachCtrl(QWND* w);       //关联一个控件
    void RemoveCtrl(QWND* w);       //移除一个控件

    virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual void SetLocation(const GPoint& point)override;                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //设置大小(会重新构建控件)
    virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GBrush* GetBKBrush() {/* return m_bkBrush;*/return NULL; };

    QLSizeChangeListener* SetSizeChangeListen(QLSizeChangeListener* listen);        //设置窗口大小发生改变之前的监听器
    QLSizeChangeListener* SetSizeChangedListen(QLSizeChangeListener* listen);        //设置窗口大小发生改变之后的监听器
    int SetBKBrush(GBrush* brush);

    int SetBKImage(const wchar_t* path);
    void SetTransparency(unsigned char apha);
    void ClearBKImage();
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual int GetSubCtrlCount();//取子控件数
    virtual QWND* GetSubCtrl(int index);//取子控件
private:
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    void UpdateBK(const GBrush* brush);
    int __MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);



    //**************************消息处理
    //鼠标消息
    inline void MouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    inline void MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************


    //***************************************************************************************窗口大小变化线程控制
    std::chrono::time_point<std::chrono::high_resolution_clock> m_OldChangeTime;//上一次窗口触发大小改变的时间
    static constexpr unsigned int m_wcCyc = 600;                                //单位毫秒
    std::mutex m_lock_wcsz;
    bool m_isRunWCThread = false;
    void WindChangeSizeEvent();                                                  //线程函数
    bool m_sizeChaneg = false;

    //***************************************************************************************
    std::recursive_mutex m_BKdraw_lock;//背景绘画锁



    //测试代码定义
#ifdef __TEST_CODE_QWM
    void DrawBK();


#endif



};




#endif

