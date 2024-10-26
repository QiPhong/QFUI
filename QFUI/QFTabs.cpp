#include "QFTabs.h"






#define __READ_LOCK(lk) std::shared_lock<std::shared_mutex> __LOCK(lk)

#define __WRITE_LOCK(lk) std::unique_lock<std::shared_mutex> __LOCK(lk)

inline void QTabs::QCreateDC(HDC winddc)
{
    m_pageDC = CreateCompatibleDC(winddc);
    m_pageHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_pageDC, m_pageHB);
}

inline void QTabs::ReleaseDC()
{
    DeleteObject(m_pageHB);
    m_pageHB = NULL;
    DeleteDC(m_pageDC);
    m_pageDC = NULL;
}

QTabs::QTabs(HDC winddc, const GRect& rf) :QContainer(winddc,rf)
{
    __ct = std::chrono::high_resolution_clock::now();
    m_hCursorBuff = LoadCursor(NULL, IDC_ARROW);
    QCreateDC(winddc);
    m_bkBrush = new GSolidBrush(GColor(0xaf, 0xaa, 0xaa, 0xaa));
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    HRESULT hr = m_pD2DFactory->CreateDCRenderTarget(&props, &m_pRenderTarget);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    pDWriteFactory->CreateTextFormat(
        m_fontName.c_str(),                // Font family
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,                   // Font size
        L"en-us",                // Locale
        &pTextFormat
    );

    // Set text alignment to center
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    // Set paragraph alignment to center
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中

    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    pTextFormat->SetTrimming(&trimming, nullptr);//防止溢出布局框

    RECT rc(0,0, rf.Width, rf.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//绑定DC输出

    //创建一个图层
    m_pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory));
    //************************************************************************************************
    //测试代码
    //QTabsProperties tp;
    //tp.text = L"测试标签1";
    //LoadBitmapFromFile(L"C:\\Users\\Administrator\\Desktop\\pic.jpg", m_pRenderTarget, pWICFactory, &tp.pBitmap);
    //tp.isFocus = true;
    //
    //__AddTab(tp);
    //LoadBitmapFromFile(L"C:\\Users\\Administrator\\Desktop\\pic.jpg", m_pRenderTarget, pWICFactory, &tp.pBitmap);
    //tp.text = L"标签2";
   
    //__AddTab(tp);
    //LoadBitmapFromFile(L"C:\\Users\\Administrator\\Desktop\\pic.jpg", m_pRenderTarget, pWICFactory, &tp.pBitmap);
    //tp.text = L"测试标签3";
    //__AddTab(tp);
    //************************************************************************************************
    

}

QTabs::~QTabs()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
    SafeRelease(&pLayer);

    for (auto& it : m_qto.m_tabs) {
        //if (it.plock) {
        //    {
        //        std::unique_lock lock(*it.plock);
        //    }
        //    delete it.plock;
        //
        //}
        if (it->pBitmap) {
            SafeRelease(&it->pBitmap);

        }
        delete it;
    
    }
    if (m_bkBrush) {
        delete m_bkBrush;
        m_bkBrush = nullptr;
    }
#ifdef __SAMPLE_DISPLAY__
    if (__SDbrush)delete __SDbrush;


#endif 


}

