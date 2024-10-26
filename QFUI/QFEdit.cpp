#include "QFEdit.h"
QEdit::QEdit(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    //��ʼ��һ����ɫʵ�Ļ�ˢ
    m_bkBrush = new GSolidBrush(GColor(0xA0, 0xf5, 0xf5, 0xf5));
    //�߿���ɫ
    m_bodyColo.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(0xff, 0x4E, 0xF2, 0xEA)));
    m_hCursor = LoadCursor(NULL, IDC_IBEAM);
    m_fontBrush = new GSolidBrush(GColor(0xff, 0, 0, 0));
    m_gsf = GStringFormat::GenericTypographic()->Clone();

}
QEdit::~QEdit()
{
    if (m_pBKImage) {
        delete m_pBKImage;
    }
    //�ͷű�����ˢ
    if (m_bkBrush) {
        delete m_bkBrush;
    }
    //�ͷ����廭ˢ
    if (m_fontBrush) {
        delete m_fontBrush;
    }
    delete m_gsf;

}
int QEdit::Draw(HDC sdc)
{
    int ret = 0;
    if (isChange ) {
        ret = 1;
        isChange = false;
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);

        //2.���¿ؼ�����
        Gdiplus::Graphics graphics(m_bdc);
        {//���Ʊ���
            if (m_bkBrush)
                graphics.FillRectangle(m_bkBrush, 0, 0, m_rect.Width, m_rect.Height);
        }

        //�Ƿ��б���ͼƬ
        if (m_pBKImage) {
            graphics.DrawImage(m_pBKImage, GRect(0, 0, m_rect.Width, m_rect.Height), 0, 0, m_pBKImage->GetWidth(), m_pBKImage->GetHeight(), Gdiplus::UnitPixel, &imageAttr);
        }

        {//��������
             //д�϶�Ӧ������


            GRectF drawRect(__drawStartpos, 0, m_rect.Width - __drawStartpos, m_rect.Height);
            if (0.0f == __drawStartpos)drawRect.X = 2;
            // GStringFormat gsf(0,0); 
            // gsf.SetLineAlignment(slAlign);//��ֱ����
            // gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); 
            GFont drawFont(m_fontName.c_str(), m_fontSize);
            m_gsf->SetLineAlignment(slAlign);//��ֱ����
            m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
            __FontHeight = drawFont.GetHeight(&graphics);
            if (__FontHeight > m_rect.Height) {
                __FontHeight = m_rect.Height;
            }
            graphics.DrawString(m_text.c_str(), (INT)m_text.length(), &drawFont, drawRect, m_gsf, m_fontBrush);

            // Gdiplus::SizeF layoutSize(__FontHeight*10, __FontHeight*10);
            // GRectF strRect;
            // graphics.MeasureString(m_text.c_str(), -1, &drawFont,GPointF(0,0),&gsf, &strRect);
            // printf("%f,%f",strRect.Width,strRect.Height);
            // m_CaretPos=strRect.Width;


        }
        {//���Ʊ߿�
            GColor pcolor(0, 0, 0);
            GPen pen(m_bodyColo, 2);
            if (!m_isFocus)
                pen.SetColor(pcolor);
            GRect rect(0, 0, m_rect.Width - 1, m_rect.Height - 1);

            graphics.DrawRectangle(&pen, rect);

        }
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);

    }



    QCtrlBase::Draw(sdc);

    if (m_isFocus) {//���Ʋ����
        if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
        uint64_t  dta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_CaretUpdateTime).count();
        if (m_CaretPeriod < dta) {
            m_CaretUpdateTime = std::chrono::high_resolution_clock::now();
            if (1 == m_CaretState) {
                m_CaretState = 0;
            }
            else {
                m_CaretState = 1;
            }
        }

        if (1 == m_CaretState) {
            Gdiplus::Graphics graphics(sdc);

            Gdiplus::Pen pen(GColor(0, 0, 0));
            int Ctop = (m_rect.Height - __FontHeight) / 2;
            int start = 0;
            if (0.0f == __drawStartpos)start = 2;
            graphics.DrawLine(&pen, GPoint(m_rect.X + m_CaretPos + start, m_rect.Y + Ctop), GPoint(m_rect.X + m_CaretPos + start, m_rect.Y + m_rect.Height - Ctop));

        }
        ret = 1;


    }
    return ret;

}
inline void QEdit::SetBKImageTransparency(unsigned char apha)
{
    Gdiplus::ColorMatrix colorMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, (float)apha / 0xff, 0.0f,  // ����͸����ͨ��Ϊ0.5
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    imageAttr.SetColorMatrix(&colorMatrix);

    isChange = true;

}
inline void QEdit::__LoadPicture(const std::wstring& path)
{
    //���ر���
    m_pBKImage = new GImage(path.c_str());
    isChange = true;


}
void QEdit::LoadPicture(const std::wstring& path)
{

    __LoadPicture(path);

}
void QEdit::SetBKBrush(GBrush* brush)
{
    if (m_bkBrush)delete m_bkBrush;
    m_bkBrush = brush->Clone();

}
int QEdit::MouseMove(int x, int y, WPARAM wParam)
{
    // //�ı������
    // if(QWND::Contains(pt)){
    //     //���ɲ�����
    //     // HCURSOR hCursor = LoadCursor(NULL, IDC_IBEAM);
    //     // m_Parent->SetCursor(LoadCursor(NULL, IDC_IBEAM));//IDC_ARROW
    //     // SetClassLongPtr(HWND(0x0022185A), GCLP_HCURSOR, (LONG_PTR)hCursor);
    //     // SetCursor(hCursor);
    //     // HCURSOR hCursor = LoadCursorFromFileW(L"custom_cursor.cur");
    //     // SetSystemCursor(hCursor, 32513);

    // }
    // else{
    //     //�ָ����
    //     // m_Parent->SetCursor(NULL);
    //     // SetClassLongPtr(HWND(0x0022185A), GCLP_HCURSOR, (LONG_PTR)NULL);
    //     // SetCursor(NULL);
    // }
    return 0;
}

