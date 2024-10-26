#pragma once
#include"QFUIWND.h"
#include <mutex>
#define __QFPICTURE_TYPEDEF
#ifdef __QFPICTURE_TYPEDEF

#endif
//ͼƬ��ؼ�����Ҫ���㻺�壬���Բ��ô�QCtrlBase
class QPictureBox :public QWND
{
    HDC m_bdc = NULL;
    HBITMAP m_bhB = NULL;
    Gdiplus::ImageAttributes imageAttr;
    std::mutex m_drawLock;
public:
    QPictureBox(HDC winddc, const GRect& rf);
    virtual ~QPictureBox();
    std::wstring m_path = L"";
    virtual int Draw(HDC sdc = NULL)override;
    void LoadPicture(const std::wstring& path);
    inline void SetTransparency(unsigned char apha);
    virtual void Redraw() { m_ischange = true; };
    virtual void Refresh() { m_ischange = true; };
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
private:
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    inline void __LoadPicture();
    bool m_ischange = true;
    // unsigned char m_Apha=0xFF;//͸����0-255
    GImage* m_pImage = NULL;
};