int QTabs::Draw(HDC sdc)
{
    bool flag = 0;

    //添加绘制代码
    if (isChange) {
       
        flag = 1;
        auto now = std::chrono::high_resolution_clock::now();
        long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __ct).count();
        if (dta > 5000) {
            isChange = false;
        }
        //1.sdc中对应的位置拷贝过来
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //2.背景绘制
        {

            GRectF rectf(0, 0, m_rect.Width, m_rect.Height);
            Gdiplus::Graphics graphics(m_bdc);
            if (m_bkBrush) {
                graphics.FillRectangle(m_bkBrush, rectf);
            }


        }
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);
        //3.绘制控件
        {
            // Begin drawing
            m_pRenderTarget->BeginDraw();


            // Draw something
            ID2D1SolidColorBrush* pBrush = NULL;
            ID2D1SolidColorBrush* pfBrush = NULL;//字体颜色画刷
            auto test = D2D1::ColorF::Black;
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(__NormalTabColor&0xffffff, __NormalTabColor>>24), &pBrush);
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_fontColor & 0xffffff, m_fontColor >> 24), &pfBrush);
            D2D1_RECT_F layoutRect = D2D1::RectF(__DistancLeft, 0, __DistancLeft+60, 50);
            D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
            double startX = __DistancLeft;
            double maxH = 0;
            __READ_LOCK(m_qto.m_lock);//标签锁
            for (int i = -1; ++i < m_qto.m_tabs.size();) {
                IDWriteTextLayout* pTextLayout = NULL;
                std::unique_lock alk(m_AttributeLock);
                auto& it = m_qto.m_tabs;
                if (it[i]->isUpdateMetrics) {
                    pDWriteFactory->CreateTextLayout(
                        it[i]->text.c_str(),
                        it[i]->text.length(),
                        pTextFormat,
                        renderTargetSize.width,
                        renderTargetSize.height,
                        &pTextLayout);

                    DWRITE_TEXT_METRICS textMetrics;
                    pTextLayout->GetMetrics(&textMetrics);
                   
                    pTextLayout->Release();
                    it[i]->width = textMetrics.width;
                    it[i]->hight = textMetrics.height;
                    it[i]->isUpdateMetrics = false;
                }
                if (maxH < it[i]->hight) {
                    maxH = it[i]->hight;
                }
                if (it[i]->hight < m_qto.maxHeight) {
                    it[i]->hight = m_qto.maxHeight;
                }
                double rad = min(it[i]->width, it[i]->hight);
                double w = it[i]->width * (1+0.2)+ rad;//字体长度+预留+图片长度

                layoutRect.left = startX;
                layoutRect.right = startX + w;
                layoutRect.bottom = it[i]->hight;
                
               

                D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                    layoutRect, // Rectangle coordinates
                    rad*0.2,                          // Radius for the rounded corners
                    rad * 0.2                           // Radius for the rounded corners
                );
                it[i]->roundedRect = roundedRect;
                if (it[i] != m_qto.m_FocusTab) {
                    m_pRenderTarget->FillRoundedRectangle(roundedRect, pBrush);
                }

                //绘制标签图标
                {
                    if (it[i]->pBitmap) {
                        ID2D1EllipseGeometry* pEllipseGeometry = NULL;
                        m_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(layoutRect.left + rad / 2, layoutRect.top + rad / 2),
                            rad / 2, rad / 2 )
                            , &pEllipseGeometry);
                        //开始图层
                        m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pEllipseGeometry,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                            D2D1::IdentityMatrix(),0.8), pLayer);//0.8为透明度

                        //是则图像差值方式，防止拉伸失真，从最接近呈现像素的四个位图像素中插入颜色。
                        D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
                        // 绘制位图并拉伸到目标矩形
                        m_pRenderTarget->DrawBitmap(
                            it[i]->pBitmap,          // 位图对象
                            D2D1::RectF(layoutRect.left-0.5 , layoutRect.top - 0.5, layoutRect.left + rad+0.5, layoutRect.top+rad + 0.5),  // 目标矩形
                            1.0f,             // 不透明度（1.0表示完全不透明）
                            interpolationMode // 插值模式
                        );

                        //停止图层
                        m_pRenderTarget->PopLayer();
                        pEllipseGeometry->Release();

                    }
                    
                
                
                }
                //Draw the text
                layoutRect.left = startX + rad;
                m_pRenderTarget->DrawText(
                    it[i]->text.c_str(),
                    it[i]->text.length(),
                    pTextFormat,             // The text format
                    layoutRect,              // The layout rectangle
                    pfBrush                   // The brush
                );



                startX += w;
            }
            if (m_qto.maxHeight != maxH) {
                m_qto.maxHeight = maxH;
                //通知布局属性控件父控件大小发生改变
                for (auto& its : m_qto.m_tabs) {
                    for (auto& it : its->m_QWNDs) {

                        QWNDAttribute qwa;
                        qwa.AData = it->GetAttribute();
                        if (qwa.Attribute.isLayout) {
                            //发送消息
                            QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height - m_qto.maxHeight) & 0xffffffff);

                            it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//通知改变控件的大小
                        }
                    }
                }
            }
            pBrush->Release();
            pfBrush->Release();
            // End drawing
            m_pRenderTarget->EndDraw();


        }
        //绘制子控件
        {
            BitBlt(m_pageDC, 0, 0, m_rect.Width, m_rect.Height- m_qto.maxHeight, m_dc, 0, m_qto.maxHeight, SRCCOPY);
            Gdiplus::Graphics graphics(m_pageDC);

            Gdiplus::Region region;
            __READ_LOCK(m_qto.m_lock);

            if (m_qto.m_FocusTab) {
                
                std::unique_lock lk(m_qto.m_FocusTab->lock);
                for (auto it : m_qto.m_FocusTab->m_QWNDs) {
                    if (it->Draw(m_pageDC)) {
                        //有控件刷新
                        region.Union(it->GetRect());

                    }
                    else {

                        //如果没刷新，检查下四个顶点是否在刷新矩形内
                        GRect rect = it->GetRect();
                        GPoint pt[4] = { GPoint(rect.X,rect.Y),GPoint(rect.X + rect.Width,rect.Y)
                        ,GPoint(rect.X + rect.Width,rect.Y + rect.Height),GPoint(rect.X,rect.Y + rect.Height) };
                        for (int i = -1; ++i < 4;) {
                            if (region.IsVisible(pt[i], &graphics)) {
                                //如果重叠也要更新
                                it->Refresh();

                                //继续并集这个区域
                                region.Union(it->GetRect());
                            }
                        }

                    }


                }
            }


            BitBlt(m_dc, 0, m_qto.maxHeight, m_rect.Width, m_rect.Height - m_qto.maxHeight, m_pageDC, 0, 0, SRCCOPY);
#ifdef __SAMPLE_DISPLAY__

            if (!__SDbrush) {
                __SDbrush = new GSolidBrush(GColor(0x70, 128, 128, 128));
            }
            Gdiplus::Graphics graphics1(m_dc);
            // 设置背景为透明
            graphics1.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            graphics1.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
            GFont font(L"Arial", 48);
            // 绘制透明的字符
            GRectF rect11(0, 0, m_rect.Width, m_rect.Height);
            GStringFormat gsf(0, 0);
            gsf.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//水平居中
            gsf.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//垂直居中
            graphics1.DrawString(L"展示用例", -1, &font, rect11, &gsf, __SDbrush);
#endif // __SAMPLE_DISPLAY__
        }
    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;
}