int QEdit::KeyChar(WPARAM wParam, LPARAM lParam)
{
    if (!m_isFocus)return 1;//����1��ʾ���ؼ������ǽ�����
    return 0;
}



int QEdit::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{//�����µĿ��ܱ����뷨�ػ�ļ�ȫ�������VK_PROCESSKEY 229
    if (!m_isFocus)return 1;//����1��ʾ���ؼ������ǽ�����
    char gbkString[3] = { 0 };
    gbkString[0] = ((wParam >> 8) & 0xff);
    if (0 == gbkString[0])
        gbkString[0] = (wParam & 0xff);
    else
        gbkString[1] = (wParam & 0xff);
    wchar_t unicodeString[3] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, gbkString, -1, unicodeString, 3); // ת��ΪUnicode�ַ���

    //unicodeString[0] = (static_cast<unsigned char>(gbkString[0]) << 8) | static_cast<unsigned char>(gbkString[1]);//���ʲô����ʹ���������
    __AddChar(unicodeString[0]);
    
    isChange = true;
    return 0;
}

int QEdit::KeyDown(WPARAM wParam, LPARAM lParam)
{
    if (!m_isFocus)return 1;//����1��ʾ���ؼ������ǽ�����
    switch (wParam) {
    case VK_BACK: {
        __RemoveChar();
        break;
    }
    case VK_RIGHT: {//���ƹ��
        if (m_CaretIndex < m_text.length())++m_CaretIndex;

        __upDateCare_up();

        break;
    }
    case VK_LEFT: {//���ƹ��
        if (m_CaretIndex > 0)--m_CaretIndex;

        __upDateCare_down();
        break;
    }
    default:
        //����������������
        __KeyInput(wParam, lParam);
        break;

    }

    return 0;
}
inline int QEdit::__TrailingCharacter(GREAL wf)
{

    if (0.0f == __drawStartpos)wf += 2;
    int l, h, m = 0, o = 0;
    l = 0;
    h = m_text.length();
    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    graphics.MeasureString(m_text.c_str(), h, &drawFont, GPointF(0, 0), m_gsf, &strRect);
    if (strRect.Width <= wf)return h;
    int state = 0;
    int dta = strRect.Width;

    do {
        // printf("///////////////  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);

        m = l + (h - l) / 2;
        graphics.MeasureString(m_text.c_str(), m, &drawFont, GPointF(0, 0), m_gsf, &strRect);
        if (wf < strRect.Width) {
            h = m - 1;
            state = 0;

            // printf("*****************  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
            if (dta < abs(strRect.Width - wf)) {
                // printf("%d",o);


            }
            else {
                dta = abs(strRect.Width - wf);
                o = m;

            }

        }
        else if (wf > strRect.Width) {
            l = m + 1;
            state = 1;
            // printf("-----------------  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
            if (dta < abs(strRect.Width - wf)) {
                // printf("%d",o);

            }
            else {
                dta = abs(strRect.Width - wf);
                o = m;
            }
        }

    } while (h > l);

    if (o < m_text.length() - 1) {
        graphics.MeasureString(m_text.c_str(), o + 1, &drawFont, GPointF(0, 0), m_gsf, &strRect);
        if (dta > abs(strRect.Width - wf)) {
            dta = abs(strRect.Width - wf);
            ++o;
        }
    }
    if (0 < o) {
        graphics.MeasureString(m_text.c_str(), o - 1, &drawFont, GPointF(0, 0), m_gsf, &strRect);
        if (dta > abs(strRect.Width - wf)) {
            dta = abs(strRect.Width - wf);
            --o;
        }

    }

    return o;
    // if(0 == state){//�����0�˳�ѭ������ô�϶������λ�õ���һ���ַ�
    //     //Ҫ�������һ����λ���Ǹ����������λ��
    //     GREAL w = strRect.Width;
    //     graphics.MeasureString(m_text.c_str(), h, &drawFont,GPointF(0,0),m_gsf, &strRect);
    //     if(w +strRect.Width > 2*wf){
    //         printf("++++++++++++++  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
    //         return h;

    //     }else{
    //         printf("&&&&&&&&&&&&&  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
    //         return o;

    //     }
    // }else if(1 == state){
    //     GREAL w = strRect.Width;
    //     graphics.MeasureString(m_text.c_str(), l, &drawFont,GPointF(0,0),m_gsf, &strRect);
    //     if(w +strRect.Width > 2*wf){
    //         printf("^^^^^^^^^^^^^^^  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
    //         return l;

    //     }else{
    //         printf("$$$$$$$$$$$$$$$$  %d,%d,%d,%f,%f",m,l,h,wf,strRect.Width);
    //         return o;

    //     }
    // }



}

