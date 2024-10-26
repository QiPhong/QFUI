#pragma once
/**
* ========================================================================================================================================
* ���ÿؼ���
* 1��QGroup(�����)
* 2��QRadio����ѡ��
* 3��QDrawer (����ؼ�)
* 4��QHostCtrl�������ؼ���
* 5��QHLayout��ˮƽ��������
* 6��QVLayout ����ֱ��������
* 7��QStaticText ����̬�ı���
* 8��QSwitch �����ؿؼ���
* 9��QButton2
* ========================================================================================================================================
*/
#include "QFCtrlBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include "QFTabs.h"
#include <map>
#include <set>
#include <cairo.h>


class QDSubkey
{
public:
    std::wstring name = L"����Ŀ";
    float width = 0;//�����ַ���������Ŀ��
    float height = 0;//�����ַ���������ĸ߶�
    bool isChange = true;
    int color = 0xff000000;
    int bcolor = 0x80ffffff;
};



class Category
{
public:
    std::vector<QDSubkey> subkey;
    std::wstring name=L"��Ŀ";
    //�����ַ���������Ŀ��
    float width = 0;
    //�����ַ���������ĸ߶�
    float height = 0;
    bool isChange = true;
    int color=0xff000000;
    int bcolor = 0x80ffffff;
    bool isWithdrawn = false;
};

typedef int (*QDRAWERSELECT)(int,int);
#define __QDRAWER__
class QDrawer :public QWND
{
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    HDC m_pageDC = NULL;                 //��������dc
    HBITMAP m_pageHB = NULL;              //
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    //ID2D1Layer* pLayer = NULL;//����һ��ͼ��
    IWICImagingFactory* pWICFactory = NULL;
    bool isChange = true;
    GBrush* m_bkBrush = NULL;//������ˢ
    std::wstring m_fontName = L"Arial";
    float m_fontSize = 16.0f;
    float m_startPos = 0.0f;
    std::shared_mutex m_lock;
    std::vector<Category> m_data;
    QDRAWERSELECT __selectFUN = NULL;

    //���λ��
    int __mouseX = -1, __mouseY = -1;

public:
    QDrawer(HDC winddc, const GRect& rf);
    virtual ~QDrawer();
    virtual void Redraw() { isChange = true; };
    virtual void Refresh() { isChange = true; };
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int MouseMove(int x, int y, WPARAM wParam)override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseWheel(int x, int y, WPARAM wParam);//������ת��
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//������������Ϣ
    virtual int MouseMoveOut();
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���
private:
    void Test();

public:
    //�û��ӿ�
    void SetFontName(const wchar_t* name);
    void SetFontSize(float size);
    int InsertDrawer(int index);
    int InsertSubObject(int iDrawer,int iObj);
    int RemoveSubObject(int iDrawer, int iObj);
    int RemoveDrawer(int index);
    int GetDrawerCount();
    int GetSubObjectCount(int index);
    int SetDrawerName(int index,const wchar_t* name);
    int SetDrawerFontColor(int index,int color);
    int SetDrawerBKColor(int index, int color);
    int OpenDrawer(int index);
    int CloseDrawer(int index);
    int SetSubObjectName(int iDrawer, int iObj, const wchar_t* name);
    int SetSubObjectFontColor(int iDrawer, int iObj, int color);
    int SetSubObjectBKColor(int iDrawer, int iObj, int color);
    int SetSelectEvent(QDRAWERSELECT fun);

    int GetSubObjectName(int iDrawer, int iObj, wchar_t* pName, int bl);
    int GetDrawerName(int iDrawer, wchar_t* pName, int bl);
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
};

#define __QHOSTCTRL__


class QHostCtrl :public QWND
{
    std::mutex m_lock;


    //�����ʽ
    static HCURSOR m_hCursorWE ;
    static HCURSOR m_hCursorNS;
    static HCURSOR m_hCursorARROW;
    static HCURSOR m_hCursorSIZEALL;