int QTabs::MouseMove(int x, int y, WPARAM wParam)
{
  
    m_mousex = x - m_rect.X;
    m_mousey = y - m_rect.Y;
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理

    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_MOUSEMOVE, wParam, pcx, pcy);
    }

    isChange = true;
    return ret;
}
int QTabs::MouseMoveOut()
{
    mouseCapture = NULL;
    isChange = true;
    return 0;
}
int QTabs::KeyChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_CHAR, wParam, lParam);
    return 0;
}
int QTabs::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_IME_CHAR, wParam, lParam);
    return 0;
}
int QTabs::KeyDown(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYDOWN, wParam, lParam);
    return 0;
}
int QTabs::KeyUp(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYUP, wParam, lParam);
    return 0;
}

inline int QTabs::MouseMessage(UINT uMsg, WPARAM wParam, int x, int y)
{
    int ret = 0;
    if (!m_qto.m_FocusTab)return ret;
    POINT pt;
    pt.x = x;
    pt.y = y;

    {
        bool isAtCtrl = false;
        HCURSOR hCursor = m_hCursorBuff;
        __READ_LOCK(m_qto.m_lock);//标签锁
        auto& QWNDs = m_qto.m_FocusTab->m_QWNDs;
        for (auto pit = QWNDs.rbegin(); pit != QWNDs.rend(); ++pit) {
            auto& it = *pit;
            if (mouseCapture && it == mouseCapture) {
                ret = MoseMsg(mouseCapture, pt, uMsg, wParam);//先给捕获鼠标的控件发送一个当前鼠标消息

                if (it->Contains(pt.x, pt.y)) {
                    isAtCtrl = true;
                    hCursor = it->GetCursor();
                    break;
                }
                else if (mouseCapture == mouseFocusCtrl) {
                    mouseFocusCtrl = NULL;
                }
            }
            else if (mouseFocusCtrl) {
                if (it == mouseFocusCtrl) {
                    if (mouseFocusCtrl->m_isEnable && mouseFocusCtrl->m_isVisual) {
                        ret = MoseMsg(mouseFocusCtrl, pt, uMsg, wParam);//先给捕获鼠标的控件发送一个当前鼠标消息
                    }


                    if (it->Contains(pt.x, pt.y)) {
                        isAtCtrl = true;
                        hCursor = it->GetCursor();
                        break;
                    }
                    else {
                        mouseFocusCtrl->MouseMoveOut();
                        mouseFocusCtrl = NULL;
                    }
                }
                else {

                    if (it->Contains(pt.x, pt.y) && it->m_isVisual) {
                        mouseFocusCtrl->MouseMoveOut();
                        ret = MoseMsg(it, pt, uMsg, wParam);
                        mouseFocusCtrl = it;
                        
                        isAtCtrl = true;
                        hCursor = it->GetCursor();
                        break;
                    }
                }
            }
            else if (it->Contains(pt.x, pt.y) && it->m_isVisual && it->m_isEnable) {
                mouseFocusCtrl = it;
               
                ret = MoseMsg(it, pt, uMsg, wParam);
                isAtCtrl = true;
                hCursor = it->GetCursor();
                break;

            }

        }

        //设置对应控件的鼠标
        m_hCursor = hCursor;
  /*      SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
        ::SetCursor(hCursor);*/
        if (!isAtCtrl) {
            //点击了不是控件，原焦点控件失去焦点
            //通知原焦点控件失去焦点

            if ((WM_RBUTTONDOWN == uMsg || WM_LBUTTONDOWN == uMsg)) {
                if (FocusCtrl) {
                    FocusCtrl->killFocus();
                    FocusCtrl = NULL;
                }
                if (mouseSeleFocusCtrl) {
                    mouseSeleFocusCtrl->killFocus();
                    mouseSeleFocusCtrl = NULL;
                }
            }

        }



        // m_QWNDs.erase(m_QWNDs.begin());
    }
    isChange = true;
    return ret;
}