int QEdit::LButtonDown(int x, int y, WPARAM wParam)
{
    // m_CaretPos = x-m_rect.X;
    m_isFocus = true;

    //���������µĹ��λ��
    {
        GREAL xf = (GREAL)(x - m_rect.X) - __drawStartpos;//�ȶ��ڿ�ʼ��λ��
        //���ַ��鳤��
        m_CaretIndex = __TrailingCharacter(xf);

        m_gsf->SetLineAlignment(slAlign);//��ֱ����
        m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
        GFont drawFont(m_fontName.c_str(), m_fontSize);
        GRectF strRect;
        Gdiplus::Graphics graphics(m_bdc);
        graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);
        m_CaretPos = strRect.Width + __drawStartpos;

        isChange = true;
        if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
    }


    return 3;//��ȡ���̽��㣬�Ҳ������

}
void QEdit::killFocus()
{
    m_isFocus = false;
    isChange = true;

}

void QEdit::setText(const wchar_t* text)
{
    m_text = text;
    m_CaretIndex = m_text.length();
    isChange = true;
    if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
}
void QEdit::setText(const char* text)
{
    int unicodeLen = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);
    wchar_t* unicodeString = new wchar_t[unicodeLen + 1];
    unicodeString[unicodeLen] = 0;
    MultiByteToWideChar(CP_ACP, 0, text, -1, unicodeString, unicodeLen);

    m_text = unicodeString;
    m_CaretIndex = m_text.length();
    delete unicodeString;
    isChange = true;
    if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);

}
inline const std::wstring& QEdit::getText()const
{

    return m_text;
}
inline void QEdit::setFontName(const wchar_t* fName)
{
    m_fontName = fName;
    isChange = true;
}
inline void QEdit::setFontSize(INT size)
{
    m_fontSize = size;
    isChange = true;
}
void QEdit::setFontBrush(const GBrush& brush)
{
    if (m_fontBrush) {
        delete m_fontBrush;
    }
    m_fontBrush = brush.Clone();
    isChange = true;
}

