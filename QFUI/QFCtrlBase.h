#ifndef __QFCTRLBASE_H
#define __QFCTRLBASE_H
#include "QFUIWND.h"
#include "QFUIBase.h"
#include <windows.h>
#include <vector>


template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}


class QCtrlMouseListener
{
public:



};

class QCtrlBase :public QWND
{

protected:
    HDC m_bdc = NULL;                 //缓冲dc,控件更新时往这个dc输出内容
    HBITMAP m_bhB = NULL;
    HDC m_bkdc = NULL;                //背景dc,保存背景
    HBITMAP m_bkhB = NULL;
    bool isChange = true;
public:
    QCtrlBase(HDC winddc, const GRect& rf);
    virtual ~QCtrlBase();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual void SetLocation(const GPoint& point)override;                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //设置大小(会重新构建控件)
    virtual void Refresh() { isChange = true; };
    virtual QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
private:
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    void UpdateBK(const GBrush& brush);
    


};


HRESULT LoadBitmapFromFile(PCWSTR uri, ID2D1DCRenderTarget* prt, IWICImagingFactory* pcif, ID2D1Bitmap** pBitmap);



class QContainer :public QCtrlBase
{
public:
    QContainer(HDC winddc, const GRect& rf);
    virtual ~QContainer();
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);

};
//布局基类
class QLayout : public QWND
{
protected:
    static HCURSOR m_hCursorBuff;
    std::chrono::steady_clock::time_point __ct;

public:
    QLayout(HDC winddc, const GRect& rf);
    virtual ~QLayout();
    virtual int Type() { return 0; }//水平布局返回1，垂直布局返回2
    
};


class QCtrlBaseD2D :public QWND
{

protected:
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//创建一个图层
    IWICImagingFactory* pWICFactory = NULL;
    std::wstring m_fontName = L"黑体"; //字体名称
    double m_fontSize = 16.0f;//字体大小
    bool isChange = true;
public:
    QCtrlBaseD2D(HDC winddc, const GRect& rf);
    virtual ~QCtrlBaseD2D();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual void SetLocation(const GPoint& point)override;                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //设置大小(会重新构建控件)
    virtual void Refresh() { isChange = true; };
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
private:




};

//管理静态成员对象
template<typename T>
class D2DObjManage
{
public:
    D2DObjManage(T* p) { ptr = p; };
    T* ptr =nullptr;
    ~D2DObjManage() { 
        SafeRelease(&ptr);
    }

};



//不能在多线程同时刷新的控件
class QCtrlBaseD2DIPD :public QFUIBase
{

protected:
    static std::shared_ptr<D2DObjManage<ID2D1Factory>> __pD2DFactory_SP;
    static std::shared_ptr < D2DObjManage<ID2D1DCRenderTarget>> __pRenderTarget_SP;
    static std::shared_ptr < D2DObjManage<IDWriteFactory>> __pDWriteFactory_SP;
    static std::shared_ptr < D2DObjManage<IWICImagingFactory>> __pWICFactory_SP;
    static std::mutex __SPMakeLK;
    IDWriteTextFormat* pTextFormat = nullptr;
    std::wstring m_fontName = L"黑体"; //字体名称
    double m_fontSize = 10.0f;//字体大小
    bool isChange = true;
    unsigned int m_fontColor = 0xff000000;
    std::shared_ptr<D2DObjManage<ID2D1Factory>> m_pD2DFactory_SP;
    std::shared_ptr < D2DObjManage<ID2D1DCRenderTarget>> m_pRenderTarget_SP;
    std::shared_ptr < D2DObjManage<IDWriteFactory>> m_pDWriteFactory_SP;
    std::shared_ptr < D2DObjManage<IWICImagingFactory>> m_pWICFactory_SP;
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* m_pDWriteFactory;
    IWICImagingFactory* m_pWICFactory;
public:
    QCtrlBaseD2DIPD(HDC winddc, const GRect& rf);
    virtual ~QCtrlBaseD2DIPD();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual void SetLocation(const GPoint& point)override;                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //设置大小(会重新构建控件)
    virtual void Refresh() { isChange = true; };
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual void SetFontName(const wchar_t* t);
    virtual void SetFontSize(double size);
    virtual void SetFontColor(unsigned int color);
private:




};


//顺序容器窗口类
class QSCWnd
{
public:
    std::vector<QWND*> m_QWNDs;     //保存控件
    std::shared_mutex m_shared_lock_ctrl;//操作控件的锁

    void RemoveCtrl(QWND* w);       //移除一个控件
    virtual int GetSubCtrlCount();//取子控件数
    virtual QWND* GetSubCtrl(int index);//取子控件
protected:
    /*
    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//通知改变控件的大小
  
    //std::unique_lock<std::mutex> lock(m_clock);
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    ((QWMange*)w)->m_Parent = (QWND*)this;
    ((QWMange*)w)->m_HostHwnd = m_HostHwnd;
    * 
    */

    void AttachCtrl(QWND* w);       //关联一个控件
};
















#endif