inline int QTabs::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
{
    int ret = 0;
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        //mouseSeleFocusCtrl = qwnd;
        //如果被按下的控件，不是则停止原来的失去焦点
        if (FocusCtrl && (FocusCtrl != qwnd)) {
            FocusCtrl->killFocus();

            FocusCtrl = NULL;


        }
        if (mouseSeleFocusCtrl ) {
            if (mouseSeleFocusCtrl != qwnd) {
                mouseSeleFocusCtrl->killFocus();
                mouseSeleFocusCtrl = qwnd;
            }
        }
        else {
            mouseSeleFocusCtrl = qwnd;
        }
        ret = qwnd->LButtonDown(pt.x, pt.y, wParam);
        if (NULL == mouseCapture && QFUI_CTRL_CAPTURE_MOUSE == ret & 1) {
            mouseCapture = qwnd;
        }
        if (QFUI_CTRL_ACQUIRE_KEYBOARD == (ret & 0x02)) {
            FocusCtrl = qwnd;

        }
        break;
    }
    case WM_LBUTTONUP: {
        qwnd->LButtonUp(pt.x, pt.y, wParam);
        mouseCapture = NULL;
        break;
    }
    case WM_LBUTTONDBLCLK: {
        qwnd->LButtonDBLClick(pt.x, pt.y, wParam);
        break;
    }
    case WM_RBUTTONDOWN: {
        //如果被按下的控件，不是则停止原来的失去焦点
        if (FocusCtrl && (FocusCtrl != qwnd)) {
            FocusCtrl->killFocus();
            FocusCtrl = NULL;
        }
        ret = qwnd->RButtonDown(pt.x, pt.y, wParam);
        if (NULL == mouseCapture && QFUI_CTRL_CAPTURE_MOUSE == ret & 1) {
            mouseCapture = qwnd;
        }
        if (QFUI_CTRL_ACQUIRE_KEYBOARD == ret & 0x02) {
            FocusCtrl = qwnd;
        }
        
        break;
    }
    case WM_RBUTTONUP: {
        qwnd->RButtonUp(pt.x, pt.y, wParam);
        mouseCapture = NULL;
        break;
    }
    case WM_RBUTTONDBLCLK: {
        qwnd->RButtonDBLClick(pt.x, pt.y, wParam);
        break;
    }
    case WM_MBUTTONDOWN: {

        if (NULL == mouseCapture && QFUI_CTRL_CAPTURE_MOUSE == qwnd->MButtonDown(pt.x, pt.y, wParam)) {
            mouseCapture = qwnd;
        }
        break;
    }
    case WM_MBUTTONUP: {
        qwnd->MButtonUp(pt.x, pt.y, wParam);
        mouseCapture = NULL;
        break;
    }
    case WM_MBUTTONDBLCLK: {
        qwnd->MButtonDBLClick(pt.x, pt.y, wParam);
        break;
    }
    case WM_MOUSEMOVE: {
        qwnd->MouseMove(pt.x, pt.y, wParam);
        break;
    }
    case WM_MOUSEWHEEL: {

        qwnd->MouseWheel(pt.x, pt.y, wParam);
        break;
    }

    }
    return ret;
}
inline void QTabs::KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN: {//没有输入发的字符
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyDown(wParam, lParam);
            if (1 == ret) {//该控件不在有焦点
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_KEYUP: {//没有输入发的字符
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyUp(wParam, lParam);
            if (1 == ret) {//该控件不在有焦点
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_IME_CHAR: {//输入法发来的字符
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyIMEChar(wParam, lParam);
            if (1 == ret) {//该控件不在有焦点
                FocusCtrl = NULL;
            }
        }

        return;
    }
    case WM_CHAR: {//输入法发来的字符
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyChar(wParam, lParam);
            if (1 == ret) {//该控件不在有焦点
                FocusCtrl = NULL;
            }
        }
        return;
    }


    }
}
#include <cmath>

// 判断一个点是否在一个D2D1_ROUNDED_RECT内
bool IsPointInRoundedRect(const D2D1_POINT_2F& point, const D2D1_ROUNDED_RECT& roundedRect) {
    // 检查是否在外接矩形内
    if (point.x < roundedRect.rect.left || point.x > roundedRect.rect.right ||
        point.y < roundedRect.rect.top || point.y > roundedRect.rect.bottom) {
        return false;
    }

    // 圆角半径
    float radiusX = roundedRect.radiusX;
    float radiusY = roundedRect.radiusY;

    // 判断四个圆角区域
    if (point.x < roundedRect.rect.left + radiusX && point.y < roundedRect.rect.top + radiusY) {
        // 左上角
        float dx = point.x - (roundedRect.rect.left + radiusX);
        float dy = point.y - (roundedRect.rect.top + radiusY);
        return (dx * dx + dy * dy) <= (radiusX * radiusX);
    }
    else if (point.x > roundedRect.rect.right - radiusX && point.y < roundedRect.rect.top + radiusY) {
        // 右上角
        float dx = point.x - (roundedRect.rect.right - radiusX);
        float dy = point.y - (roundedRect.rect.top + radiusY);
        return (dx * dx + dy * dy) <= (radiusX * radiusX);
    }
    else if (point.x < roundedRect.rect.left + radiusX && point.y > roundedRect.rect.bottom - radiusY) {
        // 左下角
        float dx = point.x - (roundedRect.rect.left + radiusX);
        float dy = point.y - (roundedRect.rect.bottom - radiusY);
        return (dx * dx + dy * dy) <= (radiusX * radiusX);
    }
    else if (point.x > roundedRect.rect.right - radiusX && point.y > roundedRect.rect.bottom - radiusY) {
        // 右下角
        float dx = point.x - (roundedRect.rect.right - radiusX);
        float dy = point.y - (roundedRect.rect.bottom - radiusY);
        return (dx * dx + dy * dy) <= (radiusX * radiusX);
    }

    // 其他区域
    return true;
}



int QTabs::LButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_qto.maxHeight) {

        for (int i = -1; ++i < m_qto.m_tabs.size();) {
            if (IsPointInRoundedRect(D2D1_POINT_2F(px, py), m_qto.m_tabs[i]->roundedRect)
                && m_qto.m_FocusTab!= m_qto.m_tabs[i]) {
                if (m_qto.m_FocusTab)m_qto.m_FocusTab->isFocus = false;
                m_qto.m_FocusTab = m_qto.m_tabs[i];
                m_qto.m_FocusTab->isFocus = true;
                FocusCtrl = NULL;
                break;
            }

        }
    }
    else {
        int pcx = px;
        int pcy = py- m_qto.maxHeight;
        ret = MouseMessage(WM_LBUTTONDOWN, wParam,pcx,pcy);
    }
    isChange = true;

    return ret;
}