inline void QEdit::__KeyInput(WPARAM wParam, LPARAM lParam)
{
    char c = QFKT::KeyValue2char(wParam, lParam);
    if (0 != c) {//��������ַ�����
        char tem[2] = { 0 };
        tem[0] = c;
        wchar_t unicodeString[2] = { 0 };
        MultiByteToWideChar(CP_ACP, 0, tem, -1, unicodeString, 2); // ת��ΪUnicode�ַ���
        __AddChar(unicodeString[0]);
        return;

    }
    //������ǿ����ַ�������ִ�а������ܣ����´���������
    // printf("%x",wParam);
    if ((0x8000 & GetKeyState(VK_CONTROL))) {//����CTRL
        switch (wParam) {
        case 0x41 + 2: {//C��
          
            auto fun = [&]() {
                if (OpenClipboard(NULL)) {	//����Ȩ�鵱ǰ��������ӵ��
                    EmptyClipboard();		//��ռ���������
                    int len = m_text.length() * 2;
                    HGLOBAL hemn = GlobalAlloc(GMEM_MOVEABLE, len + 2);	//�����ڴ�
                    if (hemn != NULL) {
                        LPSTR lpstr = (LPSTR)GlobalLock(hemn);	//��ס�ڴ�
                        memcpy(lpstr, m_text.c_str(), len);	//copy
                        lpstr[len + 1] = 0;	//������
                        lpstr[len + 2] = 0;	//������
                        GlobalUnlock(hemn);		//�ͷ���
                        SetClipboardData(CF_UNICODETEXT, hemn);	//д������
                    }
                    CloseClipboard();				//�رռ�����
                }
                };
            std::thread t(fun);
            t.detach();
        }
                     break;
        case 'V': {
            auto fun = [&]() {
                if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {//Unicode�ַ�����
                    if (OpenClipboard(NULL)) {
                        HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
                        if (hMem) {
                            //  ��ȡ�ַ���    
                            wchar_t* lpStr = (wchar_t*)GlobalLock(hMem);

                            if (lpStr) {
                                std::wstring Str(lpStr);
                                m_text.insert(m_CaretIndex, Str);
                                m_CaretIndex += Str.length();//�����λ�ñ仯
                                if (__ChangeEvent.fun) {
                                    __ChangeEvent.fun((int)this, (int)m_text.c_str(), __ChangeEvent.addParam);
                                }
                                __UpdateCarePos();
                                // �ͷ��ڴ���    
                                GlobalUnlock(hMem);
                            }
                        }
                        // �رռ��а�    
                        CloseClipboard();
                    }
                }
                else if (IsClipboardFormatAvailable(CF_TEXT)) {//ANSI�ַ�����

                }
                };
            std::thread t(fun);
            t.detach();
        }
                break;

        }

    }



}
inline void QEdit::__AddChar(wchar_t wc)
{
    // wchar_t w[2]={0};
    // w[0]=wc;
    m_text.insert(m_text.begin() + m_CaretIndex, wc);
    ++m_CaretIndex;
    // GStringFormat gsf = ; 
    // gsf.SetLineAlignment(slAlign);//��ֱ����
    // gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap|Gdiplus::StringFormatFlags::StringFormatFlagsMeasureTrailingSpaces); //̫����������
    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);

    m_CaretPos = strRect.Width;
    // printf("%f,%f",m_CaretPos,(Gdiplus::REAL)m_rect.Width);
    if (m_CaretPos > m_rect.Width) {//�����ַ���̫����������
        __drawStartpos = (Gdiplus::REAL)m_rect.Width - m_CaretPos;
        m_CaretPos = m_rect.Width;

    }
    isChange = true;
    if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
    if (__ChangeEvent.fun) {
        __ChangeEvent.fun((int)this, (int)m_text.c_str(), __ChangeEvent.addParam);
    }
}