    QWND* FocusCtrl = nullptr;
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    HDC m_pageDC = NULL;                 //��������dc
    HBITMAP m_pageHB = NULL;              //
    ID2D1Factory* m_pD2DFactory = NULL;
    ID2D1DCRenderTarget* m_pRenderTarget = NULL;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;

    int m_MouseStartX = -1, m_MouseStartY = -1;
    int m_MouseCurrentX = -1, m_MouseCurrentY = -1;
    //ID2D1Layer* pLayer = NULL;//����һ��ͼ��
    IWICImagingFactory* pWICFactory = NULL;
    bool isChange = true;
    bool isDown = false;
    bool isrDown = false;
    bool isSelect = false;//��ǰ�ؼ��Ƿ�ѡ��
    struct {
        int flag = -1;//0Ϊ�ϱߣ�1Ϊ�ұߣ�2Ϊ�±ߣ�3Ϊ��ߣ�4Ϊ�м�
        float dta = -1;//����ʱ������һ�ߵľ���
        float dta2= -1;//���þ���

    }__DownData;
    std::wstring m_fontName = L"Arial";
    float m_fontSize = 16.0f;

    //�ڲ��ؼ�
    QWND* m_boardCtrl = NULL;

    std::map<std::string, std::string> __pairArr;

public:
    
    QHostCtrl(HDC winddc, const GRect& rf);
    virtual ~QHostCtrl();
    virtual void Redraw() { isChange = true; };
    virtual void Refresh() { isChange = true; };
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ
    


    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ

    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                         //���´���
    virtual int MouseMove(int x, int y, WPARAM wParam)override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void killFocus()override;
    virtual int MouseMoveOut()override;
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;//������ת��
   
    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//���̼���һ���ַ�
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//���뷨����һ���ַ�����
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//���̰���ĳ��
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//���̵���ĳ��
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
public:
    //�ⲿ�ӿ�
    int SetParasiticCtrl(QWND* ctrl, QWND** oldCtrl = nullptr);//����-1 Ϊ�ؼ�����ֱ��Ϊ�����ؼ�
    QWND* GetParasiticCtrl();
    
private:
    void Test();
    
};




class QLayoutObj 
{
public:
    QWND* pWnd = NULL;
    unsigned int weight = 0;//Ȩ�أ�0Ϊ�����ȣ�����0��ʱ�����Ȩ�ذ�����

};
#define __QHLAYOUT__
//ˮƽ������
class QHLayout :public QLayout
{

    bool isChange = true;
    std::vector<QLayoutObj> m_QWNDs;
    std::shared_mutex m_QWND_lk;
    std::shared_mutex m_Draw_lk;
public:
    QHLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() ;
    virtual void Refresh() ;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���
    virtual void Reconstruct(HDC sdc);                                          //���´���
    virtual ~QHLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
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
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //���ô�С(�����¹����ؼ�)
    virtual int Type()override { return 1; };//ˮƽ���ַ���1����ֱ���ַ���2
private:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
    inline void CtrlReconstruct();//���¿ؼ����Σ�û��������������ʹ��
    double m_mousex = -1, m_mousey = -1;
    QWND* FocusCtrl = NULL;           //����ؼ�����Ҫ��Ӧ���̵�
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
    
private:
    //����
    void Test();

public:
    //�û��ӿ�
    int InsertCtrl(QWND* ctrl, unsigned int weight=0,int index=-1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};

#define __QVLAYOUT__
//��ֱ������
class QVLayout :public QLayout
{

    bool isChange = true;
    std::vector<QLayoutObj> m_QWNDs;
    std::shared_mutex m_QWND_lk;
    std::shared_mutex m_Draw_lk;
public:
    QVLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���
    virtual void Reconstruct(HDC sdc);                                          //���´���
    virtual ~QVLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
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
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //���ô�С(�����¹����ؼ�)
    virtual int Type()override { return 2; };//ˮƽ���ַ���1����ֱ���ַ���2
private:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************

    inline void CtrlReconstruct();//���¿ؼ����Σ�û��������������ʹ��

    double m_mousex = -1, m_mousey = -1;
    QWND* FocusCtrl = NULL;           //����ؼ�����Ҫ��Ӧ���̵�
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
private:
    //����
    void Test();

public:
    //�û��ӿ�
    int InsertCtrl(QWND* ctrl, unsigned int weight = 0, int index = -1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};

#define __QTLAYOUT__
//ƽ�̲�����
class QTLayout :public QLayout
{

    bool isChange = true;
    QWND* pWnd = nullptr;
    std::shared_mutex m_Draw_lk;
public:
    QTLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���
    virtual void Reconstruct(HDC sdc);                                          //���´���
    virtual ~QTLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
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
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //���ô�С(�����¹����ؼ�)
    virtual int Type()override { return 3; };//ˮƽ���ַ���1����ֱ���ַ���2��ƽ�̲��ַ���3
private:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************

    inline void CtrlReconstruct();//���¿ؼ����Σ�û��������������ʹ��


private:
    //����
    void Test();

public:
    //�û��ӿ�
    int SetCtrl(QWND* ctrl);//����ؼ���weight���ؼ�����Ȩ�أ�
    int RemoveCtrl();//�Ƴ��ؼ�
    QWND* GetCtrl();
};



#define __QSTATICTEXT__
class QStaticText :public QWND
{
    std::shared_mutex m_dlk;
    //inline void QCreateDC(HDC winddc);
    //inline void ReleaseDC();
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//����һ��ͼ��
    IWICImagingFactory* pWICFactory = NULL;

    std::wstring m_fontName = L"����"; //��������
    double m_fontSize = 16.0f;//�����С
    
    HCURSOR m_hCursorBuff = NULL;
    bool isChange = true;
    unsigned int m_fontColor = 0xff000000;
    unsigned int m_bkColor = 0xffd0d0d0;
    std::wstring m_text = L"Static";
public:
    QStaticText(HDC winddc, const GRect& rf);
    virtual ~QStaticText();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���

    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;

public:
    //�û��ӿ�
    void SetText(const wchar_t* t);
    void SetFontName(const wchar_t* t);
    void SetFontSize(double size);
    void SetFontColor(unsigned int color);
    void SetBKColor(unsigned int color);
    const wchar_t* GetText();

};



//----------------------------------------------------------------------------------------------------------------------------------------------�ؼ�������
#define __QCTRLTOOLBOX__
#include "QFButton.h"
#include "QFPButton.h"
class QCtrlToolbox :public QCtrlBaseD2D
{
    
protected:
    std::shared_mutex m_drawLK;
    QVLayout* __qvl = nullptr;
    QVLayout* m_ctrlqvl = nullptr;
    QVLayout* m_attrlqvl = nullptr;
    QVLayout* m_fileqvl = nullptr;
    QTabs* m_pTable = nullptr;
    unsigned int m_bkColor = 0xff000000;
    //�ļ�ҳ�ڵĿؼ�
    std::vector<QWND*> m_mangerCtrl;
    QPButton* m_OpenFile = nullptr;
    QPButton* m_SeveFile = nullptr;

public:
    QCtrlToolbox(HDC winddc, const GRect& rf);
    virtual ~QCtrlToolbox();
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ

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
public:
    //�û��ӿ�
    int InsertCtrl(QWND* ctrl, unsigned int weight = 0, int index = -1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    int InsertAttributeCtrl(QWND* w,int index=-1);       //����һ�����Կؼ�
    int RemoveAttributeCtrl(int index);       //�Ƴ�һ�����Կؼ�
    void ClearAttributeCtrl();       //�Ƴ�һ�����Կؼ�
    void SetFileButtonEvent(QFBUTTONCLICK en, int index = 0);

private:
    // ���ؼ���С���͸ı��¼�
    void ParentCtrlSizeChangeEvent(int w, int h);
};

//----------------------------------------------------------------------------------------------------------------------------------------------���崰��

#define __QEDITORWIN__
typedef int (*QEDITORWIN_LBDOWN)(int,int ,int);
typedef int (*QEDITORWIN_PLACE_CTRL)(int, int, int,int,int);
typedef void (*QEDITORWIN_DELETE_CTRL)(int);
typedef void (*QEDITORWIN_SELECT_CTRL)(int);

class QEditorWin :public QCtrlBaseD2D
{
    QEDITORWIN_LBDOWN __qLBDown = nullptr;
    QEDITORWIN_PLACE_CTRL __qPCtrl = nullptr;
    QEDITORWIN_DELETE_CTRL __qDeleteCtrl = nullptr;
    QEDITORWIN_SELECT_CTRL __qSelectCtrl = nullptr;
protected:
    std::shared_mutex m_drawLK;
    std::shared_mutex m_ctrlLK;
    std::vector<QWND*> m_QWNDs;
    bool m_sizeChaneg = false;
   