void QTabs::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;

    {
        std::unique_lock<std::mutex> lk(m_drawLock);
        m_rect.Width = rect.Width;
        m_rect.Height = rect.Height;

        Reconstruct(sdc);
    }
    
    //QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

    //std::shared_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    //for (auto& it : m_QWNDs) {
    //    it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//通知改变控件的大小
    //}

    isChange = true;
    //Draw(m_sdc);
}

void QTabs::Reconstruct(HDC sdc)
{
    QCtrlBase::Reconstruct(sdc);

    ReconstructDC(m_pageDC, m_dc, m_pageHB, m_rect);
    //发送消息
    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height - m_qto.maxHeight) & 0xffffffff);
   
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    for (auto& its : m_qto.m_tabs) {
        for (auto& it : its->m_QWNDs) {

            it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//通知改变控件的大小
        }
    }
  
    RECT rc(0, 0, m_rect.Width, m_rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//绑定DC输出
}

int QTabs::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_LBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;
    return ret ;
}

int QTabs::LButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_LBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::RButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_RBUTTONDOWN, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::RButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_RBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_RBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::MButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_MBUTTONDOWN, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::MButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_MBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_MBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}

int QTabs::MouseWheel(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;
    if (py < m_qto.maxHeight) {
        //鼠标在标签上的处理
    }
    else {
        int pcx = px;
        int pcy = py - m_qto.maxHeight;
        ret = MouseMessage(WM_MOUSEWHEEL, wParam, pcx, pcy);
    }
    isChange = true;
    return ret;
}



int QTabs::GetSubCtrlCount()
{
   
    __READ_LOCK(m_qto.m_lock);
    if (!m_qto.m_FocusTab)return 0;
 
    return m_qto.m_FocusTab->m_QWNDs.size();
}

void QTabs::killFocus()
{
    if (FocusCtrl) {
        FocusCtrl->killFocus();
    }
}

QWND* QTabs::GetSubCtrl(int index)
{
    __READ_LOCK(m_qto.m_lock);

    QWND* pwnd = nullptr;
    if (!m_qto.m_FocusTab)return pwnd;
    if(index<0 || index> (int)m_qto.m_FocusTab->m_QWNDs.size()-1)return pwnd;
    pwnd = m_qto.m_FocusTab->m_QWNDs[index];
    return pwnd;
}

const wchar_t* QTabs::GetCurrentTabeText()
{
    __READ_LOCK(m_qto.m_lock);
    if (!m_qto.m_FocusTab)return nullptr;
    return m_qto.m_FocusTab->text.c_str();
}

const wchar_t* QTabs::GetCurrentTabeIcoPath()
{
    __READ_LOCK(m_qto.m_lock);
    if (!m_qto.m_FocusTab)return nullptr;
    return m_qto.m_FocusTab->mapPath.c_str();
}

int QTabs::SetCurrentTabeText(const wchar_t* txt)
{
    __WRITE_LOCK(m_qto.m_lock);
    if (!m_qto.m_FocusTab)return -1;
    m_qto.m_FocusTab->text = txt;
    m_qto.m_FocusTab->isUpdateMetrics = true;
    isChange = true;
    return 1;
}

int QTabs::SetCurrentTabeIcoPath(const wchar_t* path)
{
    __WRITE_LOCK(m_qto.m_lock);
    if (!m_qto.m_FocusTab)return -1;
    if (m_qto.m_FocusTab->pBitmap) {
        SafeRelease(&m_qto.m_FocusTab->pBitmap);

    }
    LoadBitmapFromFile(path, m_pRenderTarget, pWICFactory, &m_qto.m_FocusTab->pBitmap);
    m_qto.m_FocusTab->mapPath = path;

    return 1;
}

void QTabs::Redraw()
{
    isChange = true;
    __READ_LOCK(m_qto.m_lock);//标签锁
    for (int i = -1; ++i < m_qto.m_tabs.size();) {
        for (auto& it : m_qto.m_tabs[i]->m_QWNDs) {
            it->Redraw();
        }
       
    
    }
    UpdateWND();
}

void QTabs::__AddTab(QTabsProperties& qtp)
{
    QTabsProperties* p = new QTabsProperties(qtp);
    __WRITE_LOCK(m_qto.m_lock);//标签锁
    if (m_qto.m_tabs.size() < 1)p->isFocus = true;
    m_qto.m_tabs.push_back(p);
    if (p->isFocus) {
        if (m_qto.m_FocusTab)m_qto.m_FocusTab->isFocus = false;
        m_qto.m_FocusTab = m_qto.m_tabs[m_qto.m_tabs.size()-1];
        m_qto.m_FocusTab->isFocus = true;
    }
    

}

#include "QFNormallyCtrl.h"