inline void QEdit::__RemoveChar()
{

    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    // wchar_t unicodeString[2] ={0};
    // unicodeString[0] = m_text[m_text.length()-1];
    // m_text=m_text.substr(0,m_text.length()-1);
    --m_CaretIndex;
    if (0 > m_CaretIndex) { m_CaretIndex = 0; return; }
    if (m_CaretIndex < m_text.length())
        m_text.erase(m_CaretIndex, 1);

    graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);
    m_CaretPos = strRect.Width;
    if (m_CaretPos > m_rect.Width) {//�����ַ���̫����������
        __drawStartpos = (Gdiplus::REAL)m_rect.Width - m_CaretPos;
        m_CaretPos = m_rect.Width;
    }
    else {
        if (0 > __drawStartpos) {
            Gdiplus::REAL dta = (Gdiplus::REAL)m_rect.Width - strRect.Width;
            if (dta > -__drawStartpos) {//�����ֵ���ڿ�ʼֵ�ľ���ֵ��ֱ�Ӵ�0.0f��ʼ
                __drawStartpos = 0.0f;
            }
            else {//��������һ��dta
                __drawStartpos = (Gdiplus::REAL)m_rect.Width - strRect.Width;
                m_CaretPos = m_rect.Width;

            }

        }
    }


    isChange = true;
    if (m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
    if (__ChangeEvent.fun) {
        __ChangeEvent.fun((int)this,(int)m_text.c_str(), __ChangeEvent.addParam);
    }

}
inline void QEdit::__upDateCare_up()
{
    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);

    m_CaretPos = strRect.Width;
    // printf("%f,%f",m_CaretPos,(Gdiplus::REAL)m_rect.Width);
    if (m_CaretPos > m_rect.Width) {//�����ַ���̫����������
        __drawStartpos = (Gdiplus::REAL)m_rect.Width - m_CaretPos;
        m_CaretPos = m_rect.Width;

    }
    isChange = true;


}
inline void QEdit::__upDateCare_down()
{

    if (0 > m_CaretIndex) { m_CaretIndex = 0; return; }
    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);
    m_CaretPos = strRect.Width;
    if (m_CaretPos > m_rect.Width) {//�����ַ���̫����������
        __drawStartpos = (Gdiplus::REAL)m_rect.Width - m_CaretPos;
        m_CaretPos = m_rect.Width;
    }
    else {
        if (0 > __drawStartpos) {
            Gdiplus::REAL dta = (Gdiplus::REAL)m_rect.Width - strRect.Width;
            if (dta > -__drawStartpos) {//�����ֵ���ڿ�ʼֵ�ľ���ֵ��ֱ�Ӵ�0.0f��ʼ
                __drawStartpos = 0.0f;
            }
            else {//��������һ��dta
                __drawStartpos = (Gdiplus::REAL)m_rect.Width - strRect.Width;
                m_CaretPos = m_rect.Width;

            }

        }
    }

    isChange = true;

}
inline void QEdit::__UpdateCarePos()
{
    m_gsf->SetLineAlignment(slAlign);//��ֱ����
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap); //̫����������
    GFont drawFont(m_fontName.c_str(), m_fontSize);
    GRectF strRect;
    Gdiplus::Graphics graphics(m_bdc);
    graphics.MeasureString(m_text.c_str(), m_CaretIndex, &drawFont, GPointF(0, 0), m_gsf, &strRect);

    m_CaretPos = strRect.Width;
    // printf("%f,%f",m_CaretPos,(Gdiplus::REAL)m_rect.Width);
    if (m_CaretPos > m_rect.Width) {//�����ַ���̫����������
        __drawStartpos = (Gdiplus::REAL)m_rect.Width - m_CaretPos;
        m_CaretPos = m_rect.Width;

    }
    isChange = true;

}

void QEdit::SetChangeEvent(QEDIT_CHANGE_EVENT even, int param)
{
    __ChangeEvent.fun = even;
    __ChangeEvent.addParam = param;
}




//*************************************************EXPORTS
extern "C" __declspec(dllexport) void QEDSetBKImageTransparency(QEdit* qd, unsigned char apha)
{   
    qd->SetBKImageTransparency(apha);
}

extern "C" __declspec(dllexport) void QEDLoadPicture(QEdit * qd, const wchar_t* path)
{
    qd->LoadPicture(path);
}

extern "C" __declspec(dllexport) void QEDSetBKBrush(QEdit * qd, GBrush * brush)
{

    qd->SetBKBrush(brush);
}
extern "C" __declspec(dllexport) void QEDSetText(QEdit * qd, const wchar_t* text)
{
    qd->setText(text);
}
extern "C" __declspec(dllexport) void QEDSetTextA(QEdit * qd, const char* text)
{
    qd->setText(text);
}

extern "C" __declspec(dllexport) const wchar_t* QEDGetText(QEdit * qd)
{
    return qd->getText().c_str();
}

extern "C" __declspec(dllexport) void QEDSetFontName(QEdit * qd, const wchar_t* fName)
{
    return qd->setFontName(fName);
}
extern "C" __declspec(dllexport) void QEDSetFontSize(QEdit * qd, INT size)
{
    return qd->setFontSize(size);
}
extern "C" __declspec(dllexport) void QEDSetFontBrush(QEdit * qd, GBrush* brush)
{
    return qd->setFontBrush(*brush);
}


extern "C"
{
    __declspec(dllexport) void QEDSetChangeEvent(QEdit* qd, QEDIT_CHANGE_EVENT even, int param)
    {
    
        return qd->SetChangeEvent(even, param);
    }


}

