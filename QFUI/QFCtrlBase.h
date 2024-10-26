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
    HDC m_bdc = NULL;                 //����dc,�ؼ�����ʱ�����dc�������
    HBITMAP m_bhB = NULL;
    HDC m_bkdc = NULL;                //����dc,���汳��
    HBITMAP m_bkhB = NULL;
    bool isChange = true;
public:
    QCtrlBase(HDC winddc, const GRect& rf);
    virtual ~QCtrlBase();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual void SetLocation(const GPoint& point)override;                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //���ô�С(�����¹����ؼ�)
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
//���ֻ���
class QLayout : public QWND
{
protected:
    static HCURSOR m_hCursorBuff;
    std::chrono::steady_clock::time_point __ct;

public:
    QLayout(HDC winddc, const GRect& rf);
    virtual ~QLayout();
    virtual int Type() { return 0; }//ˮƽ���ַ���1����ֱ���ַ���2
    
};


class QCtrlBaseD2D :public QWND
{

protected:
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//����һ��ͼ��
    IWICImagingFactory* pWICFactory = NULL;
    std::wstring m_fontName = L"����"; //��������
    double m_fontSize = 16.0f;//�����С
    bool isChange = true;
public:
    QCtrlBaseD2D(HDC winddc, const GRect& rf);
    virtual ~QCtrlBaseD2D();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual void SetLocation(const GPoint& point)override;                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //���ô�С(�����¹����ؼ�)
    virtual void Refresh() { isChange = true; };
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
private:




};

//����̬��Ա����
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



//�����ڶ��߳�ͬʱˢ�µĿؼ�
class QCtrlBaseD2DIPD :public QFUIBase
{

protected:
    static std::shared_ptr<D2DObjManage<ID2D1Factory>> __pD2DFactory_SP;
    static std::shared_ptr < D2DObjManage<ID2D1DCRenderTarget>> __pRenderTarget_SP;
    static std::shared_ptr < D2DObjManage<IDWriteFactory>> __pDWriteFactory_SP;
    static std::shared_ptr < D2DObjManage<IWICImagingFactory>> __pWICFactory_SP;
    static std::mutex __SPMakeLK;
    IDWriteTextFormat* pTextFormat = nullptr;
    std::wstring m_fontName = L"����"; //��������
    double m_fontSize = 10.0f;//�����С
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

    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual void SetLocation(const GPoint& point)override;                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //���ô�С(�����¹����ؼ�)
    virtual void Refresh() { isChange = true; };
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual void SetFontName(const wchar_t* t);
    virtual void SetFontSize(double size);
    virtual void SetFontColor(unsigned int color);
private:




};


//˳������������
class QSCWnd
{
public:
    std::vector<QWND*> m_QWNDs;     //����ؼ�
    std::shared_mutex m_shared_lock_ctrl;//�����ؼ�����

    void RemoveCtrl(QWND* w);       //�Ƴ�һ���ؼ�
    virtual int GetSubCtrlCount();//ȡ�ӿؼ���
    virtual QWND* GetSubCtrl(int index);//ȡ�ӿؼ�
protected:
    /*
    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
  
    //std::unique_lock<std::mutex> lock(m_clock);
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    ((QWMange*)w)->m_Parent = (QWND*)this;
    ((QWMange*)w)->m_HostHwnd = m_HostHwnd;
    * 
    */

    void AttachCtrl(QWND* w);       //����һ���ؼ�
};
















#endif