QDWORD QTabs::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_POINT_IN_QHOSTCTRL: {
        //被请求坐标是否在寄宿控件上
        int x = LParam >> 32;
        int y = LParam & 0xffffffff;

        if (!m_qto.m_FocusTab)return 0;
        __READ_LOCK(m_qto.m_lock);//标签锁
        y -= m_qto.maxHeight;
        auto& QWNDs = m_qto.m_FocusTab->m_QWNDs;
        for (auto pit = QWNDs.rbegin(); pit != QWNDs.rend(); ++pit) {
            
            auto& it = (*pit);

            if (it->Contains(x, y)) {
                //printf("X=%d,Y=%d,R=%d,B=%d :===== >(%d,%d)\n",it->GetRect().X, it->GetRect().Y,
                //it->GetRect().GetRight(), it->GetRect().GetBottom(),x,y);
                QWNDAttribute qwa;
                qwa.AData = it->GetAttribute();
                if (qwa.Attribute.isQHostCtrl) {
                    if (0 != RParam) {
                        QHostCtrl** p = (QHostCtrl**)RParam;
                        *p = (QHostCtrl*)it;
                    
                    }


                    return 1;
                }

                return 0;
            }   
        
        }

    }break;
    case QM_PLACE_CTRL: {
        QWND* p =(QWND*)LParam;
        GRect rect = p->GetRect();
        p->SetLocation(GPoint(rect.X, rect.Y- m_qto.maxHeight));
        AttachCtrlToCurrentTabe(p);
    }return 1;
    case QM_REMOVE_CTRL: {
        QWND* p = (QWND*)LParam;
        __READ_LOCK(m_qto.m_lock);//标签锁
        for (auto& its : m_qto.m_tabs) {
            for (int i = -1; ++i < its->m_QWNDs.size();) {
                if (its->m_QWNDs[i] == p) {
                    its->m_QWNDs.erase(its->m_QWNDs.begin()+i);
                    if (FocusCtrl == p) {
                        FocusCtrl = nullptr;
                    }
                    if (mouseFocusCtrl == p) {
                        mouseFocusCtrl = nullptr;
                    }
                    if (mouseSeleFocusCtrl == p) {
                        mouseSeleFocusCtrl = nullptr;
                    }
                    if (mouseCapture == p) {
                        mouseCapture = nullptr;
                    }
                    return 2;
                }
            }
        }

        isChange = true;

    }return 1;
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
            for (auto& its : m_qto.m_tabs) {
                for (auto& it : its->m_QWNDs) {
                    it->Redraw();
                    it->TransferMessage(msgID, LParam, RParam, AddParam);
                }
            
            }
        }


    }break;
    case QM_GET_SUB_CTRL: {
        std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
        int count = 0;
        QWND** pwnd = (QWND**)LParam;
        for (int i = -1; auto & its : m_qto.m_tabs) {
            count += its->m_QWNDs.size();
            if (LParam) {
                for (auto& it : its->m_QWNDs) {
                    if (++i == RParam) {
                        *pwnd = it;
                    }
                }
            }
        }


     

        return count;
    }
    case QM_TO_CLIENT_XY: {
        QDWORD* plp = (QDWORD*)LParam;
        QDWORD lp = *plp;
        int x = lp >> 32;
        int y = lp & 0xffffffff;

        y += m_qto.maxHeight+m_rect.Y;
        x += m_rect.X;
        *plp = (QDWORD(x) << 32) | (QDWORD(y) & 0xffffffff);
        return QContainer::TransferMessage(QM_TO_CLIENT_XY, QDWORD(plp), 0, 0);

    }break;
    case QM_POINT_IN_CTRL: {
        std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
        if (!m_qto.m_FocusTab)return 0;
        for (auto & its : m_qto.m_FocusTab->m_QWNDs) {
            if (its->Contains(LParam, RParam- m_qto.maxHeight)) {

                return its->TransferMessage(QM_POINT_IN_CTRL, LParam-m_rect.X, RParam - m_qto.maxHeight -m_rect.Y,0);
            }
        }
    }


    }



    return QContainer::TransferMessage(msgID, LParam, RParam, AddParam);
}

int QTabs::AttachCtrl(QWND* w, int index)
{
    __READ_LOCK(m_qto.m_lock);//标签锁
    if (index < 0 || index >= m_qto.m_tabs.size())return -1;

    //发送消息
    QDWORD lp = (QDWORD(m_rect.Width)<<32)|(QDWORD(m_rect.Height- m_qto.maxHeight)&0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp,0,0);//通知改变控件的大小
   
    ((QTabs*)w)->m_Parent = (QWND*)this;
    ((QTabs*)w)->m_HostHwnd = m_HostHwnd;
    std::unique_lock lk(m_qto.m_tabs[index]->lock);
    m_qto.m_tabs[index]->m_QWNDs.push_back(w);
    isChange = true;
    return 1;
 
}

int QTabs::RemoveCtrl(QWND* w, int index)
{
    __READ_LOCK(m_qto.m_lock);//标签锁
    if (index < 0 || index >= m_qto.m_tabs.size())return -1;
    std::unique_lock lk(m_qto.m_tabs[index]->lock);
    auto it = std::find_if(m_qto.m_tabs[index]->m_QWNDs.begin(), m_qto.m_tabs[index]->m_QWNDs.end(), [&](const QWND* q) {return q == w; });
    if (it == m_qto.m_tabs[index]->m_QWNDs.end())return -2;
    m_qto.m_tabs[index]->m_QWNDs.erase(it);
    QWND* p = w;
    if (FocusCtrl == p) {
        FocusCtrl = nullptr;
    }
    else if (mouseFocusCtrl == p) {
        mouseFocusCtrl = nullptr;
    }
    else if (mouseSeleFocusCtrl == p) {
        mouseSeleFocusCtrl = nullptr;
    }
    else if (mouseCapture == p) {
        mouseCapture = nullptr;
    }
    isChange = true;
    return 1;
}

int QTabs::AddTab(const wchar_t* name, const wchar_t* pBPath)
{
   
    QTabsProperties tp;
    tp.text = name;
    if (pBPath) {
        tp.mapPath = pBPath;
        LoadBitmapFromFile(pBPath, m_pRenderTarget, pWICFactory, &tp.pBitmap);
    }
    __AddTab(tp);

    isChange = true;
    return 1;
}