    int m_MouseStartX = -1, m_MouseStartY = -1;
    int m_MouseCurrentX = -1, m_MouseCurrentY = -1;
    static HCURSOR m_hCursorWE;
    static HCURSOR m_hCursorNS;
    static HCURSOR m_hCursorARROW;
    static HCURSOR m_hCursorSIZEALL;
    QWND* m_selectCtrl = nullptr;//��ѡ�еĿؼ�
    QWND* m_selectCtrl_h = nullptr;//ѡ�пؼ��ļ����ؼ�
    QWND* FocusCtrl = nullptr;
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
    bool isDown = false;
    bool isSelect = false;//��ǰ�ؼ��Ƿ�ѡ��
    struct {
        int flag = -1;//0Ϊ�ϱߣ�1Ϊ�ұߣ�2Ϊ�±ߣ�3Ϊ��ߣ�4Ϊ�м�,5���ƾ���
        float dta = -1;//����ʱ������һ�ߵľ���
        float dta2 = -1;//���þ���

    }__DownData;
    unsigned int m_bkColor = 0xeeffffff;
    unsigned int m_TitleBKColor = 0xffc0c0c0;
    int m_titleW = 24;
    HDC m_clientDC = NULL;                 //��������dc
    HBITMAP m_clientHB = NULL;              //
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
public:

    QEditorWin(HDC winddc, const GRect& rf);
    virtual ~QEditorWin();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ

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
    QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//���̵���ĳ��
private:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//


public:
    //�û��ӿ�
    int SetLButtonEvent(QEDITORWIN_LBDOWN ent);
    void AttachCtrl(QWND* w);       //����һ���ؼ�
    void RemoveCtrl(QWND* w);       //�Ƴ�һ���ؼ�


    int SetPlaceCtrlEvent(QEDITORWIN_PLACE_CTRL ent);
    int SetDeleteCtrlEvent(QEDITORWIN_DELETE_CTRL ent);
    int SetSelectCtrlEvent(QEDITORWIN_SELECT_CTRL ent);
};
#define __QSWITCH__
typedef int (*QSWITCH_STATE_CHANGE)(int);

class QSwitch :public QCtrlBaseD2DIPD
{
    //״̬�ı�ص�����

    QSWITCH_STATE_CHANGE __State_ChangeEvent = nullptr;

    unsigned char __state=0;
    bool __isInctrl =false;//����Ƿ��ڿؼ���
    std::chrono::steady_clock::time_point __startT;
    bool __isMutex = false;

    BOOL __Contains(IN INT x, IN INT y)const;
public:
    QSwitch(HDC winddc, const GRect& rf);
    ~QSwitch();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual BOOL Contains(IN const GPoint& pt)const override;
    //�Ƿ����ĳ��
    virtual BOOL Contains(IN INT x,IN INT y) const override;
    virtual int MouseMoveOut();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
public:
    //�û��ӿ�
    int SetStateChangeEvent(QSWITCH_STATE_CHANGE ev);
    int GetState()const;
    void SetState(int state);
    void IsMutex(bool im);



};

#define __QSLIDER__
typedef int (*QSLIDER_VALUE_CHANGE)(int,float);
class QSlider :public QCtrlBaseD2DIPD
{
    unsigned char __direction = 0;//�ؼ�����0Ϊˮƽ��1Ϊ��ֱ
    float __railRatio = 0.3;
    bool __isLDown = false;
    int __lDownPX = -1, __lDownPY = -1;
    unsigned int __railColor = 0xff00f0f0;
    unsigned int __BKColor = 0xffffffff;
    bool __isInBt = false;

  
    float __minValue = 0.0;//��Сֵ
    float __maxValue = 100.0;//���ֵ
    float __currenValue = __minValue;//��ǰֵ

