#pragma once
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")
#include <wincodec.h> // ����ͼƬ����

#pragma comment(lib, "windowscodecs.lib")
#include <Windows.h>
#include<windowsx.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

#include <thread>
#include<algorithm>

#include <string>
#include "QFMsg.h"
#include <shared_mutex>
#include "QFdef.h"
typedef Gdiplus::Image GImage;
typedef Gdiplus::TextureBrush GTextureBrush;
typedef Gdiplus::Brush GBrush;
typedef Gdiplus::Rect GRect;
typedef Gdiplus::Size GSize;
typedef Gdiplus::Point GPoint;
typedef Gdiplus::PointF GPointF;
typedef Gdiplus::Color GColor;
typedef Gdiplus::SolidBrush GSolidBrush;
typedef Gdiplus::Font GFont;
typedef Gdiplus::RectF GRectF;
typedef Gdiplus::StringFormat GStringFormat;
typedef Gdiplus::Pen GPen;
typedef Gdiplus::ARGB GARGB;


/**
 * ==============================================================================================================================
 * ��細�ںͿؼ��Ļ���
 * ����ʱ�䣺2023��11��13��
 *
 *
 *
 *
 *
 * ==============================================================================================================================
*/


typedef Gdiplus::Brush GBrush;
typedef Gdiplus::Rect GRect;
typedef Gdiplus::Size GSize;
typedef Gdiplus::Point GPoint;
typedef Gdiplus::PointF GPointF;
typedef Gdiplus::Color GColor;
typedef Gdiplus::SolidBrush GSolidBrush;
typedef Gdiplus::Font GFont;
typedef Gdiplus::RectF GRectF;
typedef Gdiplus::StringFormat GStringFormat;
typedef Gdiplus::Pen GPen;
typedef Gdiplus::Image GImage;
typedef Gdiplus::REAL GREAL;
std::string GetCurrentModuleAddr();
bool isFileExists(const std::string& dllPath);

int ReconstructDC(HDC& dc, HDC& sDC, HBITMAP& hb, const GRect& rf);

//��������
union QWNDAttribute{
    unsigned long long AData=0;
    struct {
        unsigned long long isQHostCtrl : 1;//�Ƿ������ؼ�
        unsigned long long isContainerCtrl : 1;//�Ƿ������ؼ����ؼ����滹�洢�����ؼ�
        unsigned long long isLayout : 1;//�Ƿ񲼾�����
        unsigned long long HostCtrlMouse : 1;//QHostCtrl���������Ϣ

    }Attribute;

};

class QWND
{
protected:
    HDC m_dc;//����dc
    HDC m_sdc;//��һ���dc
    GRect m_rect;
    HBITMAP m_hBitmap;
    HCURSOR m_hCursor = NULL;
    QWND* m_Parent = NULL;
    std::string m_name;
    QWNDAttribute __WNDAttribute;
    //std::shared_mutex m_draw_lk;
   
public:
    bool m_isVisual = true;
    bool m_isEnable = true;
    HWND m_HostHwnd = NULL;//�������ھ��,�������Ǹ�������Ҫָ���¾����������Щ����ʧЧ,�����m_ParentҪ����һ��
     HCURSOR SetCursor(HCURSOR hc) ;
    HCURSOR GetCursor();
    void LoadThisCtrlCursor() ;
    virtual void Refresh();


    /**
     * ��һ��������Ҫչʾ��dc�������չʾ�ڴ��������ǸĴ��ڵ�dc��������ڴ�dcҲ��
     * �ڶ�������������ؼ����ߴ���Ҫչʾ�Ĵ�С����λ��
    */
    QWND(HDC winddc, const GRect& rf);
    /**
     * ����һ��Ҫչʾ��dc�������չʾ�ڴ��������ǸĴ��ڵ�dc��������ڴ�dcҲ��
     * ��������չʾλ��x����
     * ��������չʾλ��y����
     * �����ģ�չʾ���
     * �����壬չʾ�߶�
    */
    QWND(IN HDC winddc,
        IN INT x,
        IN INT y,
        IN INT width,
        IN INT height);
    virtual ~QWND();

    virtual int Draw(HDC sdc = NULL);                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���
    virtual void Reconstruct(HDC sdc);                                          //���´���
    virtual void SetLocation(const GPoint& point);                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //���ô�С(�����¹����ؼ�)
    //�Ƿ����ĳ��
    virtual BOOL Contains(IN INT x,
        IN INT y) const;
    virtual BOOL Contains(IN const GPoint& pt) const;
    virtual BOOL Contains(IN GRect& rect) const;
    virtual void Redraw() ;

    ///////////////////////////////////////////////////////////////////////////
    //��Ϣ����
        //������Ҽ��ظ�����,
#define QFUI_CTRL_CAPTURE_MOUSE 1 //����갴�µ��¼��������ֵ�ܲ�����꣬ǰ����û�пؼ��ڲ���
#define QFUI_CTRL_ACQUIRE_KEYBOARD 2 //����갴�µ��¼��������ֵ�ܻ�ü��̽���



    virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);//ԭʼ��Ϣ

    virtual int LButtonDown(int x, int y, WPARAM wParam);//������������Ϣ��
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//������������Ϣ��
    virtual int LButtonUp(int x, int y, WPARAM wParam);//������˫����Ϣ��

    virtual int RButtonDown(int x, int y, WPARAM wParam);//����Ҽ�������Ϣ��
    virtual int RButtonUp(int x, int y, WPARAM wParam);//����Ҽ�������Ϣ��
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//����Ҽ�˫����Ϣ��

    virtual int MButtonDown(int x, int y, WPARAM wParam);//����м�������Ϣ��
    virtual int MButtonUp(int x, int y, WPARAM wParam);//����м�������Ϣ��
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//����м�˫����Ϣ��
    virtual int MouseWheel(int x, int y, WPARAM wParam);//������ת��

    virtual int MouseMove(int x, int y, WPARAM wParam);//����ƶ���Ϣ
    virtual int MouseMoveOut();                         //����Ƴ�

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//���̼���һ���ַ�
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//���뷨����һ���ַ�����
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//���̰���ĳ��
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//���̵���ĳ��


    ///////////////////////////////////////////////////////////////////////////
    //ʧȥ����
    virtual void killFocus() {};


    /*
    *������Ϣ
    * ����ֵ�Ͳ���Ϊ�Զ���
    * ����һ����ϢID
    * �������������
    * ���������в���
    * �����ģ����Ӳ���
    */
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    void SetQWNDName(const char* name);
    const char* GetQWNDName();
    GRect GetRect();
    unsigned long long GetAttribute();//��ȡ����ֵ
    virtual int GetSubCtrlCount();//ȡ�ӿؼ���
    virtual QWND* GetSubCtrl(int index);//ȡ�ӿؼ�
   
private:
    //�����Լ���DC
    inline HDC QCreateDC(HDC winddc);
    //�ͷ�DC
    inline void ReleaseDC();


protected:
    virtual void UpdateWND(QDWORD para=1);



};