int QTabs::RemoveTab(int index)
{
    __WRITE_LOCK(m_qto.m_lock);//标签锁
    if (index < 0 || index >= m_qto.m_tabs.size())return -1;
    if (m_qto.m_tabs[index]->pBitmap) {
        SafeRelease(&m_qto.m_tabs[index]->pBitmap);
    }
    if (m_qto.m_tabs[index] == m_qto.m_FocusTab) {
        
        if (index< m_qto.m_tabs.size()-2) {
            m_qto.m_FocusTab = m_qto.m_tabs[index+1];
        }
        else if(m_qto.m_tabs.size()>1){
            m_qto.m_FocusTab = m_qto.m_tabs[0];
        }
    }
    if (m_qto.m_tabs[index]->pBitmap) {
        SafeRelease(&m_qto.m_tabs[index]->pBitmap);
    }


    m_qto.m_tabs.erase(m_qto.m_tabs.begin()+index);
    isChange = true;
    return 1;
}



void QTabs::__AddTab(QTabsProperties& qtp, int index)
{
    if (index<-1 || index > QDWORD(m_qto.m_tabs.size()))return;
    if (-1 == index)index = m_qto.m_tabs.size();
    QTabsProperties* p = new QTabsProperties(qtp);
    __WRITE_LOCK(m_qto.m_lock);//标签锁
    if (m_qto.m_tabs.size() < 1)p->isFocus = true;
    m_qto.m_tabs.insert(m_qto.m_tabs.begin()+ index,p);
    if (p->isFocus) {
        if (m_qto.m_FocusTab)m_qto.m_FocusTab->isFocus = false;
        m_qto.m_FocusTab = m_qto.m_tabs[m_qto.m_tabs.size() - 1];
        m_qto.m_FocusTab->isFocus = true;
    }
}

int QTabs::SetBKBrush(GBrush* brush)
{
    std::unique_lock alk(m_AttributeLock);
    if (m_bkBrush) {
        delete m_bkBrush;
        m_bkBrush = NULL;
    }
    int ret = 2;
    if (brush) {
        m_bkBrush = brush->Clone();
        ret = 1;
    }
    return 0;
}

void QTabs::SetFontName(const wchar_t* name)
{
    std::unique_lock alk(m_AttributeLock);
    IDWriteTextFormat* pTFormat = nullptr;
    
    m_fontName = name;
    pDWriteFactory->CreateTextFormat(
        m_fontName.c_str(),                // Font family
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,                   // Font size
        L"en-us",                // Locale
        &pTFormat
    );
    // Set text alignment to center
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中

    IDWriteTextFormat* buf = pTextFormat;
    pTextFormat = pTFormat;

    SafeRelease(&buf);
    __READ_LOCK(m_qto.m_lock);//标签锁
    for (int i = -1; ++i < m_qto.m_tabs.size();) {
        m_qto.m_tabs[i]->isUpdateMetrics = true;
    }
    isChange = true;
}



void QTabs::SetFontSize(double size)
{
    std::unique_lock alk(m_AttributeLock);
    IDWriteTextFormat* pTFormat = nullptr;

    m_fontSize  = size;
    pDWriteFactory->CreateTextFormat(
        m_fontName.c_str(),                // Font family
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,                   // Font size
        L"en-us",                // Locale
        &pTFormat
    );
    // Set text alignment to center
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中

    IDWriteTextFormat* buf = pTextFormat;
    pTextFormat = pTFormat;

    SafeRelease(&buf);
    __READ_LOCK(m_qto.m_lock);//标签锁
    for (int i = -1; ++i < m_qto.m_tabs.size();) {
        m_qto.m_tabs[i]->isUpdateMetrics = true;
    }
    isChange = true;
}



void QTabs::SetTabIco(int index, const wchar_t* path)
{
    __WRITE_LOCK(m_qto.m_lock);//标签锁
    if (index < 0 || index >= m_qto.m_tabs.size())return;
    if (m_qto.m_tabs[index]->pBitmap) {
        SafeRelease(&m_qto.m_tabs[index]->pBitmap);
    
    }
    LoadBitmapFromFile(path, m_pRenderTarget, pWICFactory, &m_qto.m_tabs[index]->pBitmap);
    isChange = true;
}

void QTabs::SetTabName(int index, const wchar_t* name)
{
    __WRITE_LOCK(m_qto.m_lock);//标签锁
    if (index < 0 || index >= m_qto.m_tabs.size())return;
    m_qto.m_tabs[index]->text = name;
    m_qto.m_tabs[index]->isUpdateMetrics = true;
    isChange = true;
}

void QTabs::AttachCtrlToCurrentTabe(QWND* w)
{
    __READ_LOCK(m_qto.m_lock);//标签锁
    if (!m_qto.m_FocusTab)return;
    ((QTabs*)w)->m_Parent = (QWND*)this;
    ((QTabs*)w)->m_HostHwnd = m_HostHwnd;
    m_qto.m_FocusTab->m_QWNDs.push_back(w);

    isChange = true;
}

int QTabs::GetTabeCount()
{
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    return m_qto.m_tabs.size();
}

int QTabs::GetTabeCtrlCount(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    if (index <0 || index>QDWORD(m_qto.m_tabs.size()) - 1)return -1;

    return m_qto.m_tabs[index]->m_QWNDs.size();
}

