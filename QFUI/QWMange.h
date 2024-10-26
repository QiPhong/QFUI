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
//���Ա�־
//#define __TEST_CODE_QWM

typedef Gdiplus::Brush GBrush;
class QWMangeListener
{
protected:


public:
    QLSizeChangeListener* __sizeChangeListen = NULL;//�ı�ǰ��
    QLSizeChangeListener* __sizeChangedListen = NULL;//�ı�֮���



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
    std::shared_mutex m_drawLK;//������
private:
    std::vector<QWND*> m_QWNDs;     //����ؼ�
    std::shared_mutex m_shared_lock_ctrl;//�����ؼ�����
    
    std::list<QMsgModule*> m_msgModule;
    void LoadMsgModule();
    void UnLoadMsgModule();
    //std::mutex m_clock;
    std::mutex m_clock_queue;
    std::condition_variable m_cv_queue;
    bool isRunThread = true;
    std::queue<QFMSG> m_msg_queue;
    //��Ϣ�߳��˳�
    std::condition_variable m_cv_msgThread_Exit;
    std::mutex m_clock_msgThread_Exit;

    std::thread m_msg_thread;
    void msgThreadFun();
    void pushQueue(const QFMSG& msg);
    QFMSG popQueue();


    QWND* FocusCtrl = NULL;           //����ؼ�����Ҫ��Ӧ���̵�
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    HDC m_bdc = NULL;                 //����dc,�ؼ�����ʱ�����dc�������
    HBITMAP m_bhB = NULL;
    HDC m_bkdc = NULL;                //����dc,���汳��
    HBITMAP m_bkhB = NULL;
    bool isChange = true;

    GBrush* m_bkBrush = NULL;       //������ˢ
    QWMangeListener __listener;    //�¼�������
    //
    //�ؼ�����ͼƬ
    Gdiplus::ImageAttributes* imageAttr=NULL;
    GImage* m_pImage = NULL;


    std::atomic <QPopDialogBase*> m_popDialog = nullptr;//
    //�л�������
    std::shared_mutex m_invertPopDLK;

public:
    QWMange(HDC winddc, const GRect& rf);
    virtual ~QWMange();

    void AttachCtrl(QWND* w);       //����һ���ؼ�
    void RemoveCtrl(QWND* w);       //�Ƴ�һ���ؼ�

    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual void SetLocation(const GPoint& point)override;                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //���ô�С(�����¹����ؼ�)
    virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GBrush* GetBKBrush() {/* return m_bkBrush;*/return NULL; };

    QLSizeChangeListener* SetSizeChangeListen(QLSizeChangeListener* listen);        //���ô��ڴ�С�����ı�֮ǰ�ļ�����
    QLSizeChangeListener* SetSizeChangedListen(QLSizeChangeListener* listen);        //���ô��ڴ�С�����ı�֮��ļ�����
    int SetBKBrush(GBrush* brush);

    int SetBKImage(const wchar_t* path);
    void SetTransparency(unsigned char apha);
    void ClearBKImage();
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual int GetSubCtrlCount();//ȡ�ӿؼ���
    virtual QWND* GetSubCtrl(int index);//ȡ�ӿؼ�
private:
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    void UpdateBK(const GBrush* brush);
    int __MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);



    //**************************��Ϣ����
    //�����Ϣ
    inline void MouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    inline void MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************


    //***************************************************************************************���ڴ�С�仯�߳̿���
    std::chrono::time_point<std::chrono::high_resolution_clock> m_OldChangeTime;//��һ�δ��ڴ�����С�ı��ʱ��
    static constexpr unsigned int m_wcCyc = 600;                                //��λ����
    std::mutex m_lock_wcsz;
    bool m_isRunWCThread = false;
    void WindChangeSizeEvent();                                                  //�̺߳���
    bool m_sizeChaneg = false;

    //***************************************************************************************
    std::recursive_mutex m_BKdraw_lock;//�����滭��



    //���Դ��붨��
#ifdef __TEST_CODE_QWM
    void DrawBK();


#endif



};




#endif

