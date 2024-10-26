#include "QFButton.h"

QButton::QButton(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    // m_LB = Gdiplus::LinearGradientBrush(GPointF(m_rect.Width/2,0.0),GPointF(m_rect.Width/2, m_rect.Height),);
    // m_LB.SetLinearColors(GColor,GColor(180,150,150,150));
    // m_LB.SetBlendBellShape

    color1.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(180, 0xF2, 0xF2, 0xF2)));
    color2.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(180, 0XEC, 0XEC, 0XEC)));

    color3.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(180, 0XDB, 0XDB, 0XDB)));
    color4.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(180, 0XCF, 0XCF, 0XCF)));
}
QButton::~QButton()
{


}

int QButton::Draw(HDC sdc)
{
    bool flag = 0;

    //添加绘制代码
    if (isChange) {
        flag = 1;
        isChange = false;
        //1.sdc中对应的位置拷贝过来
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);

        //2.更新控件内容
        Gdiplus::Graphics graphics(m_bdc);
        // Gdiplus::LinearGradientBrush lb(GPointF(m_rect.Width/2,0.0),GPointF(m_rect.Width/2, m_rect.Height),color1,color2);
        // graphics.FillRectangle(&lb, 0, 0, m_rect.Width, m_rect.Height/2);


        // Gdiplus::LinearGradientBrush lb2(GPointF(m_rect.Width/2,0.0),GPointF(m_rect.Width/2, m_rect.Height),color3,color4);

        // graphics.FillRectangle(&lb2, 0, m_rect.Height/2, m_rect.Width, m_rect.Height/2);
        GRectF rectf(0, 0, m_rect.Width, m_rect.Height);

        Gdiplus::LinearGradientBrush bh(GPointF(rectf.Width / 2.0, 0), GPointF(rectf.Width / 2.0, rectf.Height)
            , color1, color2);
        GColor ca[4];
        ca[0] = color1;
        ca[1] = color2;
        ca[2] = color3;
        ca[3] = color4;
        Gdiplus::REAL pos[] = { 0.0f,0.45f,0.55f,1.0f };
        bh.SetInterpolationColors(ca, pos, 4);
        graphics.FillRectangle(&bh, rectf);
        {
            //写上对应的文字
            GSolidBrush gsb(GColor(255, 0, 0, 0));

            GRectF drawRect(0, 0, m_rect.Width, m_rect.Height);
            GStringFormat gsf(0, 0);
            gsf.SetAlignment(sAlign);//水平居中
            gsf.SetLineAlignment(slAlign);//垂直居中
            GFont drawFont(m_fontName.c_str(), m_fontSize);
            graphics.DrawString(m_text.c_str(), (INT)m_text.length(), &drawFont, drawRect, &gsf, &gsb);
        }
        if (0 == m_Status) {

            //如果鼠标不在按钮，绘制暗度
            GSolidBrush gbs(GColor(0x20, 0, 0, 0));
            graphics.FillRectangle(&gbs, 0, 0, m_rect.Width, m_rect.Height);
        }
        else {
            GPen pen(GColor(0xff, 0x4E, 0xF2, 0xEA),0.1);
            GRectF rect(0, 0, m_rect.Width, m_rect.Height);
           
            rect.Width -=1;
            rect.Height -=1;
            graphics.DrawRectangle(&pen, rect);
        }



        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);

    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;

}
inline void QButton::ChangeStatus(int satus)
{
    m_Status = satus;
    isChange = true;

}
int QButton::MouseMove(int x, int y, WPARAM wParam)
{

    int status = 0;

    if (m_rect.Contains(x, y)) {
        status = 1;
    }
    if (status != m_Status) {
        ChangeStatus(status);

    }
    return 0;
}
int QButton::LButtonDown(int x, int y, WPARAM wParam)
{
    if (m_rect.Contains(x, y)) {
        m_Status = 0;
        if (m_clickEvent)m_clickEvent(this);
        m_Status = 1;
    }
    return 0;
}




void QButton::SetText(const wchar_t* t)
{
    m_text = t;

    isChange = true;
}
const wchar_t* QButton::GetText()
{
    return m_text.c_str();
}
void QButton::SetFontName(const wchar_t* name)
{
    m_fontName = name;

    isChange = true;
}
void QButton::SetFontSize(INT size)
{
    m_fontSize = size;

    isChange = true;
}
void QButton::SetTextAlgn(const Gdiplus::StringAlignment& align)
{
    sAlign = align;

    isChange = true;
}
void QButton::SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign)
{
    sAlign = align;
    slAlign = lalign;

    isChange = true;
}
void QButton::SetTextAllgn(const Gdiplus::StringAlignment& lalign)
{
    slAlign = lalign;

    isChange = true;
}

int QButton::MouseMoveOut()
{
    m_Status = 0;
    isChange = true;
    return 0;
}

void QButton::SetClickEvent(QFBUTTONCLICK fun)
{
    m_clickEvent = fun;
}



extern "C" {
    __declspec(dllexport) void QFBSetClickEvent(QButton* qw, QFBUTTONCLICK f)
    {
        return qw->SetClickEvent(f);
    }

    __declspec(dllexport) void QFBSetText(QButton* qw, const wchar_t* t)
    {
        return qw->SetText(t);
    }


    __declspec(dllexport) void QFBSetFontName(QButton* qw, const wchar_t* t)
    {
        return qw->SetFontName(t);
    }


    __declspec(dllexport) void QFBSetFontSize(QButton* qw, INT size)
    {
        return qw->SetFontSize(size);
    }

    __declspec(dllexport) void QFBSetTextAlgn(QButton* qw, int index)
    {
        if (index < 0 || 2 < index)
            return qw->SetTextAlgn(Gdiplus::StringAlignment(index));
    }

    __declspec(dllexport) void QFBSetTextAllgn(QButton* qw, int index)
    {
        if (index < 0 || 2 < index)
            return qw->SetTextAllgn(Gdiplus::StringAlignment(index));
    }
    __declspec(dllexport) const wchar_t* QFBGetText(QButton* qw)
    {
    
        return qw->GetText();
    }

}