const wchar_t* QTabs::GetTabeBMPPath(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    if (index <0 || index>QDWORD(m_qto.m_tabs.size()) - 1)return nullptr;

    return m_qto.m_tabs[index]->mapPath.c_str();
}

const wchar_t* QTabs::GetTabeText(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    if (index <0 || index>QDWORD(m_qto.m_tabs.size()) - 1)return nullptr;

    return m_qto.m_tabs[index]->text.c_str();
}

QWND* QTabs::GetCtrl(int ti, int index)
{
    std::shared_lock<std::shared_mutex> lk(m_qto.m_lock);
    if (ti <0 || ti>QDWORD(m_qto.m_tabs.size()) - 1)return nullptr;
    if (index <0 || index>QDWORD(m_qto.m_tabs[ti]->m_QWNDs.size()) - 1)return nullptr;

    return m_qto.m_tabs[ti]->m_QWNDs[index];
}

int QTabs::InsertTab(const wchar_t* name, int index, const wchar_t* pBPath)
{
    QTabsProperties tp;
    tp.text = name;
    
    if (pBPath) {
        tp.mapPath = pBPath;
        LoadBitmapFromFile(pBPath, m_pRenderTarget, pWICFactory, &tp.pBitmap);
    }
    __AddTab(tp,index);

    isChange = true;
    return 1;
}

void QTabs::SetTabBKColor(unsigned long color)
{
    __NormalTabColor = color;

    isChange = true;
}

void QTabs::SetFontColor(unsigned long color)
{
    m_fontColor = color;
    isChange = true;
}



extern "C"
__declspec(dllexport) int QTBAddTab(QTabs * qt, const wchar_t* name, const wchar_t* pBPath)
{
    return qt->AddTab(name, pBPath);
}

extern "C"
__declspec(dllexport) int QTBAttachCtrl(QTabs * qt, QWND * w, int index)
{
    return qt->AttachCtrl(w, index);
}
extern "C"
__declspec(dllexport) int QTBRemoveCtrl(QTabs * qt, QWND * w, int index)
{
    return qt->RemoveCtrl(w, index);
}

extern "C"
__declspec(dllexport) int QTBRemoveTab(QTabs * qt, int index)
{
    return qt->RemoveTab(index);
}

extern "C"
__declspec(dllexport) int QTBSetBKBrush(QTabs * qt, GBrush * brush)
{
    return qt->SetBKBrush(brush);
}

extern "C"
__declspec(dllexport) void QTBSetFontName(QTabs * qt, const wchar_t* name)
{
    return qt->SetFontName(name);
}
extern "C"
__declspec(dllexport) void QTBSetFontSize(QTabs * qt, double size)
{
    return qt->SetFontSize(size);
}
extern "C"
__declspec(dllexport) void QTBSetFontColor(QTabs * qt, unsigned long color)
{
    return qt->SetFontColor(color);
}
extern "C"
__declspec(dllexport) void QTBSetTabIco(QTabs * qt, int index, const wchar_t* path)
{
    return qt->SetTabIco(index, path);

}
extern "C"
__declspec(dllexport) void QTBSetTabName(QTabs * qt, int index, const wchar_t* name)
{
    return qt->SetTabName(index, name);

}
extern "C"
__declspec(dllexport) void QTBSetTabBKColor(QTabs * qt, unsigned long color)
{
    return qt->SetTabBKColor(color);

}

extern "C" {

    __declspec(dllexport) void QTBAttachCtrlToCurrentTabe(QTabs* qt, QWND* w)
    {
        return qt->AttachCtrlToCurrentTabe(w);
    }
    __declspec(dllexport) int QTBGetTabeCount(QTabs* qt)
    {
        return qt->GetTabeCount();
    }
    __declspec(dllexport) int QTBGetTabeCtrlCount(QTabs* qt, int index)
    {
        return qt->GetTabeCtrlCount(index);
    
    }
    __declspec(dllexport) QWND* QTBGetCtrl(QTabs* qt, int ti, int index)
    {
        return qt->GetCtrl(ti,index);
    
    }
    __declspec(dllexport) int QTBInsertTab(QTabs* qt, const wchar_t* name, int index, const wchar_t* pBPath)
    {
    
        return qt->InsertTab(name, index, pBPath);
    }
    __declspec(dllexport) const wchar_t* QTBGetTabeBMPPath(QTabs* qt, int index)
    {
    
        return qt->GetTabeBMPPath(index);
    }
    __declspec(dllexport) const wchar_t* QTBGetTabeText(QTabs* qt, int index)
    {
    
        return qt->GetTabeText(index);
    }

    __declspec(dllexport) const wchar_t* QTBGetCurrentTabeText(QTabs* qt)
    {

        return qt->GetCurrentTabeText();
    }


    __declspec(dllexport) const wchar_t* QTBGetCurrentTabeIcoPath(QTabs* qt)
    {

        return qt->GetCurrentTabeIcoPath();
    }


    __declspec(dllexport) int QTBSetCurrentTabeText(QTabs* qt,const wchar_t* txt)
    {

        return qt->SetCurrentTabeText(txt);
    }

    __declspec(dllexport) int QTBSetCurrentTabeIcoPath(QTabs* qt, const wchar_t* path)
    {

        return qt->SetCurrentTabeIcoPath(path);


    }
}
































