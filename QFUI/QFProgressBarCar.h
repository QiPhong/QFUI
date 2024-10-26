#pragma once
#include"QFUIWND.h"
#include <mutex>

class QFPBCM 
{
public:
    static HMODULE __srcDLL;
    static Gdiplus::GraphicsPath* __CarPath;
    virtual ~QFPBCM();
};


class QProgressBarCar :public QWND
{
    static QFPBCM __qm;
    HDC m_bdc = NULL;
    HBITMAP m_bhB = NULL;
    std::mutex m_drawLock;
    std::mutex __lock;
    Gdiplus::GraphicsPath* m_CarPath = NULL;

    //HDC m_pdc = NULL;
    //HBITMAP m_phB = NULL;
    Gdiplus::RectF m_path_bounds;
    int pdcW,pdcH;
    std::wstring drawFontName=L"Arial";
    double drawFontSize =12;
    unsigned int drawFontColor = 0xee808080;
    Gdiplus::Brush* fBrush=NULL;
    Gdiplus::StringFormat* m_gsf = NULL;
    double m_rat = 50.0;//当前比例,0-100
    bool isShowDec = true;
public:
    QProgressBarCar(HDC winddc, const GRect& rf);
    virtual ~QProgressBarCar();
    virtual void SetPercentage(double percent);
    virtual double GetPercentage();
    virtual int Draw(HDC sdc = NULL)override;

    virtual void Redraw()override { m_ischange = true; };
    virtual void Refresh()override { m_ischange = true; };
    virtual void SetFontSize(double size);
    virtual void SetFontName(const wchar_t* name);
    virtual void SetFontColor(unsigned int colo);
    virtual void IsShowDec(bool isShow);
    virtual void SetFillBrush(Gdiplus::Brush* brush);
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建
    virtual QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
private:
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();

    bool m_ischange = true;
    void LoadResou();

};