    std::shared_mutex __SegmLK;//�ֶβ�����
    bool __isSegm = false;//�Ƿ�ֶΣ�����ֶ���Ҫ�������µķֶ�ֵ
    std::set<float> __SegmArr;//�ֶε���ֵ
    QSLIDER_VALUE_CHANGE __ValueChangeEvent = nullptr;//��ֵ���͸ı��¼�


  

public:
    QSlider(HDC winddc, const GRect& rf);
    ~QSlider();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMoveOut();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ
private:
    void __ChangeCurrenValue(float v);



public:

    void SetMinValue(float v);
    void SetMaxValue(float v);
    void SetCurrentValue(float v);
    unsigned int SetRailColor(unsigned int color);
    float SetRailRatio(float ato);
    void IsSegmentation(bool b);
    int ClearSegmArr();
    int AddSegmVuale(float v);
    int GetSegmValue(float* arr,int bufL);
    void SetDirection(unsigned char d);
    int RemoveSegmVuale(float v);
    QDWORD SetValueChangeEvent(QSLIDER_VALUE_CHANGE ev);

    unsigned int SetColor(unsigned int color);

};

//������
#define __QPROGRESS__

class QProgress :public QCtrlBaseD2DIPD
{
    unsigned char __direction = 0;//�ؼ�����0Ϊˮƽ��1Ϊ��ֱ
    unsigned int __style = 1;//0:ֱ����������1ΪԲ��������
    float __currentValue = 60;//��ǰ��������λ��
    unsigned int __Color = 0xff00f000;

    unsigned char __decimals = 0;//С��λ��

    cairo_surface_t* __surface = nullptr;//��ͼ����
    cairo_t* __cr = nullptr;//��ͼ����
    unsigned int __ringcolor1 = 0xffffffff;
    unsigned int __ringcolor2 = 0xff00ff00;
    int __currentRW = 1000;//��ǰԲ����ȡɫ���屳��Ŀ��
    int __currentRH = 1000;//��ǰԲ����ȡɫ���屳��ĸ߶�
    float __thickness = 50;//Բ���Ŀ��

    //��ʼ��Բ����ɫ�߳�
    std::thread __threadRColor;
    bool __isThreadRefresh = false;
    void __threadRingColor();
    void __threadDraw_gradient_ring(double centerX, double centerY, double radius, double thickness);
    std::mutex __lockRColor;

public:
    QProgress(HDC winddc, const GRect& rf);
    ~QProgress();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���

private:
    void DrawRingStyle();
    void DrawSLStyle();

 


public:
    //�û��ӿ�
    //���ý���������
    void SetDirection(unsigned char d);
    //���ý�������ʽ
    void SetStyle(unsigned int style);
    //��ý�������ʽ
    unsigned int GetStyle();
    //���õ�ǰֵ
    void SetCurrentValue(float v);
    //�������ͽ���������ɫ
    void SetLineTypeProgressColor(unsigned int color);
    //����С����ʾ��λ��
    void SetDecimalPlaces(unsigned char dp);
    //���û��ν���������ɫ
    void SetRingProgressColor(unsigned int color1,unsigned int color2);
    //����Բ�����
    void SetRingProgressThickness(float thickness);


};

//��ɫ��
#define __QCOLORBOX__
#include "QFPopDialog.h"
typedef int (*QCOLORBOX_CHANGE)(QPOINTER,unsigned int);
class QColorBox :public QCtrlBaseD2DIPD
{
    unsigned int m_color=0xffa00000;//��ǰ��ɫ
    QCOLORBOX_CHANGE m_colorChange = nullptr;

protected:
    class QColorPopDialog :public QPopDialog
    {
    public:
        QColorPopDialog(HDC winddc, const GRect& rf);
        ~QColorPopDialog();
        virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;

    };
    static QColorPopDialog* m_cpDialog;
    static QSlider* m_redQslider;
    static QSlider* m_greeQslider;
    static QSlider* m_blueQslider;
    static QSlider* m_alphaQslider;
    static std::mutex createLK;
    static int __currentCount;
    int m_popDialogHeight = 90;
    int m_popDialogWidth = 120;
    inline void __ReconstructDialog(HDC dc);
public:
    QColorBox(HDC winddc, const GRect& rf);
    ~QColorBox();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ


public:
    friend static int sliderVChange(QPOINTER ct,float v);


public:
    //�û��ӿ�
    unsigned int GetColor();
    unsigned int SetColor(unsigned int color);
    QCOLORBOX_CHANGE SetColorChangeEvent(QCOLORBOX_CHANGE ev);
    void SetPopDialogHeight(int h);
    void SetPopDialogWidth(int w);

};
int sliderVChange(QPOINTER ct, float v);

//�����б��
#define __QSELECT__
typedef int (*QSELECT_SELECTCHANGE)(QPOINTER,int);
class QSelect :public QCtrlBaseD2DIPD
{
protected:
    class QSelectPopDialog :public QPopList
    {
    public:
        QSelectPopDialog(HDC winddc, const GRect& rf);
        ~QSelectPopDialog();


    };
    static QSelectPopDialog* m_spdialog;
    static std::mutex createLK;
    static int __currentCount;
    int m_popDialogHeight = 90;
    int m_popDialogWidth = 120;
    //�������ݵ�ƫ��
    float m_offsetX = 0;
    //�������ݵ�ƫ��Y
    float m_offsetY = 0;
    std::list<PopListObj> m_list;
    std::shared_mutex m_LK_list;

    //������ɫ
    unsigned int m_bkColor = 0xB3b0b0b0;
    //��Ŀ�߶�
    float m_itemHeight = 32;
    //��ǰ��ʾ��Ŀ
    //std::wstring m_text;
    PopListObj* m_currentItem =nullptr;
    int m_index = -1;
    //������ťռ��
    float m_btWato = 0.3;
    //�Ƿ�������б�
    bool m_isOpen=false;

    //��Ŀ��������
    std::wstring m_itemFName = L"����"; 

    //��Ŀ�����С
    double m_itemFSize = 10.0f;

    QSELECT_SELECTCHANGE m_selectEvent = nullptr;

public:
    QSelect(HDC winddc, const GRect& rf);
    ~QSelect();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //���´���

public:
    //�ڲ������Ӧ����
    friend int SelsctChange(QPOINTER ct, int v);

public:
    //�û��ӿ�

    //���õ���߶�
    void SetPopDialogHeight(int h);
    //���õ�����
    void SetPopDialogWidth(int w);
    //����һ����Ŀ
    int InsertItem(const wchar_t* text, int index = -1);
    //�Ƴ�һ����Ŀ
    int RemoveItem(int index);
    //����������ť�Ŀ�ȱ���
    void SetButtonRatio(float rat);
    //������Ŀ�߶�
    void SetItemHeight(float h);
    //������Ŀ��������
    void SetItemFontName(const wchar_t* nm);
    //������Ŀ�����С
    void SetItemFontSize(float size);
    //��ȡѡ�е���Ŀ�ı�
    const wchar_t* GetSelectedText();
    //��ȡ��Ŀ�ı�
    const wchar_t* GetItemText(int index);
    //����ָ����Ŀ������ɫ
    void SetItemBKColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //����ָ����Ŀ������ɫ
    void SetItemFontColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //������Ŀ��ѡ��ʱ�ص�����
    QSELECT_SELECTCHANGE SetSelectChangeEvent(QSELECT_SELECTCHANGE ev);
    //��ȡ��Ŀ��
    size_t GetItemCount();
    //���õ�ǰѡ����Ŀ
    int SetCurrentItem(int index);
    //��ȡ��ǰѡ������
    int GetIndex();
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    int GetItemFontColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    int GetItemBKColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
};
int SelsctChange(QPOINTER ct, int v);
























