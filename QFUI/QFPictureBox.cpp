#include "QFPictureBox.h"
QPictureBox::QPictureBox(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
    QCreateDC(winddc);

}
QPictureBox::~QPictureBox()
{
    if (m_pImage) {

        delete m_pImage;
    }
    ReleaseDC();
}
inline void QPictureBox::QCreateDC(HDC winddc)
{
    //创建缓冲DC
    m_bdc = CreateCompatibleDC(winddc);
    m_bhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bdc, m_bhB);

    __LoadPicture();



}
inline void QPictureBox::__LoadPicture()
{
    if (m_pImage) {
        delete m_pImage;
        m_pImage = NULL;
    }
    //加载背景
    m_pImage = new GImage(m_path.c_str());
    m_ischange = true;


}
void QPictureBox::LoadPicture(const std::wstring& path)
{
    m_path = path;
    __LoadPicture();

}
inline void QPictureBox::ReleaseDC()
{
    //释放缓冲dc
    DeleteObject(m_bhB);
    m_bhB = NULL;
    DeleteDC(m_bdc);
    m_bdc = NULL;

}
int QPictureBox::Draw(HDC sdc)
{
    int ret = 0;
    if (m_ischange) {
        std::unique_lock<std::mutex> lk(m_drawLock);
        m_ischange = false;
        ret = 1;
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //绘制
        if (m_pImage) {
            Gdiplus::Graphics graphics(m_bdc);
            // GTextureBrush texture(m_pImage);
            // texture.
            // graphics.FillRectangle(&texture,0,0,m_rect.Width,m_rect.Height);
 
            graphics.DrawImage(m_pImage, GRect(0, 0, m_rect.Width, m_rect.Height), 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight(), Gdiplus::UnitPixel, &imageAttr);
        }


        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);
    }
    QWND::Draw(sdc);
    return ret;
}
inline void QPictureBox::SetTransparency(unsigned char apha)
{
    Gdiplus::ColorMatrix colorMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, (float)apha / 0xff, 0.0f,  // 设置透明度通道为0.5
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    imageAttr.SetColorMatrix(&colorMatrix);

    m_ischange = true;


}

void QPictureBox::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    std::unique_lock<std::mutex> lk(m_drawLock);
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;

    Reconstruct(sdc);
}

void QPictureBox::Reconstruct(HDC sdc)
{
    QWND::Reconstruct(sdc);

    ReconstructDC(m_bdc, m_dc, m_bhB, m_rect);
}

//导出函数
extern "C" __declspec(dllexport) void QPBLoadPicture(QPictureBox * qpb,const wchar_t* path)
{
    return qpb->LoadPicture(std::wstring(path));
}
extern "C" __declspec(dllexport) void QPBSetTransparency(QPictureBox * qpb, unsigned char apha)
{
    return qpb->SetTransparency(apha);
}


