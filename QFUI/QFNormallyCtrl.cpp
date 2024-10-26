

#include "QFNormallyCtrl.h"
#include <cmath>
#include <numbers>
bool isPointInRoundedRectangle(double px, double py, double cx, double cy, double width, double height, double radius) {
    // �м���β��ֵ��ж�
    if (px >= cx - width / 2 + radius && px <= cx + width / 2 - radius &&
        py >= cy - height / 2 + radius && py <= cy + height / 2 - radius) {
        return true;
    }

    // �ĸ�Բ��������ж�
    // ����Բ��
    double leftTopCornerX = cx - width / 2 + radius;
    double leftTopCornerY = cy - height / 2 + radius;
    if (std::hypot(px - leftTopCornerX, py - leftTopCornerY) <= radius) {
        return true;
    }

    // ����Բ��
    double rightTopCornerX = cx + width / 2 - radius;
    double rightTopCornerY = cy - height / 2 + radius;
    if (std::hypot(px - rightTopCornerX, py - rightTopCornerY) <= radius) {
        return true;
    }

    // ����Բ��
    double leftBottomCornerX = cx - width / 2 + radius;
    double leftBottomCornerY = cy + height / 2 - radius;
    if (std::hypot(px - leftBottomCornerX, py - leftBottomCornerY) <= radius) {
        return true;
    }

    // ����Բ��
    double rightBottomCornerX = cx + width / 2 - radius;
    double rightBottomCornerY = cy + height / 2 - radius;
    if (std::hypot(px - rightBottomCornerX, py - rightBottomCornerY) <= radius) {
        return true;
    }

    return false;
}
bool isPointInRoundedRectangle(
    ID2D1Factory* pFactory,
    D2D1_POINT_2F point,
    D2D1_ROUNDED_RECT roundedRect
) {
    // Create a rounded rectangle geometry
    ID2D1RoundedRectangleGeometry* pRoundedRectangleGeometry = nullptr;
    HRESULT hr = pFactory->CreateRoundedRectangleGeometry(roundedRect, &pRoundedRectangleGeometry);

    if (FAILED(hr)) {
        
        return false;
    }

    // Check if the point is inside the rounded rectangle geometry
    BOOL containsPoint = FALSE;
    hr = pRoundedRectangleGeometry->FillContainsPoint(point, nullptr, &containsPoint);

    pRoundedRectangleGeometry->Release();

    if (FAILED(hr)) {
        
        return false;
    }

    return containsPoint == TRUE;
}

//QDrawer�ĺ�������
#ifdef __QDRAWER__
inline void QDrawer::QCreateDC(HDC winddc)
{
    m_pageDC = CreateCompatibleDC(winddc);
    m_pageHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_pageDC, m_pageHB);
}

inline void QDrawer::ReleaseDC()
{
    DeleteObject(m_pageHB);
    m_pageHB = NULL;
    DeleteDC(m_pageDC);
    m_pageDC = NULL;
}

QDrawer::QDrawer(HDC winddc, const GRect& rf):QWND(winddc,rf)
{
    QCreateDC(winddc);
    //m_bkBrush = new GSolidBrush(GColor(0xaf, 0xaa, 0xaa, 0xaa));
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
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
    // Set paragraph alignment to center
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���

    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    pTextFormat->SetTrimming(&trimming, nullptr);//��ֹ������ֿ�

    RECT rc(0, 0, rf.Width, rf.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���

    //����һ��ͼ��
    //m_pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory));


    __WNDAttribute.Attribute.HostCtrlMouse = 1;//������������Ϣ

    //����
    //Test();
}



QDrawer::~QDrawer()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
    //SafeRelease(&pLayer);
    ReleaseDC();
}




int QDrawer::LButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    float y1 = m_startPos;
    float rad = min(m_rect.Width, m_rect.Height);
    D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
    IDWriteTextLayout* pTextLayout = NULL;
    float ratio = 0.05;
    D2D1_RECT_F proRect = D2D1::RectF(rad * ratio, 0, m_rect.Width - rad * ratio, m_rect.Height);
    std::shared_lock<std::shared_mutex> lk(m_lock);
    for (auto& it : m_data) {
        proRect.top = y1;
        if (it.isChange) {
            it.isChange = false;
            pDWriteFactory->CreateTextLayout(
                it.name.c_str(),
                it.name.length(),
                pTextFormat,
                renderTargetSize.width,
                renderTargetSize.height,
                &pTextLayout);

            DWRITE_TEXT_METRICS textMetrics;
            pTextLayout->GetMetrics(&textMetrics);
            it.width = textMetrics.width * 1.1;
            it.height = textMetrics.height * 1.1;

            pTextLayout->Release();
        }
        proRect.bottom = y1 + it.height;
        float r = min(it.width, it.height);
        float at = 0.3;//����
        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
            proRect, // Rectangle coordinates
            r * at,                          // Radius for the rounded corners
            r * at                           // Radius for the rounded corners
        );



        if (isPointInRoundedRectangle(m_pD2DFactory,D2D1::Point2F(px,py), rRect)) {
            it.isWithdrawn = !it.isWithdrawn;
            isChange = true;
            break;
        }
       

        y1 += it.height;
        //����
        {
            if (it.isWithdrawn) {
                for (auto& sit : it.subkey) {
                    proRect.top = y1;
                    if (sit.isChange) {
                        sit.isChange = false;
                        pDWriteFactory->CreateTextLayout(
                            sit.name.c_str(),
                            sit.name.length(),
                            pTextFormat,
                            renderTargetSize.width,
                            renderTargetSize.height,
                            &pTextLayout);

                        DWRITE_TEXT_METRICS textMetrics;
                        pTextLayout->GetMetrics(&textMetrics);
                        sit.width = textMetrics.width * 1.1;
                        sit.height = textMetrics.height * 1.1;

                        pTextLayout->Release();
                    }
                    proRect.bottom = y1 + sit.height;


                    y1 += sit.height;
                }

            }



        }

    }

    
    return 0;
}

int QDrawer::LButtonUp(int x, int y, WPARAM wParam)
{
    return 0;
}

int QDrawer::MouseMove(int x, int y, WPARAM wParam)
{
    __mouseX = x - m_rect.X;
    __mouseY = y - m_rect.Y;
    return 0;
}

int QDrawer::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
        flag = 1;
        isChange = false;
        //1.sdc�ж�Ӧ��λ�ÿ�������
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //2.��������
        {

       /*     GRectF rectf(0, 0, m_rect.Width, m_rect.Height);
            Gdiplus::Graphics graphics(m_dc);
            if (m_bkBrush) {
                graphics.FillRectangle(m_bkBrush, rectf);
            }*/


        }

        //3.���ƿؼ�
        {
            // Begin drawing
            m_pRenderTarget->BeginDraw();


            // Draw something
            ID2D1SolidColorBrush* pBrush = NULL;
            ID2D1SolidColorBrush* pfBrush = NULL;//������ɫ��ˢ
            ID2D1SolidColorBrush* pbBrush = NULL;//�߿���ɫ��ˢ
            ID2D1SolidColorBrush* ppbBrush = NULL;//��Ŀ������ɫ��ˢ
            ID2D1SolidColorBrush* ppfBrush = NULL;//��Ŀ������ɫ��ˢ

            ID2D1SolidColorBrush* blackBrush = NULL;//��Ŀ������ɫ��ˢ
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0.2), &blackBrush);

            auto test = D2D1::ColorF::Black;
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffff,0.2), &pBrush);
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x808080,  0.2), &pfBrush);
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x808080, 0.2), &ppbBrush);
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x808080, 0.2), &ppfBrush);

            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x00a0a0, 0.2), &pbBrush);

            D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, m_rect.Width, m_rect.Height);
            float rad = min(m_rect.Width, m_rect.Height);
            D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

            float ratio = 0.05;

            D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                layoutRect, // Rectangle coordinates
                rad * ratio,                          // Radius for the rounded corners
                rad * ratio                           // Radius for the rounded corners
            );
          
            m_pRenderTarget->FillRoundedRectangle(roundedRect, pBrush);
            D2D1_RECT_F proRect = D2D1::RectF(rad * ratio, 0, m_rect.Width - rad * ratio, m_rect.Height);

          
            float y = m_startPos;
            IDWriteTextLayout* pTextLayout = NULL;

            ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES gradientProperties =
            {
                D2D1::Point2F(150, 100), // ���ĵ�����
                D2D1::Point2F(0, 0), // ����ԭ��ƫ����
                200, // X����뾶
                200  // Y����뾶
            };
            // ��������ֹͣ������
            D2D1_GRADIENT_STOP gradientStops[2];
            gradientStops[0].position = 0.0f;
            gradientStops[0].color = D2D1::ColorF(0x000055,0.2);

            gradientStops[1].position = 1.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Blue);

            // ��������ֹͣ�㼯��
            ID2D1GradientStopCollection* pGradientStops = nullptr;
            HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                2, // ����ֹͣ�������
                &pGradientStops
            );
            m_pRenderTarget->CreateRadialGradientBrush(&gradientProperties,NULL, pGradientStops, &pRadialGradientBrush);

            bool isfocus = false;
            std::shared_lock<std::shared_mutex> lk(m_lock);
            for (auto& it : m_data) {
                proRect.top = y;
                if (it.isChange) {
                    it.isChange = false;
                    pDWriteFactory->CreateTextLayout(
                        it.name.c_str(),
                        it.name.length(),
                        pTextFormat,
                        renderTargetSize.width,
                        renderTargetSize.height,
                        &pTextLayout);

                    DWRITE_TEXT_METRICS textMetrics;
                    pTextLayout->GetMetrics(&textMetrics);
                    it.width = textMetrics.width*1.1;
                    it.height = textMetrics.height*1.1;

                    pTextLayout->Release();
                }
                proRect.bottom = y + it.height;
                float r = min(it.width, it.height);
                float at = 0.3;//����
                D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
                    proRect, // Rectangle coordinates
                    r * at,                          // Radius for the rounded corners
                    r * at                           // Radius for the rounded corners
                );
                ppbBrush->SetColor(D2D1::ColorF(it.bcolor & 0xffffff, ((unsigned int)it.bcolor >> 24) / 255.0));
                m_pRenderTarget->FillRoundedRectangle(rRect, ppbBrush);
               
                m_pRenderTarget->DrawRoundedRectangle(rRect, pbBrush);
                ppfBrush->SetColor(D2D1::ColorF(it.color & 0xffffff, ((unsigned int)it.color >> 24) /255.0));

                m_pRenderTarget->DrawText(
                    it.name.c_str(),
                    it.name.length(),
                    pTextFormat,             // The text format
                    proRect,              // The layout rectangle
                    ppfBrush                   // The brush
                );

                if (!isfocus && -1 != __mouseX && -1 != __mouseY && isPointInRoundedRectangle(m_pD2DFactory, D2D1::Point2F(__mouseX, __mouseY), rRect)) {
                    //��һ�㰵����
                    m_pRenderTarget->FillRoundedRectangle(rRect, blackBrush);
                    isfocus = true;
                }

                //����������
                ID2D1PathGeometry* pPathGeometry = nullptr;
                HRESULT hr = m_pD2DFactory->CreatePathGeometry(&pPathGeometry);
                if (!FAILED(hr)) {


                    // Create geometry sink
                    ID2D1GeometrySink* pSink = nullptr;
                    hr = pPathGeometry->Open(&pSink);
                    D2D1_RECT_F trangRect = D2D1::RectF(proRect.left + r * at, proRect.top + r * at, proRect.left + r * at + r, proRect.bottom - r * at);
                    float radius = min(proRect.right-proRect.left- r * at, proRect.bottom- proRect.top -  r * at)/2.0;
                    float cx = proRect.left + r * at / 2 + radius,cy= proRect.top + r * at/2 + radius;
                    if ((proRect.right - proRect.left) / 2.0 < radius) 
                        cx = proRect.right + proRect.left / 2.0 + r * at;
   
                    if ((proRect.bottom - proRect.top) / 2.0 < radius) 
                        cy = proRect.bottom + proRect.top / 2.0 + r * at;
    


                    if (!FAILED(hr)) {

                       
                        if (it.isWithdrawn) {
                            pSink->BeginFigure(D2D1::Point2F(cx-radius* std::sin(60*std::numbers::pi / 180.0), cy- radius * std::cos(60 * std::numbers::pi / 180.0)), D2D1_FIGURE_BEGIN_FILLED);
                            pSink->AddLine(D2D1::Point2F(cx + radius * std::sin(60 * std::numbers::pi / 180.0), cy - radius * std::cos(60 * std::numbers::pi / 180.0)));
                            pSink->AddLine(D2D1::Point2F(cx, cy+radius));
                            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                        }
                        else {
                            float dx = cx + radius,dy=cy;

                            pSink->BeginFigure(D2D1::Point2F(cx - radius * std::cos(60 * std::numbers::pi / 180.0), cy - radius * std::sin(60 * std::numbers::pi / 180.0)), D2D1_FIGURE_BEGIN_FILLED);
                            pSink->AddLine(D2D1::Point2F(cx - radius * std::cos(60 * std::numbers::pi / 180.0), cy + radius * std::sin(60 * std::numbers::pi / 180.0)));
                            pSink->AddLine(D2D1::Point2F(cx+radius, cy));
                            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                        }
                    }
                    hr = pSink->Close();
                    pSink->Release();
                    pRadialGradientBrush->SetCenter(D2D1::Point2F(cx, cy));
                    pRadialGradientBrush->SetRadiusX(radius);
                    pRadialGradientBrush->SetRadiusY(radius);
                    m_pRenderTarget->FillGeometry(pPathGeometry, pRadialGradientBrush);

    /*                D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
                    m_pRenderTarget->DrawEllipse(ellipse, ppfBrush);*/
                    pPathGeometry->Release();
                }


                y += it.height;
                    
                //��������
                {
                    if (it.isWithdrawn) {
                        for (auto& sit : it.subkey) {
                            proRect.top = y;
                            if (sit.isChange) {
                                sit.isChange = false;
                                pDWriteFactory->CreateTextLayout(
                                    sit.name.c_str(),
                                    sit.name.length(),
                                    pTextFormat,
                                    renderTargetSize.width,
                                    renderTargetSize.height,
                                    &pTextLayout);

                                DWRITE_TEXT_METRICS textMetrics;
                                pTextLayout->GetMetrics(&textMetrics);
                                sit.width = textMetrics.width * 1.1;
                                sit.height = textMetrics.height * 1.1;

                                pTextLayout->Release();
                            }
                            proRect.bottom = y + sit.height;
                            D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
                                proRect, // Rectangle coordinates
                                r * at,                          // Radius for the rounded corners
                                r * at                           // Radius for the rounded corners
                            );
                            ppbBrush->SetColor(D2D1::ColorF(sit.bcolor & 0xffffff, ((unsigned int)sit.bcolor >> 24) / 255.0));
                            m_pRenderTarget->FillRoundedRectangle(rRect, ppbBrush);
                            m_pRenderTarget->DrawRoundedRectangle(rRect, pbBrush);
                            ppfBrush->SetColor(D2D1::ColorF(sit.color & 0xffffff, ((unsigned int)sit.color >> 24) / 255.0));

                            m_pRenderTarget->DrawText(
                                sit.name.c_str(),
                                sit.name.length(),
                                pTextFormat,             // The text format
                                proRect,              // The layout rectangle
                                ppfBrush                   // The brush
                            );
                            if (!isfocus && -1 != __mouseX && -1 != __mouseY && isPointInRoundedRectangle(m_pD2DFactory, D2D1::Point2F(__mouseX, __mouseY), rRect)) {
                                //��һ�㰵����
                                m_pRenderTarget->FillRoundedRectangle(rRect, blackBrush);
                                isfocus = true;
                            }



                            y += sit.height;
                        }
                    
                    }
                
                
                
                }
            
            }



            

            pBrush->Release();
            pfBrush->Release();
            pbBrush->Release();
            ppfBrush->Release();
            ppbBrush->Release();
            pGradientStops->Release();
            pRadialGradientBrush->Release();
            blackBrush->Release();
            // End drawing
            m_pRenderTarget->EndDraw();


        }

        //BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_pageDC, 0, 0, SRCCOPY);
    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}

int QDrawer::MouseWheel(int x, int y, WPARAM wParam)
{
    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    m_startPos += zDelta;
    float y1 = 0;
    float rad = min(m_rect.Width, m_rect.Height);
    D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
    IDWriteTextLayout* pTextLayout = NULL;
    float ratio = 0.05;
    D2D1_RECT_F proRect = D2D1::RectF(rad * ratio, 0, m_rect.Width - rad * ratio, m_rect.Height);
    for (auto& it : m_data) {
        proRect.top = y1;
        if (it.isChange) {
            it.isChange = false;
            pDWriteFactory->CreateTextLayout(
                it.name.c_str(),
                it.name.length(),
                pTextFormat,
                renderTargetSize.width,
                renderTargetSize.height,
                &pTextLayout);

            DWRITE_TEXT_METRICS textMetrics;
            pTextLayout->GetMetrics(&textMetrics);
            it.width = textMetrics.width * 1.1;
            it.height = textMetrics.height * 1.1;

            pTextLayout->Release();
        }
        proRect.bottom = y1 + it.height;
        float r = min(it.width, it.height);
        float at = 0.3;//����
        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
            proRect, // Rectangle coordinates
            r * at,                          // Radius for the rounded corners
            r * at                           // Radius for the rounded corners
        );



        y1 += it.height;
        //����
        {
            if (it.isWithdrawn) {
                for (auto& sit : it.subkey) {
                    proRect.top = y1;
                    if (sit.isChange) {
                        sit.isChange = false;
                        pDWriteFactory->CreateTextLayout(
                            sit.name.c_str(),
                            sit.name.length(),
                            pTextFormat,
                            renderTargetSize.width,
                            renderTargetSize.height,
                            &pTextLayout);

                        DWRITE_TEXT_METRICS textMetrics;
                        pTextLayout->GetMetrics(&textMetrics);
                        sit.width = textMetrics.width * 1.1;
                        sit.height = textMetrics.height * 1.1;

                        pTextLayout->Release();
                    }
                    proRect.bottom = y1 + sit.height;


                    y1 += sit.height;
                }

            }



        }

    }



    float maxR = m_rect.Height - y1;

    if (m_startPos > 0) {
        m_startPos = 0;
    }
    else if (m_startPos < maxR) {
        if (maxR <= 0) {
            //���һ���ĵױ߲���С�ڿؼ��ױ�
            m_startPos = maxR;
        }
        else {
            m_startPos = 0;
        }


    }
    isChange = true;
    return 0;
}

int QDrawer::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    float y1 = m_startPos;
    float rad = min(m_rect.Width, m_rect.Height);
    D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
    IDWriteTextLayout* pTextLayout = NULL;
    float ratio = 0.05;
    D2D1_RECT_F proRect = D2D1::RectF(rad * ratio, 0, m_rect.Width - rad * ratio, m_rect.Height);
    for (int i = -1; auto & it : m_data) {
        ++i;
        proRect.top = y1;
        if (it.isChange) {
            it.isChange = false;
            pDWriteFactory->CreateTextLayout(
                it.name.c_str(),
                it.name.length(),
                pTextFormat,
                renderTargetSize.width,
                renderTargetSize.height,
                &pTextLayout);

            DWRITE_TEXT_METRICS textMetrics;
            pTextLayout->GetMetrics(&textMetrics);
            it.width = textMetrics.width * 1.1;
            it.height = textMetrics.height * 1.1;

            pTextLayout->Release();
        }
        proRect.bottom = y1 + it.height;
        float r = min(it.width, it.height);
        float at = 0.3;//����
        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
            proRect, // Rectangle coordinates
            r * at,                          // Radius for the rounded corners
            r * at                           // Radius for the rounded corners
        );



        if (isPointInRoundedRectangle(m_pD2DFactory, D2D1::Point2F(px, py), rRect)) {
            it.isWithdrawn = !it.isWithdrawn;
            isChange = true;
            break;
        }

        y1 += it.height;
        //����
        {
            if (it.isWithdrawn) {
                bool isCheck = false;
                for (int j = -1; auto & sit : it.subkey) {
                    ++j;
                    proRect.top = y1;
                    if (sit.isChange) {
                        sit.isChange = false;
                        pDWriteFactory->CreateTextLayout(
                            sit.name.c_str(),
                            sit.name.length(),
                            pTextFormat,
                            renderTargetSize.width,
                            renderTargetSize.height,
                            &pTextLayout);

                        DWRITE_TEXT_METRICS textMetrics;
                        pTextLayout->GetMetrics(&textMetrics);
                        sit.width = textMetrics.width * 1.1;
                        sit.height = textMetrics.height * 1.1;

                        pTextLayout->Release();
                    }
                    proRect.bottom = y1 + sit.height;
                    D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
                        proRect, // Rectangle coordinates
                        r * at,                          // Radius for the rounded corners
                        r * at                           // Radius for the rounded corners
                    );
                    if (isPointInRoundedRectangle(m_pD2DFactory, D2D1::Point2F(px, py), rRect)) {
                        if (__selectFUN) {
                            __selectFUN(i,j);
                        }
                        //printf("%d,%d\n",i,j);
                        isCheck = true;
                        break;
                    }

                    y1 += sit.height;
                }
                if (isCheck)break;

            }



        }

    }
    return 0;
}

int QDrawer::MouseMoveOut()
{
    __mouseX = -1, __mouseY = -1;
    return 0;
}

void QDrawer::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    QWND::Reconstruct(sdc, rect);
    ReconstructDC(m_pageDC, m_dc, m_pageHB, rect);
    RECT rt(0,0, rect.Width, rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rt);//��DC���

}

void QDrawer::Test()
{
    for (int i = -1; ++i < 10;) {
        Category c;
        c.name = L"��Ŀ" + std::to_wstring(i);
        m_data.push_back(c);
    }
    for (int i = -1; ++i < 10;) {
        m_data[0].subkey.push_back(QDSubkey());
    }
    for (int i = -1; ++i < 10;) {
        m_data[1].subkey.push_back(QDSubkey());
    }
}

void QDrawer::SetFontName(const wchar_t* name)
{
    if (name == nullptr || L'\0' == *name)return;

    m_fontName = name;
    IDWriteTextFormat* pTFormat = nullptr;
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

    if (pTFormat) {

        pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
        pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
        // Set paragraph alignment to center
        pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���
        IDWriteTextFormat* buff = pTextFormat;
        pTextFormat = pTFormat;
        SafeRelease(&buff);
        std::unique_lock<std::shared_mutex> lk(m_lock);
        for (auto& its : m_data) {
            its.isChange = true;
            for (auto& it : its.subkey) {
                it.isChange = true;
            }
        }


        isChange = true;
    }
}

void QDrawer::SetFontSize(float size)
{
    if (size <= 1)size =1;
    m_fontSize = size;

    IDWriteTextFormat* pTFormat = nullptr;
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
    if (pTFormat) {
        
        pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
        pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
        // Set paragraph alignment to center
        pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���
        IDWriteTextFormat* buff = pTextFormat;
        pTextFormat = pTFormat;
        SafeRelease(&buff);
        std::unique_lock<std::shared_mutex> lk(m_lock);
        for (auto& its : m_data) {
            its.isChange = true;
            for (auto& it : its.subkey) {
                it.isChange = true;
            }
        }


        isChange = true;
    }
}

int QDrawer::InsertDrawer(int index)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);

    if (index<-1 || index > (int)m_data.size())return -1;
    if (-1 == index) index = m_data.size();
    m_data.insert(m_data.begin()+ index, Category());
    isChange = true;
    return index;
}

int QDrawer::InsertSubObject(int iDrawer, int iObj)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (-1 == iDrawer)iDrawer = QDWORD(m_data.size()) - 1;
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if(iObj<-1 || iObj >(int)m_data[iDrawer].subkey.size())return -2;
   
    auto& sk = m_data[iDrawer].subkey;
    if (-1 == iObj) iObj = sk.size();
    sk.insert(sk.begin()+ iObj, QDSubkey());

    isChange = true;
    return iObj;
}

int QDrawer::RemoveSubObject(int iDrawer, int iObj)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (iObj<-1 || iObj >(int)m_data[iDrawer].subkey.size()-1)return -2;
    auto& sk = m_data[iDrawer].subkey;
    if (-1 == iObj) iObj = sk.size();
    sk.erase(sk.begin() + iObj);
    return 1;
}

int QDrawer::RemoveDrawer(int index)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    m_data.erase(m_data.begin() + index);
    return 1;
}

int QDrawer::GetDrawerCount()
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    

    return m_data.size();
}

int QDrawer::GetSubObjectCount(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    return m_data[index].subkey.size();
}

int QDrawer::SetDrawerName(int index, const wchar_t* name)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == index)index = QDWORD(m_data.size()) - 1;
    if (index<0 || index >QDWORD(m_data.size()) - 1)return -1;
    m_data[index].name = name;

    isChange = true;
    return 1;
}

int QDrawer::SetDrawerFontColor(int index, int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == index)index = QDWORD(m_data.size()) - 1;
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    m_data[index].color = color;
    isChange = true;
    return 1;
}

int QDrawer::SetDrawerBKColor(int index, int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == index)index = QDWORD(m_data.size()) - 1;
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    m_data[index].bcolor = color;
    isChange = true;
    return 1;
}

int QDrawer::OpenDrawer(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    m_data[index].isWithdrawn = true;
    isChange = true;
    return 1;
}

int QDrawer::CloseDrawer(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (index<0 || index >(int)m_data.size() - 1)return -1;
    m_data[index].isWithdrawn = true;
    isChange = false;
    return 1;
}

int QDrawer::SetSubObjectName(int iDrawer, int iObj, const wchar_t* name)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == iDrawer)iDrawer = QDWORD(m_data.size()) - 1;
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (-1 == iObj)iObj = QDWORD(m_data[iDrawer].subkey.size()) - 1;
    if (iObj<0 || iObj >(int)m_data[iDrawer].subkey.size() - 1)return -2;
    m_data[iDrawer].subkey[iObj].name = name;

    isChange = true;
    return 1;
}

int QDrawer::SetSubObjectFontColor(int iDrawer, int iObj, int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == iDrawer)iDrawer = QDWORD(m_data.size()) - 1;
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (-1 == iObj)iObj = QDWORD(m_data[iDrawer].subkey.size()) - 1;
    if (iObj<0 || iObj >(int)m_data[iDrawer].subkey.size() - 1)return -2;
    m_data[iDrawer].subkey[iObj].color = color;
    isChange = true;
    return 1;
}

int QDrawer::SetSubObjectBKColor(int iDrawer, int iObj, int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (-1 == iDrawer)iDrawer = QDWORD(m_data.size()) - 1;
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (-1 == iObj)iObj = QDWORD(m_data[iDrawer].subkey.size()) - 1;
    if (iObj<0 || iObj >(int)m_data[iDrawer].subkey.size() - 1)return -2;
    m_data[iDrawer].subkey[iObj].bcolor = color;

    isChange = true;
    return 1;
}

int QDrawer::SetSelectEvent(QDRAWERSELECT fun)
{
    __selectFUN = fun;
    return 1;
}

int QDrawer::GetSubObjectName(int iDrawer, int iObj, wchar_t* pName, int bl)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (iObj<0 || iObj >(int) m_data[iDrawer].subkey.size() - 1)return -2;
    if (NULL == pName)return m_data[iDrawer].subkey[iObj].name.length();
    int len = min((unsigned int)bl, m_data[iDrawer].subkey[iObj].name.length());
    wcsncpy_s(pName, bl, m_data[iDrawer].subkey[iObj].name.c_str(), m_data[iDrawer].subkey[iObj].name.length());
 

    return len;
}

int QDrawer::GetDrawerName(int iDrawer, wchar_t* pName, int bl)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (iDrawer<0 || iDrawer >(int)m_data.size() - 1)return -1;
    if (NULL == pName)return (int)m_data[iDrawer].name.length();
    int len = min((unsigned int)bl, m_data[iDrawer].name.length());
    wcsncpy_s(pName, bl, m_data[iDrawer].name.c_str(), m_data[iDrawer].name.length());
    return len;
}

QDWORD QDrawer::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }



    }break;
    }

    return QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}

#endif

extern "C" {
    //�û��ӿ�
    __declspec(dllexport) void QDWSetFontName(QDrawer* p,const wchar_t* name)
	{
        return p->SetFontName(name);
	}
    __declspec(dllexport) void QDWSetFontSize(QDrawer* p, float size)
	{
        return p->SetFontSize(size);
	}
    __declspec(dllexport) int QDWInsertDrawer(QDrawer* p, int index)
	{
        return p->InsertDrawer(index);
	}
    __declspec(dllexport) int QDWInsertSubObject(QDrawer* p, int iDrawer, int iObj)
	{
        return p->InsertSubObject(iDrawer, iObj);
	}
    __declspec(dllexport) int QDWRemoveSubObject(QDrawer* p, int iDrawer, int iObj)
	{
        return p->RemoveSubObject(iDrawer, iObj);
	}
    __declspec(dllexport) int QDWRemoveDrawer(QDrawer* p, int index)
	{
        return p->RemoveDrawer(index);
	}
    __declspec(dllexport) int QDWGetDrawerCount(QDrawer* p)
	{
        return p->GetDrawerCount();
	}
    __declspec(dllexport) int QDWGetSubObjectCount(QDrawer* p, int index)
	{
        return p->GetSubObjectCount(index);
	}
    __declspec(dllexport) int QDWSetDrawerName(QDrawer* p, int index, const wchar_t* name)
	{
        return p->SetDrawerName(index, name);
	}
    __declspec(dllexport) int QDWSetDrawerFontColor(QDrawer* p, int index, int color)
	{
        return p->SetDrawerFontColor(index, color);
	}
    __declspec(dllexport) int QDWSetDrawerBKColor(QDrawer* p, int index, int color)
	{
        return p->SetDrawerBKColor(index, color);
	}
    __declspec(dllexport) int QDWOpenDrawer(QDrawer* p, int index)
	{
        return p->OpenDrawer(index);
	}
    __declspec(dllexport) int QDWCloseDrawer(QDrawer* p, int index)
	{
        return p->CloseDrawer(index);
	}
    __declspec(dllexport) int QDWSetSubObjectName(QDrawer* p, int iDrawer, int iObj, const wchar_t* name)
	{
        return p->SetSubObjectName(iDrawer, iObj, name);
	}
    __declspec(dllexport) int QDWSetSubObjectFontColor(QDrawer* p, int iDrawer, int iObj, int color)
	{
        return p->SetSubObjectFontColor(iDrawer, iObj, color);
	}
    __declspec(dllexport) int QDWSetSubObjectBKColor(QDrawer* p, int iDrawer, int iObj, int color)
	{
        return p->SetSubObjectBKColor(iDrawer, iObj, color);
	}
    __declspec(dllexport) int QDWSetSelectEvent(QDrawer* p, QDRAWERSELECT fun)
	{
        return p->SetSelectEvent(fun);
	}

    __declspec(dllexport) int QDWGetSubObjectName(QDrawer* p, int iDrawer, int iObj, wchar_t* pName, int bl)
	{
        return p->GetSubObjectName(iDrawer, iObj, pName, bl);
	}
    __declspec(dllexport) int QDWGetDrawerName(QDrawer* p, int iDrawer, wchar_t* pName, int bl)
	{
        return p->GetDrawerName(iDrawer, pName, bl);
	}










}
#ifdef __QHOSTCTRL__
HCURSOR QHostCtrl::m_hCursorWE = NULL;
HCURSOR QHostCtrl::m_hCursorNS = NULL;
HCURSOR QHostCtrl::m_hCursorARROW = NULL;
HCURSOR QHostCtrl::m_hCursorSIZEALL = NULL;


inline void QHostCtrl::QCreateDC(HDC winddc)
{
    m_pageDC = CreateCompatibleDC(winddc);
    m_pageHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_pageDC, m_pageHB);
}

inline void QHostCtrl::ReleaseDC()
{
    DeleteObject(m_pageHB);
    m_pageHB = NULL;
    DeleteDC(m_pageDC);
    m_pageDC = NULL;
}

QHostCtrl::QHostCtrl(HDC winddc, const GRect& rf):QWND(winddc,rf)
{
    __WNDAttribute.Attribute.isQHostCtrl = 1; //���ؼ���һ������
    if(!m_hCursorWE)
    m_hCursorWE = LoadCursor(NULL, IDC_SIZEWE);
    if (!m_hCursorNS)
    m_hCursorNS = LoadCursor(NULL, IDC_SIZENS);
    if (!m_hCursorARROW)
    m_hCursorARROW = LoadCursor(NULL, IDC_ARROW);
    if (!m_hCursorSIZEALL)
    m_hCursorSIZEALL = LoadCursor(NULL, IDC_SIZEALL);
    QCreateDC(winddc);
    //m_bkBrush = new GSolidBrush(GColor(0xaf, 0xaa, 0xaa, 0xaa));
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
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
    // Set paragraph alignment to center
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���

    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    pTextFormat->SetTrimming(&trimming, nullptr);//��ֹ������ֿ�

    RECT rc(0, 0, rf.Width, rf.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���

    //����һ��ͼ��
    //m_pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory));



#ifdef _DEBUG
    //����
    Test();
#endif
}

QHostCtrl::~QHostCtrl()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
   
    ReleaseDC();
}

int QHostCtrl::LButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    if (m_rect.Contains(x, y)) {
        FocusCtrl = nullptr;

        bool isto = false;//�Ƿ��пؼ��ӹ����
        if (m_boardCtrl) {
            QHostCtrl* p = nullptr;
            QDWORD lp = (QDWORD(x - m_rect.X) << 32) | ((y - m_rect.Y) & 0xffffffff);
            QWNDAttribute qwa = { 0 };
            qwa.AData = m_boardCtrl->GetAttribute();
            if ((qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout || qwa.Attribute.HostCtrlMouse) &&m_boardCtrl->Contains(x - m_rect.X, y - m_rect.Y)) {
                ret = m_boardCtrl->LButtonDown(x - m_rect.X, y - m_rect.Y, wParam);
                m_boardCtrl->Redraw();
            }
            if (1 == m_boardCtrl->TransferMessage(QM_POINT_IN_QHOSTCTRL, lp, (QDWORD)&p, 0)) {
                isto = true;
                isSelect = false;
            }
            m_boardCtrl->Redraw();
        }
        if (!FocusCtrl) {
            if (m_Parent) {
                m_Parent->TransferMessage(QM_QHOST_LDOWN, (QDWORD)this, (QDWORD)m_boardCtrl, 0);
            }
        
        
        }


        if (!isto) {
            isSelect = true;
            if (isSelect) {
                isDown = true;
                if (abs(m_rect.GetRight() - x) < 5) {
                    m_hCursor = m_hCursorWE;
                    __DownData.flag = 1;
                    __DownData.dta = m_rect.GetRight() - x;
                }
                else if (abs(m_rect.X - x) < 5) {
                    m_hCursor = m_hCursorWE;
                    __DownData.flag = 3;
                    __DownData.dta = m_rect.X - x;
                }
                else if (abs(m_rect.GetBottom() - y) < 5) {
                    m_hCursor = m_hCursorNS;
                    __DownData.flag = 2;
                    __DownData.dta = m_rect.GetBottom() - y;
                }
                else if (abs(m_rect.Y - y) < 5) {
                    m_hCursor = m_hCursorNS;
                    __DownData.flag = 0;
                    __DownData.dta = m_rect.Y - y;
                }
                else {
                    m_hCursor = m_hCursorSIZEALL;

                    __DownData.flag = 4;
                    __DownData.dta = m_rect.X - x;
                    __DownData.dta2 = m_rect.Y - y;
                }
            }
            else {
                isSelect = true;
            }
        }
        //m_Parent->TransferMessage(QM_QHOST_LDOWN, (QDWORD)this, (QDWORD)m_boardCtrl, 0);
    }
    isChange = true;

    return ret;
}

int QHostCtrl::LButtonUp(int x, int y, WPARAM wParam)
{
    bool isto = false;//�Ƿ��пؼ��ӹ����
    if (m_boardCtrl) {
        QHostCtrl* p = nullptr;
        QDWORD lp = (QDWORD(x-m_rect.X)<<32)|((y - m_rect.Y)&0xffffffff);
        QWNDAttribute qwa = { 0 };
        qwa.AData = m_boardCtrl->GetAttribute();
        if ((qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout || qwa.Attribute.HostCtrlMouse) && m_boardCtrl->Contains(x - m_rect.X, y - m_rect.Y)) {
            m_boardCtrl->LButtonUp(x - m_rect.X, y - m_rect.Y, wParam);
            m_boardCtrl->Redraw();
        }
        if (1 == m_boardCtrl->TransferMessage(QM_POINT_IN_QHOSTCTRL, lp,(QDWORD)&p,0)) {
            isto = true;
            
        }
    }

    /*if (!isto)*/ {
        isDown = false;
        __DownData.flag = -1;
    }
    return 0;
}
int QHostCtrl::RButtonDown(int x, int y, WPARAM wParam)
{

    int ret = 0;
    if (m_rect.Contains(x, y)) {
        FocusCtrl = nullptr;
        m_hCursor = m_hCursorARROW;
        bool isto = false;//�Ƿ��пؼ��ӹ����
        if (m_boardCtrl) {
            QHostCtrl* p = nullptr;
            QDWORD lp = (QDWORD(x - m_rect.X) << 32) | ((y - m_rect.Y) & 0xffffffff);
            QWNDAttribute qwa = { 0 };
            qwa.AData = m_boardCtrl->GetAttribute();
            if ((qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout) && m_boardCtrl->Contains(x - m_rect.X, y - m_rect.Y)) {
                ret = m_boardCtrl->RButtonDown(x - m_rect.X, y - m_rect.Y, wParam);
                m_boardCtrl->Redraw();
            }
            if (1 == m_boardCtrl->TransferMessage(QM_POINT_IN_QHOSTCTRL, lp, (QDWORD)&p, 0)) {
                isto = true;
                isSelect = false;
            }
            m_boardCtrl->Redraw();
        }
        

        if (!isto) {
            isrDown = true;
      
            QWNDAttribute qwa = { 0 };
            qwa.AData = m_boardCtrl->GetAttribute();
            if (qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout) {
                __DownData.flag = 5;
                m_MouseStartX = x - m_rect.X;
                m_MouseStartY = y - m_rect.Y;
                m_MouseCurrentX = x - m_rect.X;
                m_MouseCurrentY = y - m_rect.Y;
            }
        

        }
        //m_Parent->TransferMessage(QM_QHOST_LDOWN, (QDWORD)this, (QDWORD)m_boardCtrl, 0);
    }
    isChange = true;

    return ret;
}

int QHostCtrl::RButtonUp(int x, int y, WPARAM wParam)
{
    bool isto = false;//�Ƿ��пؼ��ӹ����
    if (m_boardCtrl) {
        QHostCtrl* p = nullptr;
        QDWORD lp = (QDWORD(x - m_rect.X) << 32) | ((y - m_rect.Y) & 0xffffffff);
        QWNDAttribute qwa = { 0 };
        qwa.AData = m_boardCtrl->GetAttribute();
        if ((qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout) && m_boardCtrl->Contains(x - m_rect.X, y - m_rect.Y)) {
            m_boardCtrl->RButtonUp(x - m_rect.X, y - m_rect.Y, wParam);
            m_boardCtrl->Redraw();
        }
        if (1 == m_boardCtrl->TransferMessage(QM_POINT_IN_QHOSTCTRL, lp, (QDWORD)&p, 0)) {
            isto = true;

        }
    }

    if (5 == __DownData.flag) {
        if (m_Parent) {
            QDWORD lp = QDWORD(m_boardCtrl);
            QDWORD rp = (QDWORD((std::min)(m_MouseCurrentX, m_MouseStartX)) << 32) | (QDWORD((std::min)(m_MouseCurrentY, m_MouseStartY)&0xffffffff));
            QDWORD ap = (QDWORD((std::max)(m_MouseCurrentX, m_MouseStartX)) << 32) | (QDWORD((std::max)(m_MouseCurrentY, m_MouseStartY) & 0xffffffff));
            m_Parent->TransferMessage(QM_QHOST_RDOWN, lp, rp, ap);
        }
        
    }

    /*if (!isto)*/ {

        isrDown = false;
        __DownData.flag = -1;
    }
    return 0;
}

int QHostCtrl::MouseMove(int x, int y, WPARAM wParam)
{
    bool isto = false;//�Ƿ��пؼ��ӹ����
    m_hCursor = m_hCursorARROW;
    if (m_boardCtrl) {
        QHostCtrl* p = nullptr;
        QDWORD lp = (QDWORD(x - m_rect.X) << 32) | ((y - m_rect.Y) & 0xffffffff);
        QWNDAttribute qwa = { 0 };
        qwa.AData = m_boardCtrl->GetAttribute();
        if ((qwa.Attribute.isContainerCtrl || qwa.Attribute.isLayout || qwa.Attribute.HostCtrlMouse) && m_boardCtrl->Contains(x - m_rect.X, y - m_rect.Y)) {
            m_boardCtrl->MouseMove(x - m_rect.X, y - m_rect.Y, wParam);
            m_boardCtrl->Redraw();
            if (1 == m_boardCtrl->TransferMessage(QM_POINT_IN_QHOSTCTRL, lp, (QDWORD)&p, 0)) {
                isto = true;
                m_hCursor = p->GetCursor();
            }
        }
        m_boardCtrl->Redraw();

    }

    if (!isto) {

        if (isDown) {
            switch (__DownData.flag)
            {
            case 0: {
                QWNDAttribute qwa;
                qwa.AData = m_boardCtrl->GetAttribute();
                if (qwa.Attribute.isLayout) {
                    QLayout* p = (QLayout*)m_boardCtrl;
                    int type = p->Type();
                    bool isupdate = false;
                    switch (type)
                    {
                    case 3:
                    case 2: {
                        isupdate = true;
                    }break;



                    }
                    if (isupdate) break;

                }
                float _y = __DownData.dta + y;
                int bm = m_rect.GetBottom();
                if (_y < bm) {
                    std::unique_lock<std::mutex>lk(m_lock);
                    m_rect.Y = _y;
                    m_rect.Height = bm - _y;

                    Reconstruct(NULL);//���´���
                    //ReleaseDC();
                    //QCreateDC(m_dc);
                    ReconstructDC(m_pageDC, m_dc, m_pageHB, m_rect);
                    RECT rc(0, 0, m_rect.Width, m_rect.Height);
                    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
                    if (m_boardCtrl) {
                        GRect rect(0, 0, m_rect.Width, m_rect.Height);
                        rect.Inflate(-1, -1);
                        
                        QWNDAttribute qa;
                        qa.AData=m_boardCtrl->GetAttribute();
                        if (qa.Attribute.isLayout) {
                            m_boardCtrl->SetLocation(GPoint(1,1));
                            QDWORD lp;
                            lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height)&0xffffffff);
                            m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp,0,0);
                        }
                        else {
                            m_boardCtrl->Reconstruct(m_dc, rect);
                        }
                        
                        m_boardCtrl->Redraw();
                    }
                }
            }
                  break;
            case 1: {
                QWNDAttribute qwa;
                qwa.AData = m_boardCtrl->GetAttribute();
                if (qwa.Attribute.isLayout) {
                    QLayout* p = (QLayout*)m_boardCtrl;
                    int type = p->Type();
                    bool isupdate = false;
                    switch (type)
                    {
                    case 3:
                    case 1: {
                        isupdate = true;
                    }break;



                    }
                    if (isupdate) break;

                }
                float _rt = __DownData.dta + x;

                if (m_rect.X < _rt) {
                    std::unique_lock<std::mutex>lk(m_lock);
                    m_rect.Width = _rt - m_rect.X;

                    Reconstruct(NULL);//���´���
                    //ReleaseDC();
                    //QCreateDC(m_dc);
                    ReconstructDC(m_pageDC, m_dc, m_pageHB, m_rect);
                    RECT rc(0, 0, m_rect.Width, m_rect.Height);
                    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
                    if (m_boardCtrl) {
                        GRect rect(0, 0, m_rect.Width, m_rect.Height);
                        rect.Inflate(-1, -1);
                        QWNDAttribute qa;
                        qa.AData = m_boardCtrl->GetAttribute();
                        if (qa.Attribute.isLayout) {
                            m_boardCtrl->SetLocation(GPoint(1, 1));
                            QDWORD lp;
                            lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height) & 0xffffffff);
                            m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);
                        }
                        else {
                            m_boardCtrl->Reconstruct(m_dc, rect);
                        }
                        m_boardCtrl->Redraw();
                    }
                }

            }
                  break;
            case 2: {
                QWNDAttribute qwa;
                qwa.AData = m_boardCtrl->GetAttribute();
                if (qwa.Attribute.isLayout) {
                    QLayout* p = (QLayout*)m_boardCtrl;
                    int type = p->Type();
                    bool isupdate = false;
                    switch (type)
                    {
                    case 3:
                    case 2: {
                        isupdate = true;
                    }break;



                    }
                    if (isupdate) break;

                }
                float _bm = __DownData.dta + y;

                if (m_rect.Y < _bm) {
                    std::unique_lock<std::mutex>lk(m_lock);
                    m_rect.Height = _bm - m_rect.Y;

                    Reconstruct(NULL);//���´���
                    //ReleaseDC();
                    //QCreateDC(m_dc);
                    ReconstructDC(m_pageDC, m_dc, m_pageHB, m_rect);
                    RECT rc(0, 0, m_rect.Width, m_rect.Height);
                    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
                    if (m_boardCtrl) {
                        GRect rect(0, 0, m_rect.Width, m_rect.Height);
                        rect.Inflate(-1, -1);
                        QWNDAttribute qa;
                        qa.AData = m_boardCtrl->GetAttribute();
                        if (qa.Attribute.isLayout) {
                            m_boardCtrl->SetLocation(GPoint(1, 1));
                            QDWORD lp;
                            lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height) & 0xffffffff);
                            m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);
                        }
                        else {
                            m_boardCtrl->Reconstruct(m_dc, rect);
                        }
                        m_boardCtrl->Redraw();
                    }
                }
            }
                  break;
            case 3: {
                QWNDAttribute qwa;
                qwa.AData = m_boardCtrl->GetAttribute();
                if (qwa.Attribute.isLayout) {
                    QLayout* p = (QLayout*)m_boardCtrl;
                    int type = p->Type();
                    bool isupdate = false;
                    switch (type)
                    {
                    case 3:
                    case 1: {
                        isupdate = true;
                    }break;



                    }
                    if (isupdate) break;

                }
 
                float _x = __DownData.dta + x;

                if (_x < m_rect.GetRight()) {
                    std::unique_lock<std::mutex>lk(m_lock);
                    int rt = m_rect.GetRight();
                    m_rect.X = _x;
                    m_rect.Width = rt - _x;
                    Reconstruct(NULL);//���´���
  
                    ReconstructDC(m_pageDC, m_dc, m_pageHB, m_rect);
                    RECT rc(0, 0, m_rect.Width, m_rect.Height);
                    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
                    if (m_boardCtrl) {
                        GRect rect(0, 0, m_rect.Width, m_rect.Height);
                        rect.Inflate(-1, -1);
                        QWNDAttribute qa;
                        qa.AData = m_boardCtrl->GetAttribute();
                        if (qa.Attribute.isLayout) {
                            m_boardCtrl->SetLocation(GPoint(1, 1));
                            QDWORD lp;
                            lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height) & 0xffffffff);
                            m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);
                        }
                        else {
                            m_boardCtrl->Reconstruct(m_dc, rect);
                        }
                        m_boardCtrl->Redraw();
                    }
                }


            }
                  break;
            case 4: {
                std::unique_lock<std::mutex>lk(m_lock);
                GRect rt = m_rect;
                rt.X = __DownData.dta + x;
                rt.Y = __DownData.dta2 + y;


                if (m_boardCtrl) {
                    QWNDAttribute qwa;
                    qwa.AData = m_boardCtrl->GetAttribute();
                    if (qwa.Attribute.isLayout) {
                        QLayout* p = (QLayout*)m_boardCtrl;
                        int type = p->Type();
                        switch (type)
                        {
                        case 1: {
                            m_rect.Y = rt.Y;
                        }break;
                        case 2: {
                            m_rect.X = rt.X;
             
                        }break;
                        case 3: {
                            m_rect.Y = 0;
                            m_rect.X = 0;
                        }break;

                        }

                    }
                    else {
                        m_rect = rt;
                    }
                    m_boardCtrl->Redraw();

                }
             
            }
                  break;


            }

            isChange = true;

        }
        else if (isrDown) {
            switch (__DownData.flag)
            {
            case 5: {
                m_MouseCurrentX = x - m_rect.X;
                m_MouseCurrentY = y - m_rect.Y;


            }break;

            }

            isChange = true;

        }
        
        if (isSelect) {
            if (m_rect.Contains(x, y)) {
                if (abs(m_rect.GetRight() - x) < 5) {
                    m_hCursor = m_hCursorWE;
                }
                else if (abs(m_rect.X - x) < 5) {
                    m_hCursor = m_hCursorWE;

                }
                else if (abs(m_rect.GetBottom() - y) < 5) {
                    m_hCursor = m_hCursorNS;
                }
                else if (abs(m_rect.Y - y) < 5) {
                    m_hCursor = m_hCursorNS;

                }
                else if(!isrDown){

                    m_hCursor = m_hCursorSIZEALL;
                }

            }
        }


    }
    isChange = true;


    return 0;
}

int QHostCtrl::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
        std::unique_lock<std::mutex>lk(m_lock);
        flag = 1;
        isChange = false;
        //1.sdc�ж�Ӧ��λ�ÿ�������
        StretchBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height,
            sdc, m_rect.X, m_rect.Y, m_rect.Width , m_rect.Height, SRCCOPY);
        
        //2.��������
        {



        }
        //�����ӿؼ�
        {
            if (m_boardCtrl) {
                if (m_boardCtrl->Draw(m_dc)) {
                    isChange = true;
                }

            }


        }

        if(isSelect)
        {

            // Begin drawing
            m_pRenderTarget->BeginDraw();
            // Draw something
            ID2D1SolidColorBrush* pBrush = NULL;
            auto test = D2D1::ColorF::Black;
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.5), &pBrush);
            D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, m_rect.Width, m_rect.Height);
            ID2D1StrokeStyle* pStrokeStyle = nullptr;
            D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties();
            strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DASH;

            m_pD2DFactory->CreateStrokeStyle(strokeStyleProperties, nullptr, 0, &pStrokeStyle);

            // Draw dashed rectangle
         
            m_pRenderTarget->DrawRectangle(layoutRect, pBrush, 2.0f, pStrokeStyle);

            pStrokeStyle->Release();



            pBrush->Release();

            if (5 == __DownData.flag) {
                D2D1_RECT_F rect = D2D1::RectF((std::min)(m_MouseStartX, m_MouseCurrentX), (std::min)(m_MouseStartY, m_MouseCurrentY),
                    (std::max)(m_MouseStartX, m_MouseCurrentX), (std::max)(m_MouseStartY, m_MouseCurrentY));
                //D2D1_RECT_F rect(10,10,50,50);
                ID2D1StrokeStyle* pStrokeStyle = nullptr;
                D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_LINE_JOIN_MITER,
                    1.0f,
                    D2D1_DASH_STYLE_DASH,
                    0.0f
                );

                // ����������ʽ
                HRESULT hr = m_pD2DFactory->CreateStrokeStyle(
                    strokeStyleProperties,
                    nullptr,
                    0,
                    &pStrokeStyle
                );
                // ��������
                ID2D1SolidColorBrush* pBrush = nullptr;
                m_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::Black),
                    &pBrush
                );
                m_pRenderTarget->DrawRectangle(rect, pBrush, 1.0, pStrokeStyle);
                pBrush->Release();
                pStrokeStyle->Release();


            }



            // End drawing
            m_pRenderTarget->EndDraw();
        }

   

  
    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}

int QHostCtrl::MouseMoveOut()
{
    m_hCursor = m_hCursorARROW;
    
    return 0;
}

int QHostCtrl::MouseWheel(int x, int y, WPARAM wParam)
{

    return m_boardCtrl->MouseWheel(x,y, wParam);
}

#include "QFTabs.h"

#include <format>
int QHostCtrl::KeyChar(WPARAM wParam, LPARAM lParam)
{
    if (m_boardCtrl) {
        return m_boardCtrl->KeyChar(wParam, lParam);
    
    }

    return 0;
}
int QHostCtrl::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    if (m_boardCtrl) {
        return m_boardCtrl->KeyIMEChar(wParam, lParam);

    }
    return 0;
}

void QHostCtrl::Test()
{
    //m_boardCtrl = new QTabs(m_dc,GRect(1,1,m_rect.Width-1,m_rect.Height-1));
    //QTabs* qt = (QTabs*)m_boardCtrl;
    //for (int i = -1; ++i < 3;) {
    //    qt->AddTab((L"����" + std::to_wstring(i)).c_str(), L"Tabico.png");

    //}

    //QLineChart* qlc = (QLineChart*)m_boardCtrl;
    ////������ͼ������һ������ͼ��
    //std::vector<GPointF> srcPF;
    //for (GREAL x = 0.0; x < 20; x += 3.14 / 8) {
    //    srcPF.push_back(GPointF(x, std::sin(x)));
    //}
    //qlc->AddCurve(srcPF.data(), srcPF.size());

}

int QHostCtrl::KeyDown(WPARAM wParam, LPARAM lParam)
{
    if (m_boardCtrl) {
        return m_boardCtrl->KeyDown(wParam, lParam);

    }
    return 0;
}

int QHostCtrl::KeyUp(WPARAM wParam, LPARAM lParam)
{
    if (m_boardCtrl) {
        return m_boardCtrl->KeyUp(wParam, lParam);

    }
    return 0;
}

int QHostCtrl::SetParasiticCtrl(QWND* ctrl, QWND** oldCtrl)
{
    QWNDAttribute qwa;
    qwa.AData = ctrl->GetAttribute();
    if (qwa.Attribute.isQHostCtrl)return -1;
    if (oldCtrl) {
        *oldCtrl = m_boardCtrl;
    }
    m_boardCtrl = ctrl;
    ((QHostCtrl*)m_boardCtrl)->m_Parent = this;
    ((QHostCtrl*)m_boardCtrl)->m_HostHwnd = m_HostHwnd;
    GRect rect(0, 0, m_rect.Width, m_rect.Height);
    rect.Inflate(-1, -1);
    m_boardCtrl->Reconstruct(m_dc, rect);
    if (1 == qwa.Attribute.isLayout) {
        QDWORD lp = 0;
        lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height) & 0xffffffff);
        m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

    }
    m_boardCtrl->Redraw();
    return 1;
}
QWND* QHostCtrl::GetParasiticCtrl()
{
    return m_boardCtrl;
}
void QHostCtrl::killFocus()
{
    isSelect = false;
    isChange = true;
   
}

QDWORD QHostCtrl::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_QHOST_LDOWN: {
        isSelect = false;
        FocusCtrl = (QWND*)RParam;
        if (m_Parent) {

            return m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);
        }
    }

    return 1;

    case QM_HOSTCTRL_INSERT_PAIR: {

        const char* key = (char*)LParam;
        const char* value = (char*)RParam;

        __pairArr[std::string(key)] = value;


    }break;
    case QM_HOSTCTRL_GET_PAIR: {

        int index = AddParam & 0x7fffffff;
        int count = __pairArr.size();
        if (LParam && RParam) {
 
            if (index > -1 && index < DWORD(__pairArr.size())) {
                
                const char** k = (const char**)LParam;
                const char** v = (const char**)RParam;
                
                for (int i = -1; auto & it:__pairArr) {
                    if (++i == index) {
                        *k = it.first.c_str();
                        *v = it.second.c_str();
                    }
                
                
                }
                
               
            
            }
            
        
        }


        return count;

    }break;
    case QM_PARENTCTRL_SIZE_CHANGE: {
        if (m_boardCtrl) {
            int w = LParam >> 32;
            int h = LParam & 0xffffffff;
            QWNDAttribute qwa;
            qwa.AData = m_boardCtrl->GetAttribute();
            if (qwa.Attribute.isLayout) {
                QLayout* p = (QLayout*)m_boardCtrl;
                int type = p->Type();
                switch (type)
                {
               
                case 1: {
                    GRect rect = m_rect;
                    rect.X = 0;
                    rect.Width = w;
                    Reconstruct(NULL, rect);
                }break;
                case 2: {
                    GRect rect = m_rect;
                    rect.Y = 0;
                    rect.Height = h;
                    Reconstruct(NULL, rect);
                }break;
                case 3: {
                    GRect rect ;
                    rect.Y = 0;
                    rect.X = 0;
                    rect.Height = h;
                    rect.Width = w;
                    Reconstruct(NULL, rect);

                }break;

                }

            }
        }


    }break;
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            m_boardCtrl->Redraw();

        }


    }break;



    }
  



    return QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}

void QHostCtrl::Reconstruct(HDC sdc)
{
    QWND::Reconstruct(sdc);
}

void QHostCtrl::Reconstruct(HDC sdc, const GRect& rect)
{
    QWND::Reconstruct(sdc, rect);
    RECT rc(0, 0, rect.Width, rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
    if (m_boardCtrl) {
        GRect rt(rect);
        rt.X = 0;
        rt.Y = 0;
        rt.Inflate(-1, -1);
        m_boardCtrl->Reconstruct(m_dc,rt);
        QDWORD lp = 0;
        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
        m_boardCtrl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);
      
    
    }
    isChange = true;
}

















#endif // __QHOSTCTRL__
extern "C"
{
    __declspec(dllexport) int QHCSetParasiticCtrl(QHostCtrl* p, QWND* ctrl, QWND** oldCtrl)
    {
       return p->SetParasiticCtrl(ctrl, oldCtrl);
    }
    __declspec(dllexport) QWND* QHCGetParasiticCtrl(QHostCtrl* p)
    {
    
        return p->GetParasiticCtrl();
    }

}
#ifdef  __QHLAYOUT__
void QHLayout::Redraw()
{
    isChange = true;
    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
    for (auto& it : m_QWNDs) {
        it.pWnd->Redraw();

    }
}

void QHLayout::Refresh()
{
    isChange = true;
    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
    for (auto& it : m_QWNDs) {
        it.pWnd->Refresh();

    }

}

QHLayout::QHLayout(HDC winddc, const GRect& rf):QLayout(winddc, rf)
{
   


#ifdef _DEBUG
    Test();
#endif
}

int QHLayout::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
        std::shared_lock<std::shared_mutex> dlk(m_Draw_lk);
        flag = 1;
        auto now = std::chrono::high_resolution_clock::now();
        long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __ct).count();
        if (dta > 5000) {
            isChange = false;
        }
        //1.sdc�ж�Ӧ��λ�ÿ�������
        StretchBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height,
            sdc, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height, SRCCOPY);


        std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
        bool isLayout = false;
        GRect rect;

        for (auto& it : m_QWNDs) {
            if (it.pWnd->Draw(m_dc)) {


            }

            if (rect.IsEmptyArea()) {
                rect = it.pWnd->GetRect();
        }
            else {
                GRect rt = it.pWnd->GetRect();
                if (rect.IntersectsWith(rt)) {
                    isLayout = true;

                }
                else {

                    rect.Union(rect, rt, GRect(rect));

                }

            }



    }
    if (isLayout) {
        //����û���
        CtrlReconstruct();


    }
        
        
#ifndef _DEBUG
        if (0 == m_QWNDs.size())
#endif
        {
            Gdiplus::Graphics graphics(m_dc);
            GPen pen(GColor(0xff, 0xe0, 0, 0));
            pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDotDot);
            GRect rect(0, 0, m_rect.Width, m_rect.Height);
            rect.Inflate(-1, -1);
            graphics.DrawRectangle(&pen, rect);

        }



    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}
QHLayout::~QHLayout()
{
}

QDWORD QHLayout::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{

    switch (msgID) {
    case QM_PARENTCTRL_SIZE_CHANGE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;
        if (w == m_rect.Width)return 1;

        //���򲼾���ֻ��ע��ȣ��߶ȱ���
        GRect rect;
        rect.X = 0;
        rect.Y = m_rect.Y;
        rect.Height = m_rect.Height;
        rect.Width = w;
        h = m_rect.Height;
        {
            //std::unique_lock<std::shared_mutex> lk(m_Draw_lk);
            QLayout::Reconstruct(NULL, rect);
            //ֹͣ�ӿؼ�
            //h = m_rect.Height;
            //QDWORD lp = (QDWORD(w) << 32) | (QDWORD(h)&0xffffffff);
            std::shared_lock<std::shared_mutex> dlk(m_QWND_lk);
            unsigned int total = 0;
            int assigned = 0;//Ȩ��Ϊ0���ܿ��
            for (auto& it : m_QWNDs) {
                total += it.weight;
                if (0 == it.weight) {
                    assigned += it.pWnd->GetRect().Width;
                }
            }
            if (0 == total) {
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = start;
                    rt.Y = 0;
                    rt.Height = h;
                    it.pWnd->Reconstruct(m_dc, rt);
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if(1 == qwa.Attribute.isLayout){
                        QDWORD lp=0;
                        lp = (QDWORD(rect.Width)<<32)|(QDWORD(rect.Height)&0xffffffff);
                        it.pWnd->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp,0,0);
                       
                    }
               
                      
                
                    start += rt.Width;
                }

            }
            else {//��Ȩ�ص�
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = start;
                    rt.Y = 0;
                    rt.Height = h;
                    if (0 != it.weight) {
                        float ato = (float)it.weight / total;
                        int dta = m_rect.Width - assigned;
                        if (dta < 0)dta = m_rect.Width-5;
                        rt.Width = ato * dta;
                    }
                    it.pWnd->Reconstruct(m_dc, rt);
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rect.Width) << 32) | (QDWORD(rect.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);
                        
                    }
                   
                       
                    
                    start += rt.Width;

                }
            }
        }
        isChange = true;
        //Draw(m_sdc);
        return 1;

    }break;
    case QM_SETWINDOWS_SIZE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;


        //���򲼾���ֻ��ע��ȣ��߶ȱ���
        GRect rect;
        rect.X = m_rect.X;
        rect.Y = 0;
        rect.Height = h;
        rect.Width = w;
        {
            if (w != m_rect.Width || h != m_rect.Height) {
                QLayout::Reconstruct(NULL, rect);
            }

            std::shared_lock<std::shared_mutex> dlk(m_QWND_lk);
            unsigned int total = 0;
            int assigned = 0;//Ȩ��Ϊ0���ܿ��
            for (auto& it : m_QWNDs) {
                total += it.weight;
                if (0 == it.weight) {
                    assigned += it.pWnd->GetRect().Width;
                }
            }
            if (0 == total) {
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = start;
                    rt.Y = 0;
                    rt.Height = h;
                    it.pWnd->Reconstruct(m_dc, rt);
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                    start += rt.Width;
                }

            }
            else {//��Ȩ�ص�
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = start;
                    rt.Y = 0;
                    rt.Height = h;
                    if (0 != it.weight) {
                        float ato = (float)it.weight / total;
                        int dta = m_rect.Width - assigned;
                        if (dta < 0)dta = m_rect.Width - 5;
                        rt.Width = ato * dta;
                    }
                    it.pWnd->Reconstruct(m_dc, rt);
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                    start += rt.Width;

                }
            }
        }
        isChange = true;

        return 1;


    }break;
    case QM_PLACE_CTRL: {
        QWND* p = (QWND*)LParam;
        GRect rect = p->GetRect();
        int i = -1; {
            std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

            for (; ++i < m_QWNDs.size();) {
                GRect rt = m_QWNDs[i].pWnd->GetRect();
                if (rect.X < rt.X)break;
            }
        }
        InsertCtrl(p, RParam, i);
        

    }break;
    case QM_REMOVE_CTRL: {
        QWND* p = (QWND*)LParam;
        std::unique_lock<std::shared_mutex> lk(m_QWND_lk);

        for (int i = -1; ++i < m_QWNDs.size();) {
            if (m_QWNDs[i].pWnd == p) {
                m_QWNDs.erase(m_QWNDs.begin() + i);
                return 2;
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
            std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

            for (auto& it : m_QWNDs) {
                it.pWnd->Redraw();

            }


        }


    }break;
    case QM_POINT_IN_CTRL: {
        std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

        for (auto& it : m_QWNDs) {
            if (it.pWnd->Contains(LParam, RParam)) {

                return it.pWnd->TransferMessage(QM_POINT_IN_CTRL, LParam - m_rect.X, RParam - m_rect.Y, 0);
            }
        }
    }
    }



    return QLayout::TransferMessage(msgID, LParam, RParam, AddParam);

}
#include "QFButton.h"
#include "QFEdit.h"
void QHLayout::Test()
{
    /*QLayoutObj obj;
    QButton* bt = new QButton(m_dc,GRect(0,0,100,50));
    obj.pWnd = bt;
    obj.weight = 1;
    m_QWNDs.push_back(obj);
    QEdit* ed = new QEdit(m_dc, GRect(0, 0, 100, 50));
    obj.pWnd = ed;
    obj.weight = 0;
    m_QWNDs.push_back(obj);*/

}

int QHLayout::MouseMove(int x, int y, WPARAM wParam)
{
    m_mousex = x - m_rect.X;
    m_mousey = y - m_rect.Y;
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int pcx = px;
    int pcy = py ;
    ret = MouseMessage(WM_MOUSEMOVE, wParam, pcx, pcy);
    


    return ret;
}
int QHLayout::MouseMoveOut()
{
    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);

    for (auto& it : m_QWNDs) {
        it.pWnd->MouseMoveOut();
    }
    mouseCapture = NULL;
    return 0;
}
int QHLayout::KeyChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_CHAR, wParam, lParam);
    return 0;
}
int QHLayout::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_IME_CHAR, wParam, lParam);
    return 0;
}
int QHLayout::KeyDown(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYDOWN, wParam, lParam);
    return 0;
}
int QHLayout::KeyUp(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYUP, wParam, lParam);
    return 0;
}

void QHLayout::killFocus()
{
    if(mouseFocusCtrl)
    mouseFocusCtrl->killFocus();
    if (FocusCtrl) {
        FocusCtrl->killFocus();
    }
}

inline int QHLayout::MouseMessage(UINT uMsg, WPARAM wParam, int x, int y)
{
    int ret = 0;
 
    POINT pt;
    pt.x = x;
    pt.y = y;

    {
        bool isAtCtrl = false;
        HCURSOR hCursor = m_hCursorBuff;
        std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
       
        for (auto pit = m_QWNDs.rbegin(); pit != m_QWNDs.rend(); ++pit) {
            auto& it = pit->pWnd;
            if (mouseCapture && it == mouseCapture) {
                ret = MoseMsg(mouseCapture, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ

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
                        ret = MoseMsg(mouseFocusCtrl, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ
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

        //���ö�Ӧ�ؼ������
        m_hCursor = hCursor;
        /*      SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
              ::SetCursor(hCursor);*/
        if (!isAtCtrl) {
            //����˲��ǿؼ���ԭ����ؼ�ʧȥ����
            //֪ͨԭ����ؼ�ʧȥ����

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
    return ret;
}

inline int QHLayout::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
{
    int ret = 0;
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        //mouseSeleFocusCtrl = qwnd;
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
        if (FocusCtrl && (FocusCtrl != qwnd)) {
            FocusCtrl->killFocus();

            FocusCtrl = NULL;


        }
        if (mouseSeleFocusCtrl) {
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
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
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
void QHLayout::SetLocation(const GPoint& point)
{
    GPoint pt = point;
    pt.X = m_rect.X;
    QLayout::SetLocation(pt);
}
void QHLayout::SetSize(const GSize& size, HDC dc)
{
    GSize sz= size;
    sz.Width = m_rect.Width;
    QLayout::SetSize(sz, dc);
}
inline void QHLayout::KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN: {//û�����뷢���ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyDown(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_KEYUP: {//û�����뷢���ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyUp(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_IME_CHAR: {//���뷨�������ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyIMEChar(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }

        return;
    }
    case WM_CHAR: {//���뷨�������ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyChar(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }


    }
}

inline void QHLayout::CtrlReconstruct()
{

    unsigned int total = 0;
    int assigned = 0;//Ȩ��Ϊ0���ܿ��
    for (auto& it : m_QWNDs) {
        total += it.weight;
        if (0 == it.weight) {
            assigned += it.pWnd->GetRect().Width;
        }
    }
    if (0 == total) {
        int start = 0;
        for (auto& it : m_QWNDs) {
            GRect rt = it.pWnd->GetRect();
            rt.X = start;
            rt.Y = 0;
            rt.Height = m_rect.Height;
            it.pWnd->Reconstruct(m_dc, rt);
            QWNDAttribute qwa;
            qwa.AData = it.pWnd->GetAttribute();
            if (1 == qwa.Attribute.isLayout) {
                QDWORD lp = 0;
                lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

            }
           
            start += rt.Width;
        }

    }
    else {//��Ȩ�ص�
        int start = 0;
        for (auto& it : m_QWNDs) {
            GRect rt = it.pWnd->GetRect();
            rt.X = start;
            rt.Y = 0;
            rt.Height = m_rect.Height;
            if (0 != it.weight) {
                float ato = (float)it.weight / total;
                int dta = m_rect.Width - assigned;
                if (dta < 0)dta = m_rect.Width - 5;
                rt.Width = ato * dta;
            }
            it.pWnd->Reconstruct(m_dc, rt);
            QWNDAttribute qwa;
            qwa.AData = it.pWnd->GetAttribute();
            if (1 == qwa.Attribute.isLayout) {
                QDWORD lp = 0;
                lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

            }
        
            start += rt.Width;

        }
    }
    isChange = true;
    
}

int QHLayout::InsertCtrl(QWND* ctrl, unsigned int weight, int index)
{
    if (index<-1 || index > int(m_QWNDs.size()))return -1;
    QLayoutObj qlo;
    qlo.weight = weight;
    qlo.pWnd = ctrl;
    ((QHLayout*)ctrl)->m_Parent = this;
    ((QHLayout*)ctrl)->m_HostHwnd = m_HostHwnd;
    if (-1 == index)index = m_QWNDs.size();
    std::unique_lock<std::shared_mutex> lk(m_QWND_lk);
    m_QWNDs.insert(m_QWNDs.begin()+ index, qlo);
    CtrlReconstruct();
    isChange = true;
    return 1;
}
int QHLayout::RemoveCtrl(int index)
{
    if (index<0 || index > int(m_QWNDs.size()-1))return -1;
    std::unique_lock<std::shared_mutex> lk(m_QWND_lk);
    m_QWNDs.erase(m_QWNDs.begin() + index);
    CtrlReconstruct();
    isChange = true;
}

QWND* QHLayout::GetCtrl(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
    if (index < 0 || index >QDWORD(m_QWNDs.size()) - 1)return nullptr;

    return m_QWNDs[index].pWnd;
}

unsigned int QHLayout::GetCtrlWeight(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
    if (index < 0 || index >QDWORD(m_QWNDs.size()) - 1)return 0;

    return m_QWNDs[index].weight;
}

int QHLayout::GetCtrlCount()
{

    return m_QWNDs.size();
}



int QHLayout::LButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;

    int pcx = px;
    int pcy = py ;
    ret = MouseMessage(WM_LBUTTONDOWN, wParam, pcx, pcy);
    


    return ret;
}



int QHLayout::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    int pcx = px;
    int pcy = py;
    ret = MouseMessage(WM_LBUTTONDBLCLK, wParam, pcx, pcy);
  
    return ret;
}

void QHLayout::Reconstruct(HDC sdc, const GRect& rect)
{
    GRect rt(rect);
    rt.X = m_rect.X;
    rt.Width = m_rect.Width;
    return QLayout::Reconstruct(sdc, rt);
}
void QHLayout::Reconstruct(HDC sdc)
{
    return QLayout::Reconstruct(sdc);
}
int QHLayout::LButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_LBUTTONUP, wParam, px, py);
    
    return ret;
}



int QHLayout::RButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    int pcx = px;
    int pcy = py;
    ret = MouseMessage(WM_RBUTTONDOWN, wParam, pcx, pcy);
    
    return ret;
}

int QHLayout::RButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    int pcx = px;
    int pcy = py;
    ret = MouseMessage(WM_RBUTTONUP, wParam, pcx, pcy);
   
    return ret;
}

int QHLayout::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_RBUTTONDBLCLK, wParam, px, py);
 
    return ret;
}

int QHLayout::MButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MBUTTONDOWN, wParam, px, py);

    return ret;
}

int QHLayout::MButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_MBUTTONUP, wParam, px, py);

    return ret;
}

int QHLayout::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MBUTTONDBLCLK, wParam, px, py);

    return ret;
}

int QHLayout::MouseWheel(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MOUSEWHEEL, wParam, px, py);
 
    return ret;
}



















#endif //  __QHLAYOUT__
extern "C"
{
    __declspec(dllexport) int QHLInsertCtrl(QHLayout* p, QWND* ctrl, unsigned int weight, int index)
    {
    
        return p->InsertCtrl(ctrl, weight, index);
    }
    __declspec(dllexport) int QHLRemoveCtrl(QHLayout* p,int index)
    {

        return p->RemoveCtrl(index);
    }
    __declspec(dllexport) int QHLGetCtrlCount(QHLayout* p)
    {

        return p->GetCtrlCount();
    }
    __declspec(dllexport) QWND* QHLGetCtrl(QHLayout* p,int index)
    {

        return p->GetCtrl(index);
    }
    __declspec(dllexport) int QHLGetCtrlWeight(QHLayout* p, int index)
    {

        return p->GetCtrlWeight(index);
    }
}

#ifdef  __QVLAYOUT__

QVLayout::QVLayout(HDC winddc, const GRect& rf):QLayout(winddc,rf)
{
#ifdef _DEBUG
    Test();
#endif
}

void QVLayout::Redraw()
{

    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
    for (auto& it : m_QWNDs) {
        it.pWnd->Redraw();

    }
    isChange = true;
    UpdateWND();
}

void QVLayout::Refresh()
{
    isChange = true;
    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
    for (auto& it : m_QWNDs) {
        it.pWnd->Refresh();

    }

}

int QVLayout::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
       
        auto now = std::chrono::high_resolution_clock::now();
        long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __ct).count();
        if (dta > 5000) {
            isChange = false;
        }
        std::shared_lock<std::shared_mutex> dlk(m_Draw_lk);
        flag = 1;
        
        //1.sdc�ж�Ӧ��λ�ÿ�������
        StretchBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height,
            sdc, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height, SRCCOPY);
        

        std::shared_lock<std::shared_mutex>lk(m_QWND_lk);
        bool isLayout = false;
        GRect rect;
        
        for (auto& it : m_QWNDs) {
            if (it.pWnd->Draw(m_dc)) {


            }

            if (rect.IsEmptyArea()) {
                rect = it.pWnd->GetRect();
            }
            else {
                GRect rt = it.pWnd->GetRect();
                if (rect.IntersectsWith(rt)) {
                    isLayout = true;
                
                }
                else {
                    
                    rect.Union(rect,rt, GRect(rect));

                }
            
            }



        }
        if (isLayout) {
            //����û���
            CtrlReconstruct();
        
        
        }



#ifndef _DEBUG
        if (0 == m_QWNDs.size())
#endif
        {
            Gdiplus::Graphics graphics(m_dc);
            GPen pen(GColor(0xff, 0, 0xa0, 0));
            pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDotDot);
            GRect rect(0, 0, m_rect.Width, m_rect.Height);
            rect.Inflate(-1, -1);
            graphics.DrawRectangle(&pen, rect);

        }



    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}

void QVLayout::Reconstruct(HDC sdc, const GRect& rect)
{
    GRect rt(rect);
    
    return QLayout::Reconstruct(sdc, rt);
}

void QVLayout::Reconstruct(HDC sdc)
{
    return QLayout::Reconstruct(sdc);
}

QVLayout::~QVLayout()
{
}

int QVLayout::MouseMove(int x, int y, WPARAM wParam)
{
    m_mousex = x - m_rect.X;
    m_mousey = y - m_rect.Y;
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    ret = MouseMessage(WM_MOUSEMOVE, wParam, px, py);



    return ret;
}

int QVLayout::LButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;

    ret = MouseMessage(WM_LBUTTONDOWN, wParam, px, py);



    return ret;
}

int QVLayout::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_LBUTTONDBLCLK, wParam, px, py);

    return ret;
}

int QVLayout::LButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_LBUTTONUP, wParam, px, py);
    return ret;
}

int QVLayout::RButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_RBUTTONDOWN, wParam, px, py);
    return ret;
}

int QVLayout::RButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_RBUTTONUP, wParam, px, py);

    return ret;
}

int QVLayout::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_RBUTTONDBLCLK, wParam, px, py);

    return ret;
}

int QVLayout::MButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MBUTTONDOWN, wParam, px, py);

    return ret;
}

int QVLayout::MButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;


    ret = MouseMessage(WM_MBUTTONUP, wParam, px, py);

    return ret;
}

int QVLayout::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MBUTTONDBLCLK, wParam, px, py);

    return ret;
}

int QVLayout::MouseWheel(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    int ret = 0;

    ret = MouseMessage(WM_MOUSEWHEEL, wParam, px, py);

    return ret;
}
int QVLayout::MouseMoveOut()
{
    std::shared_lock<std::shared_mutex>lk(m_QWND_lk);

    for (auto& it : m_QWNDs) {
        it.pWnd->MouseMoveOut();
    }
    mouseCapture = NULL;
    return 0;
}
int QVLayout::KeyChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_CHAR, wParam, lParam);
    return 0;
}
int QVLayout::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_IME_CHAR, wParam, lParam);
    return 0;
}
int QVLayout::KeyDown(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYDOWN, wParam, lParam);
    return 0;
}
int QVLayout::KeyUp(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYUP, wParam, lParam);
    return 0;
}

void QVLayout::killFocus()
{
    if (mouseFocusCtrl)
        mouseFocusCtrl->killFocus();
    if (FocusCtrl) {
        FocusCtrl->killFocus();
    }
}

inline int QVLayout::MouseMessage(UINT uMsg, WPARAM wParam, int x, int y)
{
    int ret = 0;

    POINT pt;
    pt.x = x;
    pt.y = y;

    {
        bool isAtCtrl = false;
        HCURSOR hCursor = m_hCursorBuff;
        std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

        for (auto pit = m_QWNDs.rbegin(); pit != m_QWNDs.rend(); ++pit) {
            auto& it = pit->pWnd;
            if (mouseCapture && it == mouseCapture) {
                ret = MoseMsg(mouseCapture, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ

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
                        ret = MoseMsg(mouseFocusCtrl, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ
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

        //���ö�Ӧ�ؼ������
        m_hCursor = hCursor;
        /*      SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
              ::SetCursor(hCursor);*/
        if (!isAtCtrl) {
            //����˲��ǿؼ���ԭ����ؼ�ʧȥ����
            //֪ͨԭ����ؼ�ʧȥ����

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

inline int QVLayout::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
{
    int ret = 0;
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        //mouseSeleFocusCtrl = qwnd;
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
        if (FocusCtrl && (FocusCtrl != qwnd)) {
            FocusCtrl->killFocus();

            FocusCtrl = NULL;


        }
        if (mouseSeleFocusCtrl) {
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
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
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
void QVLayout::SetLocation(const GPoint& point)
{
    GPoint pt = point;
    pt.X = m_rect.X;
    QLayout::SetLocation(pt);
    isChange = true;
}
void QVLayout::SetSize(const GSize& size, HDC dc)
{
    GSize sz = size;
    sz.Width = m_rect.Width;
    QLayout::SetSize(sz, dc);
    isChange = true;
}
inline void QVLayout::KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN: {//û�����뷢���ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyDown(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_KEYUP: {//û�����뷢���ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyUp(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }
    case WM_IME_CHAR: {//���뷨�������ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyIMEChar(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }

        return;
    }
    case WM_CHAR: {//���뷨�������ַ�
        if (FocusCtrl) {
            int ret = FocusCtrl->KeyChar(wParam, lParam);
            if (1 == ret) {//�ÿؼ������н���
                FocusCtrl = NULL;
            }
        }
        return;
    }


    }
}

inline void QVLayout::CtrlReconstruct()
{
    unsigned int total = 0;
    int assigned = 0;//Ȩ��Ϊ0���ܿ��
    for (auto& it : m_QWNDs) {
        total += it.weight;
        if (0 == it.weight) {
            assigned += it.pWnd->GetRect().Height;
        }
    }
    if (0 == total) {
        int start = 0;
        for (auto& it : m_QWNDs) {
            GRect rt = it.pWnd->GetRect();
            rt.X = 0;
            rt.Y = start;
            rt.Width = m_rect.Width;
            it.pWnd->Reconstruct(m_dc, rt);
            QWNDAttribute qwa;
            qwa.AData = it.pWnd->GetAttribute();
            if (1 == qwa.Attribute.isLayout) {
                QDWORD lp = 0;
                lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

            }
     
            start += rt.Height;
        }

    }
    else {//��Ȩ�ص�
        int start = 0;
        for (auto& it : m_QWNDs) {
            GRect rt = it.pWnd->GetRect();
            rt.X = 0;
            rt.Y = start;
            rt.Width = m_rect.Width;
            if (0 != it.weight) {
                float ato = (float)it.weight / total;
                int dta = m_rect.Height - assigned;
                if (dta < 0)dta = m_rect.Height - 5;
                rt.Height = ato * dta;
            }
            it.pWnd->Reconstruct(m_dc, rt);
            QWNDAttribute qwa;
            qwa.AData = it.pWnd->GetAttribute();
            if (1 == qwa.Attribute.isLayout) {
                QDWORD lp = 0;
                lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

            }
            
            start += rt.Height;

        }
    }

}

void QVLayout::Test()
{

}

int QVLayout::InsertCtrl(QWND* ctrl, unsigned int weight, int index)
{
    if (index<-1 || index > int(m_QWNDs.size()))return -1;
    QLayoutObj qlo;
    qlo.weight = weight;
    qlo.pWnd = ctrl;
    ((QVLayout*)ctrl)->m_Parent = this;
    ((QVLayout*)ctrl)->m_HostHwnd = m_HostHwnd;
    if (-1 == index)index = m_QWNDs.size();
    std::unique_lock<std::shared_mutex> lk(m_QWND_lk);
    m_QWNDs.insert(m_QWNDs.begin() + index, qlo);
    CtrlReconstruct();
   
    isChange = true;

    return 1;
}
int QVLayout::RemoveCtrl(int index)
{
    if (index<0 || index > int(m_QWNDs.size() - 1))return -1;
    std::unique_lock<std::shared_mutex> lk(m_QWND_lk);
    m_QWNDs.erase(m_QWNDs.begin() + index);
    CtrlReconstruct();
    isChange = true;
}

int QVLayout::GetCtrlCount()
{

    return m_QWNDs.size();
}
unsigned int QVLayout::GetCtrlWeight(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
    if (index < 0 || index >QDWORD(m_QWNDs.size()) - 1)return 0;

    return m_QWNDs[index].weight;
}
QWND* QVLayout::GetCtrl(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
    if (index < 0 || index >QDWORD(m_QWNDs.size()) - 1)return nullptr;

    return m_QWNDs[index].pWnd;
}
QDWORD QVLayout::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{

    switch (msgID) {
    case QM_PARENTCTRL_SIZE_CHANGE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;
        if (h == m_rect.Height)return 1;

        //���򲼾���ֻ��ע��ȣ��߶ȱ���
        GRect rect;
        rect.X = m_rect.X;
        rect.Y =0;
        rect.Height = h;
        rect.Width = m_rect.Width;
        w = m_rect.Width;
        {
            //std::unique_lock<std::shared_mutex> lk(m_Draw_lk);
            QLayout::Reconstruct(NULL, rect);
            //ֹͣ�ӿؼ�
            //h = m_rect.Height;
            //QDWORD lp = (QDWORD(w) << 32) | (QDWORD(h)&0xffffffff);
            std::shared_lock<std::shared_mutex> dlk(m_QWND_lk);
            unsigned int total = 0;
            int assigned = 0;//Ȩ��Ϊ0���ܿ��
            for (auto& it : m_QWNDs) {
                total += it.weight;
                if (0 == it.weight) {
                    assigned += it.pWnd->GetRect().Height;
                }
            }
            if (0 == total) {
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = 0;
                    rt.Y = start;
                    rt.Width = w;
                    it.pWnd->Reconstruct(m_dc, rt);
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                   
                    start += rt.Height;
                }

            }
            else {//��Ȩ�ص�
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = 0;
                    rt.Y = start;
                    rt.Width = w;
                    if (0 != it.weight) {
                        float ato = (float)it.weight / total;
                        int dta = m_rect.Height - assigned;
                        if (dta < 0)dta = m_rect.Height - 5;
                        rt.Height = ato * dta;
                    }
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    it.pWnd->Reconstruct(m_dc, rt);
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                  
                    start += rt.Height;

                }
            }
        }
        isChange = true;
        //Draw(m_sdc);
        return 1;

    }break;
    case QM_SETWINDOWS_SIZE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;


        //���򲼾���ֻ��ע��ȣ��߶ȱ���
        GRect rect;
        rect.X = m_rect.X;
        rect.Y = 0;
        rect.Height = h;
        rect.Width = w;
        {
            if (w != m_rect.Width || h != m_rect.Height) {
                QLayout::Reconstruct(NULL, rect);
            }

            std::shared_lock<std::shared_mutex> dlk(m_QWND_lk);
            unsigned int total = 0;
            int assigned = 0;//Ȩ��Ϊ0���ܿ��
            for (auto& it : m_QWNDs) {
                total += it.weight;
                if (0 == it.weight) {
                    assigned += it.pWnd->GetRect().Height;
                }
            }
            if (0 == total) {
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = 0;
                    rt.Y = start;
                    rt.Width = w;
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                    it.pWnd->Reconstruct(m_dc, rt);
                    start += rt.Height;
                }

            }
            else {//��Ȩ�ص�
                int start = 0;
                for (auto& it : m_QWNDs) {
                    GRect rt = it.pWnd->GetRect();
                    rt.X = 0;
                    rt.Y = start;
                    rt.Width = w;
                    if (0 != it.weight) {
                        float ato = (float)it.weight / total;
                        int dta = m_rect.Height - assigned;
                        if (dta < 0)dta = m_rect.Height - 5;
                        rt.Height = ato * dta;
                    }
                    QWNDAttribute qwa;
                    qwa.AData = it.pWnd->GetAttribute();
                    if (1 == qwa.Attribute.isLayout) {
                        QDWORD lp = 0;
                        lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                        it.pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                    }
                    it.pWnd->Reconstruct(m_dc, rt);
                    start += rt.Height;

                }
            }
        }
        isChange = true;

        return 1;


    }break;
    case QM_PLACE_CTRL: {
        QWND* p = (QWND*)LParam;
        GRect rect = p->GetRect();
        int i = -1; {
            std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

            for (; ++i < m_QWNDs.size();) {
                GRect rt = m_QWNDs[i].pWnd->GetRect();
                if (rect.Y < rt.Y)break;
            }
        }
        InsertCtrl(p, RParam, i);


    }break;
    case QM_REMOVE_CTRL: {
        QWND* p = (QWND*)LParam;
        std::unique_lock<std::shared_mutex> lk(m_QWND_lk);

        for (int i = -1; ++i < m_QWNDs.size();) {
            if (m_QWNDs[i].pWnd == p) {
                m_QWNDs.erase(m_QWNDs.begin() + i);
                return 2;
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
        std::shared_lock<std::shared_mutex> lk(m_QWND_lk);
    
            for (auto& it : m_QWNDs) {
                it.pWnd->Redraw();
              
            }

            
        }


    }break;
    case QM_POINT_IN_CTRL: {
        std::shared_lock<std::shared_mutex> lk(m_QWND_lk);

        for (auto& it : m_QWNDs) {
            if (it.pWnd->Contains(LParam, RParam)) {

                return it.pWnd->TransferMessage(QM_POINT_IN_CTRL, LParam - m_rect.X, RParam - m_rect.Y, 0);
            }
        }
    }
    }



    return QLayout::TransferMessage(msgID, LParam, RParam, AddParam);

}

#endif //  __QVLAYOUT__

extern "C"
{
    __declspec(dllexport) int QVLInsertCtrl(QVLayout* p, QWND* ctrl, unsigned int weight, int index)
    {

        return p->InsertCtrl(ctrl, weight, index);
    }
    __declspec(dllexport) int QVLRemoveCtrl(QVLayout* p, int index)
    {

        return p->RemoveCtrl(index);
    }
    __declspec(dllexport) int QVLGetCtrlCount(QVLayout* p)
    {

        return p->GetCtrlCount();
    }
    __declspec(dllexport) QWND* QVLGetCtrl(QHLayout* p, int index)
    {

        return p->GetCtrl(index);
    }
    __declspec(dllexport) int QVLGetCtrlWeight(QHLayout* p, int index)
    {

        return p->GetCtrlWeight(index);
    }
}

#ifdef  __QTLAYOUT__

QTLayout::QTLayout(HDC winddc, const GRect& rf) :QLayout(winddc, rf)
{
#ifdef _DEBUG
    Test();
#endif
}

void QTLayout::Redraw()
{
    if (pWnd) {
        pWnd->Redraw();
    }
    isChange = true;
}

void QTLayout::Refresh()
{
    isChange = true;
    if (pWnd) {
        pWnd->Refresh();
    }

}

int QTLayout::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
        auto now = std::chrono::high_resolution_clock::now();
        long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __ct).count();
        if (dta > 5000) {
            isChange = false;
        }
        std::shared_lock<std::shared_mutex> dlk(m_Draw_lk);
        flag = 1;

        //1.sdc�ж�Ӧ��λ�ÿ�������
        StretchBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height,
            sdc, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height, SRCCOPY);
        if (pWnd) {
            pWnd->Draw(m_dc);
        
        }





#ifndef _DEBUG
        if (nullptr== pWnd)
#endif
        {
            Gdiplus::Graphics graphics(m_dc);
            GPen pen(GColor(0xff, 0, 0xa0, 0x80));
            pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDotDot);
            GRect rect(0, 0, m_rect.Width, m_rect.Height);
            rect.Inflate(-1, -1);
            graphics.DrawRectangle(&pen, rect);

        }



    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}

void QTLayout::Reconstruct(HDC sdc, const GRect& rect)
{
    GRect rt(rect);
    rt.Y =0;
    rt.X = 0;
    GRect rt2 = rt;
    rt2.Inflate(-1, -1);
    if (pWnd) {
        pWnd->Reconstruct(sdc, rt2);
    }
    return QLayout::Reconstruct(sdc, rt);
}

void QTLayout::Reconstruct(HDC sdc)
{
    return QLayout::Reconstruct(sdc);
}

QTLayout::~QTLayout()
{

}

int QTLayout::MouseMove(int x, int y, WPARAM wParam)
{

    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->MouseMove(px, py, wParam);
    }
    
    return ret;
}

int QTLayout::LButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->LButtonDown(px, py, wParam);
    }

    return ret;
}

int QTLayout::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->LButtonDBLClick(px, py, wParam);
    }

    return ret;
}

int QTLayout::LButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->LButtonUp(px, py, wParam);
    }

    return ret;
}

int QTLayout::RButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->RButtonDown(px, py, wParam);
    }

    return ret;
}

int QTLayout::RButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->RButtonUp(px, py, wParam);
    }

    return ret;
}

int QTLayout::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->RButtonDBLClick(px, py, wParam);
    }

    return ret;
}

int QTLayout::MButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->MButtonDown(px, py, wParam);
    }

    return ret;
}

int QTLayout::MButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->MButtonUp(px, py, wParam);
    }

    return ret;
}

int QTLayout::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->MButtonDBLClick(px, py, wParam);
    }

    return ret;
}

int QTLayout::MouseWheel(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    if (pWnd) {
        ret = pWnd->MouseWheel(px, py, wParam);
    }

    return ret;
}
int QTLayout::MouseMoveOut()
{
    int ret = 0;
    if (pWnd) {
        ret = pWnd->MouseMoveOut();
    }

    return ret;
}
int QTLayout::KeyChar(WPARAM wParam, LPARAM lParam)
{
    if (pWnd) {
        pWnd->KeyChar(wParam, lParam);
    }

    return 0;
}
int QTLayout::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    if (pWnd) {
        pWnd->KeyIMEChar(wParam, lParam);
    }
    return 0;
}
int QTLayout::KeyDown(WPARAM wParam, LPARAM lParam)
{
    if (pWnd) {
        pWnd->KeyDown(wParam, lParam);
    }
    return 0;
}
int QTLayout::KeyUp(WPARAM wParam, LPARAM lParam)
{
    if (pWnd) {
        pWnd->KeyUp(wParam, lParam);
    }
    return 0;
}

void QTLayout::killFocus()
{
    if (pWnd) {
        pWnd->killFocus();
    }
}


void QTLayout::SetLocation(const GPoint& point)
{
    GPoint pt = point;
    pt.X = m_rect.X;
    QLayout::SetLocation(pt);
    isChange = true;
}
void QTLayout::SetSize(const GSize& size, HDC dc)
{
    GSize sz = size;
    sz.Width = m_rect.Width;
    QLayout::SetSize(sz, dc);
    isChange = true;
}


inline void QTLayout::CtrlReconstruct()
{
    GRect rect = m_rect;
    rect.Inflate(-1, -1);

    if (pWnd) {
        pWnd->Reconstruct(m_dc,rect);
    }
    
}

void QTLayout::Test()
{

}

int QTLayout::SetCtrl(QWND* ctrl)
{
    
    pWnd = ctrl;
    QTLayout* p = (QTLayout*)pWnd;
    p->m_Parent = this;
    p->m_HostHwnd = m_HostHwnd;
    CtrlReconstruct();


    isChange = true;

    return 1;
}
int QTLayout::RemoveCtrl()
{
    
    pWnd = nullptr;
    return 1;
}

QWND* QTLayout::GetCtrl()
{
    return pWnd;
}
QDWORD QTLayout::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{

    switch (msgID) {
    case QM_PARENTCTRL_SIZE_CHANGE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;
        if (h == m_rect.Height && w  == m_rect.Width)return 1;

     
        GRect rect;
        rect.X = 0;
        rect.Y = 0;
        rect.Height = h;
        rect.Width = w;
        {
            //std::unique_lock<std::shared_mutex> lk(m_Draw_lk);
            QLayout::Reconstruct(NULL, rect);
            //ֹͣ�ӿؼ�
            
            if (pWnd) {
                GRect rt = rect;
                rt.Inflate(-1, -1);

              
                QWNDAttribute qwa;
                qwa.AData = pWnd->GetAttribute();
                pWnd->Reconstruct(m_dc, rt);
                if (1 == qwa.Attribute.isLayout) {
                    QDWORD lp = 0;
                    lp = (QDWORD(rt.Width) << 32) | (QDWORD(rt.Height) & 0xffffffff);
                    pWnd->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0, 0);

                }
            }

              

            
            
        }
        isChange = true;
        //Draw(m_sdc);
        return 1;

    }break;
    case QM_SETWINDOWS_SIZE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;


        GRect rect;
        rect.X = 0;
        rect.Y = 0;
        rect.Height = h;
        rect.Width = w;
        
        if (w != m_rect.Width || h != m_rect.Height) {
            QLayout::Reconstruct(NULL, rect);
        }

           

        return 1;


    }break;
    case QM_PLACE_CTRL: {
        QWND* p = (QWND*)LParam;
        SetCtrl(p);
    }break;
    case QM_REMOVE_CTRL: {
        
        pWnd = nullptr;


        isChange = true;

    }return 1;
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            pWnd->Redraw();
        }


    }break;

    case QM_POINT_IN_CTRL: {

        if (!pWnd)return 0;

        if (pWnd->Contains(LParam, RParam)) {

            return pWnd->TransferMessage(QM_POINT_IN_CTRL, LParam - m_rect.X, RParam - m_rect.Y, 0);
        }
   
    }
    }



    return QLayout::TransferMessage(msgID, LParam, RParam, AddParam);

}

#endif //  __QTLAYOUT__

extern "C" 
{
    __declspec(dllexport)int QTLSetCtrl(QTLayout* p,QWND* ctrl)
    {
        return p->SetCtrl(ctrl);
    }
    __declspec(dllexport)int QTLRemoveCtrl(QTLayout* p)
    {
        return p->RemoveCtrl();
    }
    __declspec(dllexport)QWND* QTLGetCtrl(QTLayout* p)
    {
        return p->GetCtrl();
    }


}


#ifdef __QSTATICTEXT__



QStaticText::QStaticText(HDC winddc, const GRect& rf):QWND(winddc, rf)
{
    m_hCursorBuff = LoadCursor(NULL, IDC_ARROW);
    //QCreateDC(winddc);
    //m_bkBrush = new GSolidBrush(GColor(0xaf, 0xaa, 0xaa, 0xaa));
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
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
    // Set paragraph alignment to center
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���

    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    pTextFormat->SetTrimming(&trimming, nullptr);//��ֹ������ֿ�

    RECT rc(0, 0, rf.Width, rf.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���

    //����һ��ͼ��
    m_pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory));
}

void QStaticText::Reconstruct(HDC sdc, const GRect& rect)
{
    if (!rect.Width || !rect.Height)return;
    std::unique_lock<std::shared_mutex> lk(m_dlk);
    QWND::Reconstruct(sdc, rect);

    //ReconstructDC(m_pDC, m_dc, m_pHB, m_rect);

    //��Ӵ���
    RECT rc(0, 0, m_rect.Width, m_rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���



    
}

QDWORD QStaticText::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
    }break;
    
    }
    return QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}





void QStaticText::SetText(const wchar_t* t)
{
    m_text = t;

    isChange = true;
}

void QStaticText::SetFontName(const wchar_t* t)
{
    m_fontName = t;
    IDWriteTextFormat* pTFormat = NULL;
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
    IDWriteTextFormat* buff = pTextFormat;
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���
    pTextFormat = pTFormat;
    SafeRelease(&buff);
    isChange = true;

}

void QStaticText::SetFontSize(double size)
{
    if (size <= 0)return;
    m_fontSize = size;
    IDWriteTextFormat* pTFormat = NULL;
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
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//���������ĺͲ��ֿ������غϣ�
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//ָʾ���ʱ�����ͬһ���У���ʹ��������ֿ򡣣����ֵ��У�
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//���¾���
    IDWriteTextFormat* buff = pTextFormat;

    pTextFormat = pTFormat;
    SafeRelease(&buff);
    isChange = true;
}

void QStaticText::SetFontColor(unsigned int color)
{
    m_fontColor = color;
    isChange = true;
}

void QStaticText::SetBKColor(unsigned int color)
{
    m_bkColor = color;
    isChange = true;
}

const wchar_t* QStaticText::GetText()
{
    return m_text.c_str();
}


QStaticText::~QStaticText()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
    SafeRelease(&pLayer);
    //ReleaseDC();
}

int QStaticText::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
        std::shared_lock<std::shared_mutex> lk(m_dlk);
        flag = 1;
        //isChange = false;
        //1.sdc�ж�Ӧ��λ�ÿ�������
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //2.��������
        {




        }
        //3.���ƿؼ�
        {

            // Begin drawing
            m_pRenderTarget->BeginDraw();


            // Draw something
            ID2D1SolidColorBrush* pBrush = NULL;
            ID2D1SolidColorBrush* pfBrush = NULL;//������ɫ��ˢ
       
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_bkColor & 0xffffff, ((m_bkColor >> 24)&0xff)/255.0), &pBrush);
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_fontColor & 0xffffff, ((m_fontColor >> 24) & 0xff) / 255.0), &pfBrush);
            D2D1_RECT_F rect = D2D1::RectF(0, 0, m_rect.Width, m_rect.Height);
            D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
            m_pRenderTarget->FillRectangle(rect, pBrush);

            m_pRenderTarget->DrawText(
                m_text.c_str(),
                m_text.length(),
                pTextFormat,             // The text format
                rect,              // The layout rectangle
                pfBrush                   // The brush
            );


        
            pBrush->Release();
            pfBrush->Release();
            // End drawing
            m_pRenderTarget->EndDraw();


        }

        //Gdiplus::Graphics graphics(m_pDC);

        //GSolidBrush gs(GColor(0xff,0,0));
        //graphics.FillRectangle(&gs,0,0,100,100);


        //BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_pDC, 0, 0, SRCCOPY);
    }


    //****************************************
    QWND::Draw(sdc);
    return flag;
}

void QStaticText::Redraw()
{
    isChange = true;
}










#endif // __QSTATICTEXT__

extern "C"
{
    __declspec(dllexport) void QSTTSetText(QStaticText* p,const wchar_t* t)
    {
        return p->SetText(t);
    }
    __declspec(dllexport) void QSTTSetFontName(QStaticText* p, const wchar_t* t)
    {
        return p->SetFontName(t);
    }
    __declspec(dllexport) void QSTTSetFontSize(QStaticText* p, double size)
    {
        return p->SetFontSize(size);
    }
    __declspec(dllexport) void QSTTSetFontColor(QStaticText* p, unsigned int color)
    {
        return p->SetFontColor(color);
    }
    __declspec(dllexport) void QSTTSetBKColor(QStaticText* p, unsigned int color)
    {
        return p->SetBKColor(color);
    }
    __declspec(dllexport) const wchar_t* QSTTGetText(QStaticText* p)
    {
        return p->GetText();
    }


}










#ifdef __QCTRLTOOLBOX__





QCtrlToolbox::QCtrlToolbox(HDC winddc, const GRect& rf):QCtrlBaseD2D(winddc,rf)
{
    GRect rect = rf;
    rect.X = 0;
    rect.Y = 0;

    __qvl = new QVLayout(winddc, rect);
    printf("__qvl= %p\n", __qvl);
    QCtrlToolbox* bp = (QCtrlToolbox*)__qvl;
    bp->m_Parent = this;
    bp->m_HostHwnd = m_HostHwnd;

    m_pTable = new QTabs(winddc, rect);
    printf("m_pTable= %p\n", m_pTable);
    __qvl->InsertCtrl(m_pTable,1);
    m_pTable->AddTab(L"�ؼ�",L"./Tabico2.png");
    m_pTable->AddTab(L"����", L"./Tabico1.png");
    m_pTable->AddTab(L"�ļ�", L"./Tabico3.png");
    //bp = (QCtrlToolbox*)m_pTable;
    //bp->m_Parent = __qvl;
    //bp->m_HostHwnd = m_HostHwnd;

    m_ctrlqvl = new QVLayout(winddc, rect);
    printf("m_ctrlqvl= %p\n", m_ctrlqvl);
/*    bp = (QCtrlToolbox*)m_ctrlqvl;
    bp->m_Parent = m_pTable;
    bp->m_HostHwnd = m_HostHwnd*/;
    m_pTable->AttachCtrl(m_ctrlqvl,0);
    m_attrlqvl = new QVLayout(winddc, rect);
    printf("m_attrlqvl= %p\n", m_attrlqvl);
    //bp = (QCtrlToolbox*)m_attrlqvl;
    //bp->m_Parent = m_pTable;
    //bp->m_HostHwnd = m_HostHwnd;
    m_pTable->AttachCtrl(m_attrlqvl, 1);
    m_fileqvl = new QVLayout(winddc, rect);
    printf("m_fileqvl= %p\n", m_fileqvl);
    //bp = (QCtrlToolbox*)m_fileqvl;
    //bp->m_Parent = m_pTable;
    //bp->m_HostHwnd = m_HostHwnd;
    m_pTable->AttachCtrl(m_fileqvl, 2);

    //
    m_OpenFile = new QPButton(winddc,GRect(0,0,100,48));
    printf("m_OpenFile= %p\n", m_OpenFile);
    //bp = (QCtrlToolbox*)m_OpenFile;
    //bp->m_Parent = m_fileqvl;
    //bp->m_HostHwnd = m_HostHwnd;
    m_OpenFile->SetText(L"���ļ�");
    m_OpenFile->SetBKColor(0xc00000ee);
    m_OpenFile->SetFontSize(16);
    m_mangerCtrl.push_back(m_OpenFile);
    m_fileqvl->InsertCtrl(m_OpenFile,0);
    m_SeveFile = new QPButton(winddc, GRect(0, 0, 100, 48));
    printf("m_SeveFile= %p\n", m_SeveFile);
    //bp = (QCtrlToolbox*)m_SeveFile;
    //bp->m_Parent = m_fileqvl;
    //bp->m_HostHwnd = m_HostHwnd;
    m_fileqvl->InsertCtrl(m_SeveFile, 0);
    m_SeveFile->SetText(L"�����ļ�");
    m_SeveFile->SetFontSize(16);
    m_SeveFile->SetBKColor(0xc00000ee);
    m_mangerCtrl.push_back(m_SeveFile);


}

QCtrlToolbox::~QCtrlToolbox()
{
   if (__qvl){
        delete __qvl;
   }
   if (m_pTable) {
       delete m_pTable;
   }
   if (m_ctrlqvl) {
       delete m_ctrlqvl;
   }
   if (m_attrlqvl) {
       delete m_attrlqvl;
   }
   if (m_fileqvl) {
       delete m_fileqvl;
   }

   for (auto it : m_mangerCtrl) {
       delete it;
   
   }
}

QDWORD QCtrlToolbox::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_PARENTCTRL_SIZE_CHANGE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;
        ParentCtrlSizeChangeEvent(w, h);
        return 1;
    }
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            if (__qvl)__qvl->Redraw();
            if (m_ctrlqvl)m_ctrlqvl->Redraw();
            if (m_attrlqvl)m_attrlqvl->Redraw();
            if (m_fileqvl)m_fileqvl->Redraw();
            if (m_pTable)m_pTable->Redraw();
            if (__qvl)__qvl->Redraw();

            
            for (auto& it : m_mangerCtrl){
                
                it->Redraw();
            }
        }


    }break;
    
    
    }



    return QCtrlBaseD2D::TransferMessage(msgID, LParam, RParam, AddParam);
}

int QCtrlToolbox::Draw(HDC sdc)
{
    bool flag = 0;

    //��ӻ��ƴ���
    if (isChange) {
       
        std::shared_lock<std::shared_mutex> lk(m_drawLK);
        flag = 1;
        isChange = false;
        //1.sdc�ж�Ӧ��λ�ÿ�������
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //2.��������
        {




        }
        //3.���ƿؼ�
        {

            // Begin drawing
            m_pRenderTarget->BeginDraw();


            // Draw something
            ID2D1SolidColorBrush* pBrush = NULL;
        

            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_bkColor & 0xffffff, m_bkColor >> 24), &pBrush);

            D2D1_RECT_F rect = D2D1::RectF(0, 0, m_rect.Width, m_rect.Height);
            D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
            m_pRenderTarget->DrawRectangle(rect, pBrush);





  








            
            // End drawing
            m_pRenderTarget->EndDraw();
            pBrush->Release();
            {//�����ӿؼ�
                __qvl->Draw(m_dc);






            }

        }

        //Gdiplus::Graphics graphics(m_pDC);

        //GSolidBrush gs(GColor(0xff,0,0));
        //graphics.FillRectangle(&gs,0,0,100,100);


        //BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_pDC, 0, 0, SRCCOPY);
    }


    //****************************************
    QCtrlBaseD2D::Draw(sdc);
    return flag;
}

void QCtrlToolbox::Redraw()
{
    isChange = true;
}


// ���ؼ���С���͸ı��¼�
void QCtrlToolbox::ParentCtrlSizeChangeEvent(int w, int h)
{
    // TODO: �ڴ˴����ʵ�ִ���.

    GRect rect;
    rect.X = w- m_rect.Width;
    rect.Y = 0;
    rect.Height = h;
    rect.Width = m_rect.Width;
    {
        std::unique_lock<std::shared_mutex> lk(m_drawLK);
        QCtrlBaseD2D::Reconstruct(NULL, rect);
        RECT rc(0, 0, rect.Width, rect.Height);
        m_pRenderTarget->BindDC(m_dc, &rc);//��DC���


        //

        if (__qvl) {
            rect.X = 0;
            __qvl->Reconstruct(m_dc, rect);
            QDWORD lp=0;
            lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(h) & 0xffffffff);
            __qvl->TransferMessage(QM_SETWINDOWS_SIZE, lp, 0,0);
            __qvl->Redraw();
        
        }


    }
    isChange = true;


}



int QCtrlToolbox::InsertCtrl(QWND* ctrl, unsigned int weight, int index)
{
    int ret = m_ctrlqvl->InsertCtrl(ctrl, weight, index);
    isChange = true;
    return ret;
}

int QCtrlToolbox::RemoveCtrl(int index)
{
    int ret = m_ctrlqvl->RemoveCtrl(index);
    isChange = true;
    return ret;
}

int QCtrlToolbox::GetCtrlCount()
{


    return __qvl->GetCtrlCount();
}



void QCtrlToolbox::Reconstruct(HDC sdc, const GRect& rect)
{

    QCtrlBaseD2D::Reconstruct(sdc,rect);

    RECT rc(0, 0, rect.Width, rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//��DC���
    isChange = true;
}

int QCtrlToolbox::MouseMove(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;


    int ret = __qvl->MouseMove(x, y, wParam);
    isChange = true;
    Redraw();
    return ret;
}

int QCtrlToolbox::LButtonDown(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->LButtonDown(x, y, wParam);
    Redraw();
    isChange = true;
    return ret;
}

int QCtrlToolbox::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->LButtonDBLClick(x, y, wParam);
    Redraw();
    isChange = true;
    return ret;
}

int QCtrlToolbox::LButtonUp(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->LButtonUp(x, y, wParam);
    Redraw();
    isChange = true;
    return ret;
}

int QCtrlToolbox::RButtonDown(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->RButtonDown(x, y, wParam);
    Redraw();

    return ret;
}

int QCtrlToolbox::RButtonUp(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->RButtonUp(x, y, wParam);
    Redraw();
    return ret;
}

int QCtrlToolbox::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->RButtonDBLClick(x, y, wParam);
    Redraw();
    return ret;
}

int QCtrlToolbox::MButtonDown(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->MButtonDown(x, y, wParam);
    Redraw();
    return ret;
}

int QCtrlToolbox::MButtonUp(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->MButtonUp(x, y, wParam);
    Redraw();
    return ret;
}

int QCtrlToolbox::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->MButtonDBLClick(x, y, wParam);
    Redraw();
    return ret;
}

int QCtrlToolbox::MouseWheel(int x, int y, WPARAM wParam)
{
    x = x - m_rect.X;
    y = y - m_rect.Y;
    int ret = __qvl->MouseWheel(x, y, wParam);
    Redraw();
    return ret;
}
int QCtrlToolbox::MouseMoveOut()
{

    int ret = __qvl->MouseMoveOut();
    Redraw();
    return ret;

}

int QCtrlToolbox::KeyChar(WPARAM wParam, LPARAM lParam)
{
    Redraw();
    return __qvl->KeyChar(wParam, lParam);
}

int QCtrlToolbox::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    Redraw();
    return __qvl->KeyIMEChar(wParam, lParam);
}

int QCtrlToolbox::KeyDown(WPARAM wParam, LPARAM lParam)
{
    Redraw();
    return __qvl->KeyDown(wParam, lParam);
}

int QCtrlToolbox::KeyUp(WPARAM wParam, LPARAM lParam)
{
    Redraw();
    return __qvl->KeyUp(wParam, lParam);
}

int QCtrlToolbox::InsertAttributeCtrl(QWND* w, int index)
{
    int ret = m_attrlqvl->InsertCtrl(w, 0, index);
    ((QCtrlToolbox*)w)->m_Parent = m_attrlqvl;
    ((QCtrlToolbox*)w)->m_HostHwnd = m_HostHwnd;
    isChange = true;
    m_attrlqvl->Redraw();
    return ret;
}

int QCtrlToolbox::RemoveAttributeCtrl(int index)
{
    int ret = m_attrlqvl->RemoveCtrl(index);
    isChange = true;
    //UpdateWND();
    m_attrlqvl->Redraw();
    return ret;
}

void QCtrlToolbox::SetFileButtonEvent(QFBUTTONCLICK en, int index)
{
    switch (index) {
    case 0:
        m_OpenFile->SetClickEvent(en);
        break;
    case 1:
        m_SeveFile->SetClickEvent(en);
        break;
    
    }
}

void QCtrlToolbox::ClearAttributeCtrl()
{
    int count = m_attrlqvl->GetCtrlCount();
    for (int i = -1; ++i < count;) {
        m_attrlqvl->RemoveCtrl(0);
    }
    isChange = true;
    m_pTable->Redraw();
    //UpdateWND();
}






#endif // __QCTRLTOOLBOX__

extern "C"
{
    __declspec(dllexport) int QCTBInsertCtrl(QCtrlToolbox* p, QWND* ctrl, unsigned int weight, int index)
    {

        return p->InsertCtrl(ctrl, weight, index);
    }
    __declspec(dllexport) int QCTBRemoveCtrl(QCtrlToolbox* p, int index)
    {

        return p->RemoveCtrl(index);
    }
    __declspec(dllexport) int QCTBGetCtrlCount(QCtrlToolbox* p)
    {

        return p->GetCtrlCount();
    }

    __declspec(dllexport) int QCTBInsertAttributeCtrl(QCtrlToolbox* p, QWND* w, int index)
    {
        return p->InsertAttributeCtrl(w, index);
    }
    __declspec(dllexport) int QCTBRemoveAttributeCtrl(QCtrlToolbox* p, int index)
    {
        return p->RemoveAttributeCtrl(index);

    }
    __declspec(dllexport) void QCTBClearAttributeCtrl(QCtrlToolbox* p)
    {
        return p->ClearAttributeCtrl();
    }
    __declspec(dllexport) void QCTBSetFileButtonEvent(QCtrlToolbox* p, QFBUTTONCLICK en, int index )
    {
        return p->SetFileButtonEvent(en, index);
    }



}


#ifdef __QEDITORWIN__

HCURSOR QEditorWin::m_hCursorWE;
HCURSOR QEditorWin::m_hCursorNS;
HCURSOR QEditorWin::m_hCursorARROW;
HCURSOR QEditorWin::m_hCursorSIZEALL;


int QEditorWin::Draw(HDC sdc)
{
    std::shared_lock<std::shared_mutex> LK(m_drawLK);
    std::shared_lock<std::shared_mutex> lock(m_ctrlLK);
    BitBlt(m_dc, 0,0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
    //1.���ƴ���

    {
        // Begin drawing
        m_pRenderTarget->BeginDraw();
        float angle = m_titleW * 0.4;

        // Draw something
        ID2D1SolidColorBrush* pBrush = NULL;
        D2D1_RECT_F rect = D2D1::RectF(0, 0, m_rect.Width, m_rect.Height);

       
        
        // ����Բ�Ǿ���
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            rect, angle, angle);

 
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_bkColor & 0xffffff, double(m_bkColor >> 24)/0xff), &pBrush);

        m_pRenderTarget->FillRoundedRectangle(roundedRect, pBrush);
        
        pBrush->Release();
        //���ƴ��ڱ�����
        roundedRect.rect= D2D1::RectF(0, 0, m_rect.Width, m_titleW);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_TitleBKColor & 0xffffff, double(m_TitleBKColor >> 24)/0xff), &pBrush);

        m_pRenderTarget->FillRoundedRectangle(roundedRect, pBrush);
        rect = D2D1::RectF(0, m_titleW/2, m_rect.Width, m_titleW);
        m_pRenderTarget->FillRectangle(rect, pBrush);
        pBrush->Release();

        //��������Բ��
        {
            //��ʼ������
            float startX = angle * 2;
            float startY = m_titleW*0.5;
            //���ư뾶
            float radius = angle*0.6;

            // Define gradient stops
            D2D1_GRADIENT_STOP gradientStops[2] = {
                { 0.0f, D2D1::ColorF(0x00ff00,1.0) },   // Start color
                { 1.0f, D2D1::ColorF(0x00ff00,0.1) }   // End color
            };

            // Create gradient stop collection
            ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
            HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                ARRAYSIZE(gradientStops),
                &pGradientStopCollection
            );

            if (SUCCEEDED(hr)) {
                // Create a linear gradient brush
  

                D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientProperties = D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(startX, startY), // Start point
                    D2D1::Point2F(0, 0),// End point
                    radius,                           // Radius X
                    radius                            // Radius Y
                );
                ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
               m_pRenderTarget->CreateRadialGradientBrush(
                    radialGradientProperties,
                    pGradientStopCollection,
                    &pRadialGradientBrush
                );

                
                D2D1_ELLIPSE elle(
                    D2D1::Point2F(startX, startY), // Center of the ellipse
                    radius, // Horizontal radius
                    radius  // Vertical radius
                );;
                //��ɫ��
                m_pRenderTarget->FillEllipse(elle, pRadialGradientBrush);
                pRadialGradientBrush->Release();
                pGradientStopCollection->Release();


                //��ɫ��
                startX += radius * 3;
                gradientStops[0].color =  D2D1::ColorF(0xfbfb1c,1.0);
                gradientStops[0].position = 0.0f;
                gradientStops[1].color = D2D1::ColorF(0xfbfb1c,0.1);
                gradientStops[1].position = 1.0f;
                
                m_pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    ARRAYSIZE(gradientStops),
                    &pGradientStopCollection
                );
                radialGradientProperties.center = D2D1::Point2F(startX, startY);
                m_pRenderTarget->CreateRadialGradientBrush(
                    radialGradientProperties,
                    pGradientStopCollection,
                    &pRadialGradientBrush
                );


                elle.point = D2D1::Point2F(startX, startY);
                m_pRenderTarget->FillEllipse(elle, pRadialGradientBrush);
                pRadialGradientBrush->Release();
                pGradientStopCollection->Release();

                //��ɫ��
                startX += radius * 3;
                gradientStops[0].color = D2D1::ColorF(0xf9320f, 1.0);
                gradientStops[0].position = 0.0f;
                gradientStops[1].color = D2D1::ColorF(0xf9320f, 0.1);
                gradientStops[1].position = 1.0f;

                m_pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    ARRAYSIZE(gradientStops),
                    &pGradientStopCollection
                );
                radialGradientProperties.center = D2D1::Point2F(startX, startY);
                m_pRenderTarget->CreateRadialGradientBrush(
                    radialGradientProperties,
                    pGradientStopCollection,
                    &pRadialGradientBrush
                );


                elle.point = D2D1::Point2F(startX, startY);
                m_pRenderTarget->FillEllipse(elle, pRadialGradientBrush);
                pRadialGradientBrush->Release();
                pGradientStopCollection->Release();


            }

        }

        if (5 == __DownData.flag) {
            D2D1_RECT_F rect = D2D1::RectF((std::min)(m_MouseStartX, m_MouseCurrentX), (std::min)(m_MouseStartY, m_MouseCurrentY), 
                (std::max)(m_MouseStartX, m_MouseCurrentX), (std::max)(m_MouseStartY, m_MouseCurrentY));
                //D2D1_RECT_F rect(10,10,50,50);
            ID2D1StrokeStyle* pStrokeStyle = nullptr;
            D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_FLAT,
                D2D1_LINE_JOIN_MITER,
                1.0f,
                D2D1_DASH_STYLE_DASH,
                0.0f
            );

            // ����������ʽ
            HRESULT hr = m_pD2DFactory->CreateStrokeStyle(
                strokeStyleProperties,
                nullptr,
                0,
                &pStrokeStyle
            );
            // ��������
            ID2D1SolidColorBrush* pBrush = nullptr;
            m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &pBrush
            );
            m_pRenderTarget->DrawRectangle(rect, pBrush, 1.0,pStrokeStyle);
            pBrush->Release();
            pStrokeStyle->Release();

        
        }


        // End drawing
        m_pRenderTarget->EndDraw();
    
    
    
    }


    //2.���ƴ����ڿؼ�
    {
        StretchBlt(m_clientDC, 0, 0, m_rect.Width, m_rect.Height - m_titleW,
        m_dc, 0, m_titleW, m_rect.Width, m_rect.Height- m_titleW, SRCCOPY);
        Gdiplus::Graphics graphics(m_clientDC);

        Gdiplus::Region region;
        {
            
            for (auto& it : m_QWNDs) {
                if (!m_sizeChaneg) {

                    if (it->Draw(m_clientDC)) {

                        //�пؼ�ˢ��
                        region.Union(it->GetRect());

                    }
                    else {

                        //���ûˢ�£�������ĸ������Ƿ���ˢ�¾�����
                        GRect rect = it->GetRect();
                        GPoint pt[4] = { GPoint(rect.X,rect.Y),GPoint(rect.X + rect.Width,rect.Y)
                        ,GPoint(rect.X + rect.Width,rect.Y + rect.Height),GPoint(rect.X,rect.Y + rect.Height) };
                        for (int i = -1; ++i < 4;) {
                            if (region.IsVisible(pt[i], &graphics)) {
                                //����ص�ҲҪ����
                                it->Refresh();

                                //���������������
                                region.Union(it->GetRect());
                            }
                        }

                    }
                }
                else {
                    //���ڴ�С�����仯��ȫ���ؼ��ػ�
                    it->Refresh();
                    m_sizeChaneg = false;
                }
            }
        }
        StretchBlt(m_dc, 0, m_titleW, m_rect.Width, m_rect.Height - m_titleW,
            m_clientDC, 0,0 , m_rect.Width, m_rect.Height - m_titleW, SRCCOPY);
    }

 

    QCtrlBaseD2D::Draw(sdc);

    return 1;
}

QEditorWin::QEditorWin(HDC winddc, const GRect& rf):QCtrlBaseD2D(winddc, rf)
{
    m_rect.X = 5;
    m_rect.Y = 5;
    if (!m_hCursorWE)
        m_hCursorWE = LoadCursor(NULL, IDC_SIZEWE);
    if (!m_hCursorNS)
        m_hCursorNS = LoadCursor(NULL, IDC_SIZENS);
    if (!m_hCursorARROW)
        m_hCursorARROW = LoadCursor(NULL, IDC_ARROW);
    if (!m_hCursorSIZEALL)
        m_hCursorSIZEALL = LoadCursor(NULL, IDC_SIZEALL);
    QCreateDC(winddc);
}


inline void QEditorWin::QCreateDC(HDC winddc)
{
    m_clientDC = CreateCompatibleDC(winddc);
    m_clientHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_clientDC, m_clientHB);
}

inline void QEditorWin::ReleaseDC()
{
    DeleteObject(m_clientHB);
    m_clientHB = NULL;
    DeleteDC(m_clientDC);
    m_clientDC = NULL;
}
void QEditorWin::Redraw()
{
    std::shared_lock<std::shared_mutex>lk(m_ctrlLK);
    for (auto& it : m_QWNDs) {
        it->Redraw();

    }
    isChange = true;

}
void QEditorWin::Refresh()
{
    std::shared_lock<std::shared_mutex>lk(m_ctrlLK);
    for (auto& it : m_QWNDs) {
        it->Refresh();

    }
    isChange = true;
}
QEditorWin::~QEditorWin()
{
    ReleaseDC();
}

void QEditorWin::Reconstruct(HDC sdc, const GRect& rect)
{
    GRect rt(rect);
    rt.X = 5;
    rt.Y = 5;
    QCtrlBaseD2D::Reconstruct(sdc, rt);
    ReconstructDC(m_clientDC, m_dc, m_clientHB, m_rect);
    isChange = true;
    UpdateWND();

}

int QEditorWin::MouseMove(int x, int y, WPARAM wParam)
{

    m_hCursor = m_hCursorARROW;


    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    ret = MouseMessage(WM_MOUSEMOVE, wParam, px, py);

    GRect rect = m_rect;

    if (!m_selectCtrl) {

        if (isDown) {
            switch (__DownData.flag)
            {
            case 1: {
                float _rt = __DownData.dta + x;

                if (m_rect.X < _rt) {
                    std::unique_lock<std::shared_mutex>lk(m_drawLK);
                    rect.Width = _rt - rect.X;

                    Reconstruct(NULL, rect);//���´���
                    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height-m_titleW) & 0xffffffff);
                    std::shared_lock<std::shared_mutex> clk(m_ctrlLK);
                    for (auto& it : m_QWNDs) {
                        it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
                    }
                }

            }
                  break;
            case 2: {

                float _bm = __DownData.dta + y;

                if (m_rect.Y < _bm) {
                    std::unique_lock<std::shared_mutex>lk(m_drawLK);
                    rect.Height = _bm - rect.Y;
                    Reconstruct(NULL, rect);//���´���
                    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height - m_titleW) & 0xffffffff);
                    std::shared_lock<std::shared_mutex> clk(m_ctrlLK);
                    for (auto& it : m_QWNDs) {
                        it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
                    }
                }
            }
                  break;
            case 5: {
                m_MouseCurrentX = px;
                m_MouseCurrentY = py;
                
            }
                  break;
            }

            isChange = true;

        }

        if (isSelect) {
            if (m_rect.Contains(x, y)) {
                if (abs(m_rect.GetRight() - x) < 5) {
                    m_hCursor = m_hCursorWE;
                }
                else if (abs(m_rect.X - x) < 5) {
                    m_hCursor = m_hCursorWE;

                }
                else if (abs(m_rect.GetBottom() - y) < 5) {
                    m_hCursor = m_hCursorNS;
                }
                else if (abs(m_rect.Y - y) < 5) {
                    m_hCursor = m_hCursorNS;

                }


            }
        }


    }


    return ret;
}

int QEditorWin::LButtonDown(int x, int y, WPARAM wParam)
{



    int ret = 0;
    if (m_rect.Contains(x, y)) {

       
        int px = x - m_rect.X;
        int py = y - m_rect.Y;
        m_selectCtrl = nullptr;
        m_selectCtrl_h = nullptr;
        ret = MouseMessage(WM_LBUTTONDOWN, wParam, px, py);
        if (!m_selectCtrl) {
            m_MouseStartX = px;
            m_MouseStartY = py;
            m_MouseCurrentX = px;
            m_MouseCurrentY = py;
            isSelect = true;
            if (isSelect) {
                isDown = true;
                if (abs(m_rect.GetRight() - x) < 5) {
                    m_hCursor = m_hCursorWE;
                    __DownData.flag = 1;
                    __DownData.dta = m_rect.GetRight() - x;
                }
                else if (abs(m_rect.GetBottom() - y) < 5) {
                    m_hCursor = m_hCursorNS;
                    __DownData.flag = 2;
                    __DownData.dta = m_rect.GetBottom() - y;

                }
                else {
                    __DownData.flag = 5;
                }
            }
            else {
                
                isSelect = true;
            }
        }
        ret |= 0x02;
        if (py > m_titleW && __qLBDown)__qLBDown((int)this,px,py- m_titleW);
        if (py > m_titleW && __qSelectCtrl) {
            int addr = (int)m_selectCtrl_h;
            if (!addr)addr = (int)this;
            __qSelectCtrl(addr);
        }
    }
   

    return ret;
}



int QEditorWin::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_LBUTTONDBLCLK, wParam, px, py);
    if (!m_selectCtrl) {
        
        
    
    
    
    
    }



    return ret;
}

int QEditorWin::LButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_LBUTTONUP, wParam, px, py);
    if (5 == __DownData.flag) {
        if (__qPCtrl)__qPCtrl((int)this, (std::min)(m_MouseStartX, m_MouseCurrentX), (std::min)(m_MouseStartY, m_MouseCurrentY)-m_titleW,
            (std::max)(m_MouseStartX, m_MouseCurrentX), (std::max)(m_MouseStartY, m_MouseCurrentY) - m_titleW);
    
    }
    
    isDown = false;
    __DownData.flag = -1;
 
    return ret;
}

int QEditorWin::RButtonDown(int x, int y, WPARAM wParam)
{
    m_selectCtrl = nullptr;
    m_selectCtrl_h = nullptr;
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_RBUTTONDOWN, wParam, px, py);
    if (!FocusCtrl) {






    }



    return ret;
}

int QEditorWin::RButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_RBUTTONUP, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}

int QEditorWin::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_RBUTTONDBLCLK, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}

int QEditorWin::MButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_MBUTTONDOWN, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}

int QEditorWin::MButtonUp(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_MBUTTONUP, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}

int QEditorWin::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_MBUTTONDBLCLK, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}

int QEditorWin::KeyUp(WPARAM wParam, LPARAM lParam)
{
    if (VK_DELETE == wParam) {
        if (__qDeleteCtrl)__qDeleteCtrl(int(m_selectCtrl));
    
    }
    return 0;
}

int QEditorWin::MouseWheel(int x, int y, WPARAM wParam)
{
    int ret = 0;
    int px = x - m_rect.X;
    int py = y - m_rect.Y;
    ret = MouseMessage(WM_MOUSEWHEEL, wParam, px, py);
    if (!m_selectCtrl) {






    }



    return ret;
}



int QEditorWin::MouseMoveOut()
{
    return 0;
}
QDWORD QEditorWin::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{

    switch (msgID) {

    case QM_SETWINDOWS_SIZE: {
        int w = LParam >> 32;
        int h = LParam & 0xffffffff;


        //���򲼾���ֻ��ע��ȣ��߶ȱ���
        GRect rect;
        rect.X = m_rect.X;
        rect.Y = 0;
        rect.Height = h;
        rect.Width = w;
        {
            if (w != m_rect.Width || h != m_rect.Height) {
                QEditorWin::Reconstruct(NULL, rect);
            }

            
            
        }
        isChange = true;

        return 1;


    }break;
    case QM_QHOST_LDOWN: {
        m_selectCtrl = (QWND*)RParam;
        m_selectCtrl_h = (QWND*)LParam;

    }break;
    case QM_QHOST_RDOWN: {
        m_selectCtrl = (QWND*)LParam;
        int x = RParam >> 32;
        int y = RParam & 0xffffffff;
        int r = AddParam >> 32;
        int b = AddParam & 0xffffffff;
        if (__qPCtrl)__qPCtrl(LParam, x, y,r, b);

    }break;
    case QM_PLACE_CTRL: {
        AttachCtrl((QWND*)LParam);

    }break;
    case QM_GETSUBCTRL: {

         /**ȡ�ؼ����ӿؼ�
        * ============================================================================================================
        * ���أ����򷵻��ӿؼ�����С��1��Ϊ���ӿؼ�������ָ�뼴QWND**
        * ����1���������2���ԣ��ǿ������ڷ��ؿؼ�ָ��
        * ����2��Ҫ��ȡ�����ӿؼ�������0~n
        * ����3������
        * ============================================================================================================
        */
        std::shared_lock<std::shared_mutex>lk(m_ctrlLK);
        if (LParam) {
            int index = RParam&0x7fffffff;
            QWND** p = (QWND**)LParam;
            if (index > -1 && index < (long long)(m_QWNDs.size())) {
                QHostCtrl* pHC =dynamic_cast<QHostCtrl*>(m_QWNDs[index]);
                if (pHC) {
                    *p = pHC->GetParasiticCtrl();
                }
                QWND** hp = (QWND**)AddParam;
                if (AddParam) {
                    *hp = m_QWNDs[index];
                }
            }
        }

        return m_QWNDs.size();

    }break;
    case QM_REMOVE_CTRL: {
        QWND* p = (QWND*)LParam;
        std::unique_lock<std::shared_mutex> lk(m_ctrlLK);
        
        for (int i = -1; ++i < m_QWNDs.size();) {
            if (m_QWNDs[i] == p) {
               m_QWNDs.erase(m_QWNDs.begin() + i);
               if (m_selectCtrl == p) {
                   m_selectCtrl = nullptr;

               }
               if (m_selectCtrl_h == p) {
                   m_selectCtrl_h = nullptr;
               }
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
      

        isChange = true;

    }return 1;
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            std::shared_lock<std::shared_mutex> lk(m_ctrlLK);
           
            for (auto& it :m_QWNDs) {
                it->Redraw();
                it->TransferMessage(msgID, LParam, RParam, AddParam);
            }

           
        }


    }break;
    }



    return QCtrlBaseD2D::TransferMessage(msgID, LParam, RParam, AddParam);

}

inline int QEditorWin::MouseMessage(UINT uMsg, WPARAM wParam, int x, int y)
{
    int ret = 0;
    POINT pt;
    pt.x = x;
    pt.y = y-m_titleW;

    {
        bool isAtCtrl = false;
        HCURSOR hCursor = m_hCursorARROW;
        std::shared_lock<std::shared_mutex> lk(m_ctrlLK);
      
        for (auto pit = m_QWNDs.rbegin(); pit != m_QWNDs.rend(); ++pit) {
            auto& it = *pit;
            if (mouseCapture && it == mouseCapture) {
                ret = MoseMsg(mouseCapture, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ

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
                        ret = MoseMsg(mouseFocusCtrl, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ
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

        //���ö�Ӧ�ؼ������
        m_hCursor = hCursor;
        /*      SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
              ::SetCursor(hCursor);*/
        if (!isAtCtrl) {
            //����˲��ǿؼ���ԭ����ؼ�ʧȥ����
            //֪ͨԭ����ؼ�ʧȥ����

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

inline int QEditorWin::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
{
    int ret = 0;
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        //mouseSeleFocusCtrl = qwnd;
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
        if (FocusCtrl && (FocusCtrl != qwnd)) {
            FocusCtrl->killFocus();

            FocusCtrl = NULL;


        }
        if (mouseSeleFocusCtrl) {
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
        //��������µĿؼ���������ֹͣԭ����ʧȥ����
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

int QEditorWin::SetLButtonEvent(QEDITORWIN_LBDOWN ent)
{
    QEDITORWIN_LBDOWN buf = __qLBDown;
    __qLBDown = ent;
    return (int)buf;
}



int QEditorWin::SetPlaceCtrlEvent(QEDITORWIN_PLACE_CTRL ent)
{
    QEDITORWIN_PLACE_CTRL buf = __qPCtrl;
    __qPCtrl = ent;
    return (int)buf;
}

int QEditorWin::SetDeleteCtrlEvent(QEDITORWIN_DELETE_CTRL ent)
{
    QEDITORWIN_DELETE_CTRL buf = __qDeleteCtrl;
    __qDeleteCtrl = ent;
    return (int)buf;
}

int QEditorWin::SetSelectCtrlEvent(QEDITORWIN_SELECT_CTRL ent)
{
    QEDITORWIN_SELECT_CTRL buf = __qSelectCtrl;
    __qSelectCtrl = ent;
    return (int)buf;
}

void QEditorWin::AttachCtrl(QWND* w)
{
    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height - m_titleW) & 0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С


    std::unique_lock<std::shared_mutex> lock(m_ctrlLK);
    ((QEditorWin*)w)->m_Parent = (QWND*)this;
    ((QEditorWin*)w)->m_HostHwnd = m_HostHwnd;

 
    m_QWNDs.push_back(w);
}

void QEditorWin::RemoveCtrl(QWND* w)
{
    std::unique_lock<std::shared_mutex> lock(m_ctrlLK);
    auto it = std::find(m_QWNDs.begin(), m_QWNDs.end(), w);
    if (it != m_QWNDs.end()) {
        m_QWNDs.erase(it);

        if (m_selectCtrl == *it) {
            m_selectCtrl = nullptr;
        
        }
        if (m_selectCtrl_h == *it) {
            m_selectCtrl_h = nullptr;
        }
        if (FocusCtrl == *it) {
            FocusCtrl = nullptr;
        }
        if (mouseFocusCtrl == *it) {
            mouseFocusCtrl = nullptr;
        }
        if (mouseSeleFocusCtrl == *it) {
            mouseSeleFocusCtrl = nullptr;
        }
        if (mouseCapture == *it) {
            mouseCapture = nullptr;
        }
  
    }
}








#endif // __QEDITORWIN__

extern "C" {

    __declspec(dllexport) int QEWSetLButtonEvent(QEditorWin* p, QEDITORWIN_LBDOWN ent)
    {
        return p->SetLButtonEvent(ent);
    
    }
    __declspec(dllexport) void QEWAttachCtrl(QEditorWin* p, QWND* w)       //����һ���ؼ�
    {
        return p->AttachCtrl(w);
    }
    __declspec(dllexport) void QEWRemoveCtrl(QEditorWin* p, QWND* w)       //�Ƴ�һ���ؼ�
    {
        return p->RemoveCtrl(w);
    }
    __declspec(dllexport) int QEWSetPlaceCtrlEvent(QEditorWin* p, QEDITORWIN_PLACE_CTRL ent)
    {
    
        return p->SetPlaceCtrlEvent(ent);
    }
    __declspec(dllexport) int QEWSetDeleteCtrlEvent(QEditorWin* p, QEDITORWIN_DELETE_CTRL ent)
    {
    
        return p->SetDeleteCtrlEvent(ent);
    }
    __declspec(dllexport) int QEWSetSelectCtrlEvent(QEditorWin* p, QEDITORWIN_SELECT_CTRL ent)
    {
        return p->SetSelectCtrlEvent(ent);
    }
}



#ifdef __QSWITCH__





QSwitch::QSwitch(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
}

QSwitch::~QSwitch()
{
}
int QSwitch::Draw(HDC sdc)
{
  
    int ret = 0;
    if (isChange) {
        isChange = false;
        ret = 1;
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //1.���ƴ���
        ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
        ID2D1Factory* pFactory = m_pD2DFactory;
        RECT rc(0, 0, m_rect.Width, m_rect.Height);
        pRenderTarget->BindDC(m_dc, &rc);//��DC���
     
        {
       
    

            int minV = (std::min)(m_rect.Height, m_rect.Width);

            
            // �����Բ�İ뾶�;��εĴ�С
            FLOAT radius = minV/2;
   

            float rx = radius;//Բ��x���
            float ry = radius;//Բ��y���
            unsigned int color = 0xa0a0a0;
            if (1 == __state) {
                if (m_rect.Width > m_rect.Height) {
                    rx = m_rect.Width - radius;
                }
                else {
                    ry = m_rect.Height - radius;
                }
                color = 0x00e000;

            }


    
            auto createPG_H = [&pFactory](ID2D1PathGeometry**pg, ID2D1GeometrySink**psk,const GRect&rect) {
                FLOAT radius = rect.Height / 2;
                FLOAT width = rect.Width;
                FLOAT height = rect.Height;
                // ����·������


                // ��������ͼ��
                pFactory->CreatePathGeometry(pg);

                // ��������ͼ�ε�����
                (*pg)->Open(psk);

                // ��������Բ
                (*psk)->BeginFigure(D2D1::Point2F(radius+ rect.X, rect.Y+ height), D2D1_FIGURE_BEGIN_FILLED);
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(radius + rect.X, rect.Y), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ���ƾ���
                (*psk)->AddLine(D2D1::Point2F(width - radius + rect.X, rect.Y)); // �������Ͻ�
                //pSink->AddLine(D2D1::Point2F(width - radius, height)); // �������½�
                //pSink->AddLine(D2D1::Point2F(radius, height)); // �������½�
                //pSink->AddLine(D2D1::Point2F(width - radius, height)); // �ص���ʼ��

                // �����Ҳ��Բ
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(width - radius + rect.X, rect.Y+ height), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ����·�����ε�����
                (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
                (*psk)->Close();

                
            };
            auto createPG_V = [&pFactory](ID2D1PathGeometry** pg, ID2D1GeometrySink** psk, const GRect& rect) {
                FLOAT radius = rect.Width / 2;
                FLOAT width = rect.Width;
                FLOAT height = rect.Height;
                // ����·������


                // ��������ͼ��
                pFactory->CreatePathGeometry(pg);

                // ��������ͼ�ε�����
                (*pg)->Open(psk);

                // ��������Բ
                (*psk)->BeginFigure(D2D1::Point2F(rect.X, radius+ rect.Y), D2D1_FIGURE_BEGIN_FILLED);
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(rect.X+width, radius + rect.Y), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ���ƾ���
                (*psk)->AddLine(D2D1::Point2F(rect.X+width , rect.Y+ height- radius)); // �������Ͻ�


                // �����Ҳ��Բ
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(rect.X, rect.Y + height - radius), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ����·�����ε�����
                (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
                (*psk)->Close();


            };
            
            // ����·������
            ID2D1PathGeometry* pPathGeometry = nullptr;
            ID2D1GeometrySink* pSink = nullptr;
            GRect rect(0, 0, m_rect.Width, m_rect.Height);
            if (m_rect.Width > m_rect.Height) {

                createPG_H(&pPathGeometry, &pSink, rect);
            }
            else {
                createPG_V(&pPathGeometry, &pSink, rect);
            }
            pRenderTarget->BeginDraw();
            // ������ͼ�λ��Ƶ�Ŀ��
            //pRenderTarget->DrawGeometry(pPathGeometry, pBrush);

            ID2D1SolidColorBrush* pBrush2 = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(color, 1.0), &pBrush2);
            pRenderTarget->FillGeometry(pPathGeometry, pBrush2);
            pBrush2->Release();
            pSink->Release();
            pPathGeometry->Release();
        
            
            std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
            long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now- __startT).count();
            long long freq = 30;//ms
            if (dta < freq) {
                // ��������
                ID2D1SolidColorBrush* pBrush = nullptr;
                pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffff, 1.0), &pBrush);
                if (m_rect.Width > m_rect.Height) {
                    rect.Inflate(0, -1);
                    int offset = dta * (m_rect.Width - radius * 2) / freq;
                    if (0 == __state) {
                        rect.Width -= offset;

                    }
                    else {

                        rect.X = offset;
                        rect.Width -= offset;

                    }
                    createPG_H(&pPathGeometry, &pSink, rect);
                    pRenderTarget->FillGeometry(pPathGeometry, pBrush);

                }
                else {
                    rect.Inflate(-1, 0);

                    int offset = dta * (m_rect.Height - radius * 2) / freq;
                    if (0 == __state) {
                        rect.Height -= offset;

                    }
                    else {

                        rect.Y = offset;
                        rect.Height -= offset;

                    }
                    createPG_V(&pPathGeometry, &pSink, rect);
                    pRenderTarget->FillGeometry(pPathGeometry, pBrush);


                
                }
                // �ͷ���Դ
                pBrush->Release();
                pSink->Release();
                pPathGeometry->Release();
                isChange = true;
                UpdateWND();
                
             
            }
            else{ //����Բ��
            
                 // Define gradient stops
                D2D1_GRADIENT_STOP gradientStops[2] = {
                    { 0.0f, D2D1::ColorF(0xffffff,1.0) },   // Start color
                    { 1.0f, D2D1::ColorF(0xffffff,0.8) }   // End color
                };

                // Create gradient stop collection
                ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
                HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    ARRAYSIZE(gradientStops),
                    &pGradientStopCollection
                );


                D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientProperties = D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(rx, ry), // Start point
                    D2D1::Point2F(0, 0),// End point
                    radius,                           // Radius X
                    radius                            // Radius Y
                );
                ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
                m_pRenderTarget->CreateRadialGradientBrush(
                    radialGradientProperties,
                    pGradientStopCollection,
                    &pRadialGradientBrush
                );

                FLOAT radius2 = (std::min)((float)rect.Height, (float)rect.Width) / 2;
                D2D1_ELLIPSE elle(
                    D2D1::Point2F(rx, ry), // Center of the ellipse
                    radius2, // Horizontal radius
                    radius2  // Vertical radius
                );
  
                m_pRenderTarget->FillEllipse(elle, pRadialGradientBrush);
               
                pGradientStopCollection->Release();
                pRadialGradientBrush->Release();
                //��겻�ڿؼ���
                if (!__isInctrl) {
                    ID2D1SolidColorBrush* pbrush = nullptr;
                    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xff000000, 0.1), &pbrush);

                    m_pRenderTarget->FillEllipse(elle, pbrush);


                    pbrush->Release();
                
                
                }
               



            }
            // End drawing
            pRenderTarget->EndDraw();


        }
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}
BOOL QSwitch::__Contains(IN INT x, IN INT y)const 
{
    int dtax = x - m_rect.X;
    int dtay = y - m_rect.Y;
    //��Ϊ���һ�������
    if (m_rect.Width > m_rect.Height) {
        //���ҵ�
        FLOAT radius = m_rect.Height / 2;
        FLOAT width = m_rect.Width;
        FLOAT height = m_rect.Height;
        
        //1���Ƿ������ұ�Բ��
        float rx1 = radius , rx2= width- radius;
        float ry1 = radius, ry2 = radius;
        if ((std::pow(dtax - rx1, 2) + std::pow(dtay - ry1, 2)) <= std::pow(radius, 2) ||
            (std::pow(dtax - rx2, 2) + std::pow(dtay - ry2, 2)) <= std::pow(radius, 2)) {
        
            return TRUE;
        }
        GRect rect(radius,0, width-2* radius, height);
        return rect.Contains(dtax, dtay);

    }
    else {
        //���µ�
        FLOAT radius = m_rect.Width / 2;
        FLOAT width = m_rect.Width;
        FLOAT height = m_rect.Height;
        //1���Ƿ������ұ�Բ��
        float rx1 = radius, rx2 = radius;
        float ry1 = radius, ry2 = height- radius;
        if ((std::pow(dtax - rx1, 2) + std::pow(dtay - ry1, 2)) <= std::pow(radius, 2) ||
            (std::pow(dtax - rx2, 2) + std::pow(dtay - ry2, 2)) <= std::pow(radius, 2)) {

            return TRUE;
        }
        GRect rect(0, radius, width, height - 2 * radius);
        return rect.Contains(dtax, dtay);
    
    }
    return 0;
}
int QSwitch::LButtonDown(int x, int y, WPARAM wParam)
{
    if (m_rect.Contains(x, y)) {
        if (__state)__state = 0;
        else {
            //��ø��ؼ����ӿؼ�
            if (__isMutex && m_Parent) {
                QWND* p = nullptr;
                int count = m_Parent->TransferMessage(QM_GET_SUB_CTRL, 0,-1,0);
                for (int i = -1; ++i < count;) {
                    p = nullptr;
                    count = m_Parent->TransferMessage(QM_GET_SUB_CTRL, QDWORD(&p),i, 0);
                    if (p) {
                        p->TransferMessage(QM_QSWITCH_STATE, 0, 0, 0);
                    }
                }
            
            }
            __state = 1;

        }
        __startT = std::chrono::high_resolution_clock::now();
        if (__State_ChangeEvent)__State_ChangeEvent((QPOINTER)this);
    
    }
    isChange = true;
    UpdateWND();
    return 0;
}
BOOL QSwitch::Contains(IN const GPoint& pt) const
{


    return __Contains(pt.X,pt.Y);
}
BOOL QSwitch::Contains(IN INT x, IN INT y) const
{
    return __Contains(x, y);
}
int QSwitch::MouseMoveOut()
{
    __isInctrl = false;
    isChange = true;
    UpdateWND();
    return 0;
}
int QSwitch::MouseMove(int x, int y, WPARAM wParam)
{
    if (__Contains(x, y)) {
        __isInctrl = true;
    
    }
    else {
        __isInctrl = false;
    }
    isChange = true;
    UpdateWND();
    return 0;
}
QDWORD QSwitch::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID)
    {
    case QM_QSWITCH_STATE: {
        //��������״̬�����ı�
        __state = LParam & 0x01;
        isChange = true;
        UpdateWND();

    }return QMSGDEF_RET;


    default:
        break;
    }



    return QCtrlBaseD2DIPD::TransferMessage(msgID,LParam,RParam,AddParam);
}
int QSwitch::SetStateChangeEvent(QSWITCH_STATE_CHANGE ev)
{
    __State_ChangeEvent = ev;
    return 1;
}
int QSwitch::GetState() const
{
    return __state;
}
void QSwitch::SetState(int state)
{
    __state = state;
  
    isChange = true;
    UpdateWND();
}
void QSwitch::IsMutex(bool im)
{
    __isMutex = im;
}
#endif // __QSWITCH__


extern "C"
{
#define __EXTERN_FUN(name) QSW##name
    //�û��ӿ�
  
    __declspec(dllexport) int  __EXTERN_FUN(SetStateChangeEvent)(QSwitch* p, QSWITCH_STATE_CHANGE ev)
    {
    
        return p->SetStateChangeEvent(ev);
    }
    __declspec(dllexport) int  __EXTERN_FUN(GetState)(QSwitch* p)
    {
        return p->GetState();
    
    }
    __declspec(dllexport) void  __EXTERN_FUN(SetState)(QSwitch* p, int state)
    {
        return p->SetState(state);
    
    }
    __declspec(dllexport) void  __EXTERN_FUN(IsMutex)(QSwitch* p, bool im)
    {
    
        return p->IsMutex(im);
    }



}



#ifdef __QSLIDER__




QSlider::QSlider(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
}

QSlider::~QSlider()
{
}

int QSlider::Draw(HDC sdc)
{
    int ret = 0;
    if (isChange) {
        isChange = false;
        ret = 1;
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        //1.���ƴ���
        ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
        ID2D1Factory* pFactory = m_pD2DFactory;
        RECT rc(0, 0, m_rect.Width, m_rect.Height);
        pRenderTarget->BindDC(m_dc, &rc);//��DC���
       
       
        {
            ID2D1SolidColorBrush* pBrush = nullptr;
            int minV = (std::min)(m_rect.Width, m_rect.Height);

            // ��������
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffff, 1.0), &pBrush);
            auto createPG_H = [&pFactory](ID2D1PathGeometry** pg, ID2D1GeometrySink** psk, const GRectF& rect) {
                FLOAT radius = rect.Height / 2;
                FLOAT width = rect.Width;
                FLOAT height = rect.Height;
          


                // ��������ͼ��
                pFactory->CreatePathGeometry(pg);

                // ��������ͼ�ε�����
                (*pg)->Open(psk);

                // ��������Բ
                (*psk)->BeginFigure(D2D1::Point2F(radius + rect.X, rect.Y + height), D2D1_FIGURE_BEGIN_FILLED);
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(radius + rect.X, rect.Y), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ���ƾ���
                (*psk)->AddLine(D2D1::Point2F(width - radius + rect.X, rect.Y)); // �������Ͻ�
                // �����Ҳ��Բ
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(width - radius + rect.X, rect.Y + height), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ����·�����ε�����
                (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
                (*psk)->Close();


            };
            auto createPG_V = [&pFactory](ID2D1PathGeometry** pg, ID2D1GeometrySink** psk, const GRectF& rect) {
                FLOAT radius = rect.Width / 2;
                FLOAT width = rect.Width;
                FLOAT height = rect.Height;
                // ����·������


                // ��������ͼ��
                pFactory->CreatePathGeometry(pg);

                // ��������ͼ�ε�����
                (*pg)->Open(psk);

                // ��������Բ
                (*psk)->BeginFigure(D2D1::Point2F(rect.X, radius + rect.Y), D2D1_FIGURE_BEGIN_FILLED);
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(rect.X + width, radius + rect.Y), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ���ƾ���
                (*psk)->AddLine(D2D1::Point2F(rect.X + width, rect.Y + height - radius)); // �������Ͻ�


                // �����Ҳ��Բ
                (*psk)->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(rect.X, rect.Y + height - radius), // ������
                    D2D1::SizeF(radius, radius),
                    0.0f, // ��ת�Ƕ�
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                // ����·�����ε�����
                (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
                (*psk)->Close();


            };


          
            pRenderTarget->BeginDraw();

            ID2D1SolidColorBrush* pBrush2 = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(__railColor&0xffffff, ((__railColor>>24)&0xff)/255.0), &pBrush2);
         
            //����Բ��
            if (0 == __direction) {
                GRectF rect(0, m_rect.Height * (1 - __railRatio) / 2, m_rect.Width, m_rect.Height * __railRatio);
                // ����·������
                ID2D1PathGeometry* pPathGeometry = nullptr;
                ID2D1GeometrySink* pSink = nullptr;
                createPG_H(&pPathGeometry, &pSink, rect);
                pRenderTarget->FillGeometry(pPathGeometry, pBrush2);
                float radius = minV / 2.0;
                float rx = radius+(m_rect.Width-minV)* ((__maxValue == __minValue)?0: __currenValue/(__maxValue- __minValue));
                float ry = radius;
                float sw = 1.0;
                ID2D1SolidColorBrush* psB = nullptr;

                // ��������
                pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(__BKColor & 0xffffff, ((__BKColor >> 24) & 0xff) / 255.0), &psB);
                if (__isInBt || __isLDown) {
                    sw = 2.0;
                }
                if(__isSegm){//�����ֶΣ����Ʒֶ�λ��
                    std::shared_lock<std::shared_mutex> lk(__SegmLK);
                    for (const auto& it : __SegmArr) {
                        if (it<__minValue || it>__maxValue)continue;
                        float radius2 = (minV / 2.0)*0.7;
                        float rx2 = radius + (m_rect.Width - minV) * ((__maxValue == __minValue) ? 0 : it / (__maxValue - __minValue));
                        float ry2 = radius;
                        float sw2 = 2.0;
                        D2D1_ELLIPSE elle1(
                            D2D1::Point2F(rx2, ry2), // Center of the ellipse
                            radius2, // Horizontal radius
                            radius2  // Vertical radius
                        );
                        D2D1_ELLIPSE elle2(
                            D2D1::Point2F(rx2, ry2), // Center of the ellipse
                            radius2- sw2, // Horizontal radius
                            radius2- sw2  // Vertical radius
                        );
                        pRenderTarget->FillEllipse(elle1, psB);
                        pRenderTarget->DrawEllipse(elle2, pBrush2, sw2);
                    
                    }
                
                }


                D2D1_ELLIPSE elle1(
                    D2D1::Point2F(rx, ry), // Center of the ellipse
                    radius, // Horizontal radius
                    radius  // Vertical radius
                );
                D2D1_ELLIPSE elle2(
                    D2D1::Point2F(rx, ry), // Center of the ellipse
                    radius - sw, // Horizontal radius
                    radius - sw  // Vertical radius
                );
  
                pRenderTarget->FillEllipse(elle1, psB);
                pRenderTarget->DrawEllipse(elle2, pBrush2, sw);
                
            
                pBrush2->Release();
                pSink->Release();
                pPathGeometry->Release();
                psB->Release();
            
            }
            else {
                GRectF rect(m_rect.Width* (1 - __railRatio) / 2,0, m_rect.Width* __railRatio, m_rect.Height);
                // ����·������
                ID2D1PathGeometry* pPathGeometry = nullptr;
                ID2D1GeometrySink* pSink = nullptr;
                createPG_V(&pPathGeometry, &pSink, rect);
                pRenderTarget->FillGeometry(pPathGeometry, pBrush2);
                float radius = minV / 2.0;
                float ry = radius + (m_rect.Height - minV) * ((__maxValue == __minValue) ? 0 : __currenValue / (__maxValue - __minValue));
                float rx = radius;
                float sw = 1.0;
                ID2D1SolidColorBrush* psB = nullptr;

                // ��������
                pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(__BKColor & 0xffffff, ((__BKColor >> 24) & 0xff) / 255.0), &psB);
                if (__isInBt || __isLDown) {
                    sw = 2.0;
                }
                if (__isSegm) {//�����ֶΣ����Ʒֶ�λ��
                    std::shared_lock<std::shared_mutex> lk(__SegmLK);
                    for (const auto& it : __SegmArr) {
                        if (it<__minValue || it>__maxValue)continue;
                        float radius2 = (minV / 2.0) * 0.7;
                        float ry2 = radius + (m_rect.Height - minV) * ((__maxValue == __minValue) ? 0 : it / (__maxValue - __minValue));
                        float rx2 = radius;
                        float sw2 = 2.0;
                        D2D1_ELLIPSE elle1(
                            D2D1::Point2F(rx2, ry2), // Center of the ellipse
                            radius2, // Horizontal radius
                            radius2  // Vertical radius
                        );
                        D2D1_ELLIPSE elle2(
                            D2D1::Point2F(rx2, ry2), // Center of the ellipse
                            radius2 - sw2, // Horizontal radius
                            radius2 - sw2  // Vertical radius
                        );
                        pRenderTarget->FillEllipse(elle1, psB);
                        pRenderTarget->DrawEllipse(elle2, pBrush2, sw2);

                    }

                }


                D2D1_ELLIPSE elle1(
                    D2D1::Point2F(rx, ry), // Center of the ellipse
                    radius, // Horizontal radius
                    radius  // Vertical radius
                );
                D2D1_ELLIPSE elle2(
                    D2D1::Point2F(rx, ry), // Center of the ellipse
                    radius - sw, // Horizontal radius
                    radius - sw  // Vertical radius
                );

                pRenderTarget->FillEllipse(elle1, psB);
                pRenderTarget->DrawEllipse(elle2, pBrush2, sw);


                pBrush2->Release();
                pSink->Release();
                pPathGeometry->Release();
                psB->Release();
            
            
            }

            pRenderTarget->EndDraw();
            pBrush->Release();

        }
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}

int QSlider::MouseMoveOut()
{
    __isInBt = false;
    isChange = true;
    UpdateWND();
    return 0;
}

int QSlider::MouseMove(int x, int y, WPARAM wParam)
{
    int minV = (std::min)(m_rect.Width, m_rect.Height);
    int dx = x - m_rect.X;
    int dy = y - m_rect.Y;
    if (0 == __direction) {

        float radius = minV / 2.0;
        float rx = radius + (m_rect.Width - minV) * ((__maxValue == __minValue) ? 0 : __currenValue / (__maxValue - __minValue));
        float ry = radius;
        if (std::pow(dx - rx, 2) + std::pow(dy - ry, 2) < std::pow(radius, 2)) {
            __isInBt = true;
        }
        else if (GRectF rect(0, m_rect.Height * (1 - __railRatio) / 2, m_rect.Width, m_rect.Height * __railRatio); rect.Contains(dx,dy)) {//����ڵ�����Ҳ�ǽ���
            __isInBt = true;
        }
        else {
            __isInBt = false;
        }
        if (__isLDown) {
           
            float dx1 = dx;
            if (dx1 - radius <= 0)dx1 = radius;
            else if (dx1 > m_rect.Width - radius)dx1 = m_rect.Width - radius;

            float ato = (dx1 - radius) / (m_rect.Width - minV);//�õ�һ������
            float value = (__maxValue - __minValue) * ato + __minValue;
  /*          if (value < __minValue)value = __minValue;
            else if (value > __maxValue) {
                value = __maxValue;
            }*/

            __ChangeCurrenValue(value);
        

            
         
        
        }
        
    
    
    }
    else {
        float radius = minV / 2.0;
        float ry = radius + (m_rect.Height - minV) * ((__maxValue == __minValue) ? 0 : __currenValue / (__maxValue - __minValue));
        float rx = radius;
        if (std::pow(dx - rx, 2) + std::pow(dy - ry, 2) < std::pow(radius, 2)) {
            __isInBt = true;
        }
        else if (GRectF rect(m_rect.Width * (1 - __railRatio) / 2,0, m_rect.Width * __railRatio, m_rect.Height); rect.Contains(dx, dy)) {//����ڵ�����Ҳ�ǽ���
            __isInBt = true;
        }
        else {
            __isInBt = false;
        }
        if (__isLDown) {

            float dy1 = dy;
            if (dy1 - radius <= 0)dy1 = radius;
            else if (dy1 > m_rect.Height - radius)dy1 = m_rect.Height - radius;

            float ato = (dy1 - radius) / (m_rect.Height - minV);//�õ�һ������
            float value = (__maxValue - __minValue) * ato + __minValue;
 

            __ChangeCurrenValue(value);




        }
    
    }

    isChange = true;
    UpdateWND();
    return 0;
}

int QSlider::LButtonDown(int x, int y, WPARAM wParam)
{
    int minV = (std::min)(m_rect.Width, m_rect.Height);
    int dx = x - m_rect.X;
    int dy = y - m_rect.Y;
    if (0 == __direction) {

        float radius = minV / 2.0;
        float rx = radius + (m_rect.Width - minV) * ((__maxValue == __minValue) ? 0 : __currenValue / (__maxValue - __minValue));
        float ry = radius;
        if (std::pow(dx - rx, 2) + std::pow(dy - ry, 2) < std::pow(radius, 2)) {
            __isInBt = true;
            __isLDown = true;
            __lDownPX = dx;
            __lDownPY = dy;
        }
        else if (GRectF rect(0, m_rect.Height * (1 - __railRatio) / 2, m_rect.Width, m_rect.Height * __railRatio); rect.Contains(dx, dy)) {
           //��������ƶ�����Ӧ��λ��
            int dx1 = dx;
            if (dx1 - radius < 0)dx1 = radius;
            else if (dx1 > m_rect.Width - radius)dx1 = m_rect.Width - radius;

            float ato = (dx1 - radius) / (m_rect.Width - minV);//�õ�һ������
            __ChangeCurrenValue((__maxValue - __minValue) * ato + __minValue);
        }
        else {
            __isInBt = false;
        }




    }
    else {
        float radius = minV / 2.0;
        float ry = radius + (m_rect.Height - minV) * ((__maxValue == __minValue) ? 0 : __currenValue / (__maxValue - __minValue));
        float rx = radius;
        if (std::pow(dx - rx, 2) + std::pow(dy - ry, 2) < std::pow(radius, 2)) {
            __isInBt = true;
            __isLDown = true;
            __lDownPX = dx;
            __lDownPY = dy;
        }
        else if (GRectF rect( m_rect.Width * (1 - __railRatio) / 2,0, m_rect.Width * __railRatio, m_rect.Height ); rect.Contains(dx, dy)) {
            //��������ƶ�����Ӧ��λ��
            int dy1 = dy;
            if (dy1 - radius < 0)dy1 = radius;
            else if (dy1 > m_rect.Height - radius)dy1 = m_rect.Height - radius;

            float ato = (dy1 - radius) / (m_rect.Height - minV);//�õ�һ������
            __ChangeCurrenValue((__maxValue - __minValue) * ato + __minValue);
        }
        else {
            __isInBt = false;
        }

    
    }
    isChange = true;
    UpdateWND();
    return 1;
}

int QSlider::LButtonUp(int x, int y, WPARAM wParam)
{
    __isLDown = false;
    return 0;
}

void QSlider::__ChangeCurrenValue(float v)
{
    if (__isSegm) {
        std::shared_lock<std::shared_mutex> lk(__SegmLK);
        if(__SegmArr.size()>0){
            float lv = __minValue;
            float hv = __maxValue;
            auto lower = __SegmArr.lower_bound(v);
            auto upper = __SegmArr.upper_bound(v);
            if (lower == __SegmArr.end()) {
                // ��� lower == mySet.end()��˵�� target �ȼ���������Ԫ�ض���
                v = *(--lower);
            }
            else if (lower == __SegmArr.begin()) {
                // ��� lower == mySet.begin()��˵�� target �ȼ���������Ԫ�ض�С
                v = *lower ;
            }
            else {
                // �������ǱȽ� lower ��ǰһ��Ԫ�صľ��룬�ҳ���ӽ�������ֵ
                auto prev = std::prev(lower);
                if (upper == __SegmArr.end()) {

                    lv = *prev;
                    hv = *lower;

                }
                else {

                    lv = abs(v - *prev) < abs(v - *lower) ? *prev : *lower;
                    hv = *upper;
                }
            
                v = abs(v - lv) < abs(v - hv) ? lv : hv;
            }
            
        }


    
    
    }


    __currenValue = v;
    printf("__ChangeCurrenValue v= %f\n",v);
    if (__ValueChangeEvent)__ValueChangeEvent(int(this), __currenValue);
    isChange = true;
    UpdateWND();
}

void QSlider::SetMinValue(float v)
{
    if (v >= __maxValue)return;
    if (v > __currenValue)__currenValue = v;
    __minValue = v;
    isChange = true;
    UpdateWND();
}

void QSlider::SetCurrentValue(float v)
{
    if (v<__minValue || v > __maxValue)return;
    __currenValue = v;
    isChange = true;
    UpdateWND();
}

unsigned int QSlider::SetRailColor(unsigned int color)
{
    unsigned int oldColor = __railColor;
    __railColor = color;
    isChange = true;
    UpdateWND();
    return oldColor;
}

float QSlider::SetRailRatio(float ato)
{
    float oldAto = __railRatio;
    __railRatio = ato;
    isChange = true;
    UpdateWND();
    return oldAto;
}

void QSlider::IsSegmentation(bool b)
{
    __isSegm = b;
    isChange = true;
    UpdateWND();
}

int QSlider::ClearSegmArr()
{
    std::unique_lock<std::shared_mutex> lk(__SegmLK);
    __SegmArr.clear();
    isChange = true;
    UpdateWND();
    return 1;
}

int QSlider::AddSegmVuale(float v)
{
    std::unique_lock<std::shared_mutex> lk(__SegmLK);
    __SegmArr.insert(v);
    isChange = true;
    UpdateWND();
    return 1;
}

void QSlider::SetDirection(unsigned char d)
{
    if (d)d = 1;
    else d = 0;

    __direction = d;
    isChange = true;
    UpdateWND();
}

QDWORD QSlider::SetValueChangeEvent(QSLIDER_VALUE_CHANGE ev)
{
    QDWORD buf = (QDWORD)__ValueChangeEvent;
    __ValueChangeEvent = ev;

    return buf;
}

unsigned int QSlider::SetColor(unsigned int color)
{
    unsigned int buf = __BKColor;
    __BKColor = color;
    isChange = true;
    UpdateWND();
    return buf;
}

int QSlider::GetSegmValue(float* arr, int bufL)
{
    std::shared_lock<std::shared_mutex> lk(__SegmLK);
    if (arr) {
        if (bufL < __SegmArr.size()) {
            for (int i = -1; ++i < bufL;){
                auto it = __SegmArr.begin();
                std::advance(it, i); // �ƶ�������
                arr[i] = *it;
            }
        
        }
        else {
            int i = -1;
            for (auto& it: __SegmArr) {
       
                arr[++i] = it;
            }
        }
    
    }

    return __SegmArr.size();
}

int QSlider::RemoveSegmVuale(float v)
{
    std::unique_lock<std::shared_mutex> lk(__SegmLK);
    __SegmArr.erase(v);
    return 1;
}

void QSlider::SetMaxValue(float v)
{
    if (v <= __minValue)return;
    if (v < __currenValue)__currenValue = v;

    __maxValue = v;

}



#endif // __QSLIDER__
extern "C"
{
#define __EXTERN_FUN(name) QSL##name

    __declspec(dllexport) void __EXTERN_FUN(SetMinValue)(QSlider* p, float v)
    {
    
        return p->SetMinValue(v);
    }
    __declspec(dllexport) void __EXTERN_FUN(SetMaxValue)(QSlider* p, float v)
    {
        return p->SetMaxValue(v);
    }
    __declspec(dllexport) void __EXTERN_FUN(SetCurrentValue)(QSlider* p, float v)
    {
    
        return p->SetCurrentValue(v);
    }
    __declspec(dllexport) unsigned int __EXTERN_FUN(SetRailColor)(QSlider* p, unsigned int color)
    {
        return p->SetRailColor(color);
    }
    __declspec(dllexport) float __EXTERN_FUN(SetRailRatio)(QSlider* p, float ato)
    {
        return p->SetRailRatio(ato);
    }
    __declspec(dllexport) void __EXTERN_FUN(IsSegmentation)(QSlider* p, bool b)
    {
        return p->IsSegmentation(b);
    }
    __declspec(dllexport) int __EXTERN_FUN(ClearSegmArr)(QSlider* p)
    {
        return p->ClearSegmArr();
    }
    __declspec(dllexport) int __EXTERN_FUN(AddSegmVuale)(QSlider* p, float v)
    {
        return p->AddSegmVuale(v);
    }
    __declspec(dllexport) int __EXTERN_FUN(RemoveSegmVuale)(QSlider* p, float v)
    {
        return p->RemoveSegmVuale(v);
    }
    __declspec(dllexport) void __EXTERN_FUN(SetDirection)(QSlider* p, unsigned char d)
    {
        return p->SetDirection(d);
    }

    __declspec(dllexport) QDWORD __EXTERN_FUN(SetValueChangeEvent)(QSlider* p, QSLIDER_VALUE_CHANGE ev)
    {
        return p->SetValueChangeEvent(ev);
    }
    __declspec(dllexport) unsigned int __EXTERN_FUN(SetColor)(QSlider* p, unsigned int color)
    {
        return p->SetColor(color);
    
    }
    //��ȡ�ֶ�ֵ
    __declspec(dllexport) int  __EXTERN_FUN(GetSegmValue)(QSlider* p, float* arr, int bufL)
    {
        return p->GetSegmValue(arr, bufL);
    }

}


#ifdef __QPROGRESS__
#include "caicoAlgorithm.h"


QProgress::QProgress(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{


    // ����ԴPNGͼ����棬��Ⱥ͸߶�Ϊ
    __surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, __currentRW, __currentRH);
    __cr = cairo_create(__surface);
    int minV = (std::min)(rf.Width, rf.Height);
    if (minV / 2.0 <= __thickness)__thickness = minV / 4.0;
    //draw_gradient_ring(__cr, __currentRW/2.0, __currentRH/2.0, minV/2.0, __thickness);
    __threadRingColor();


}

void QProgress::__threadRingColor()
{
    __isThreadRefresh = true;
    double thickness = __thickness;

    std::unique_lock<std::mutex> lk(__lockRColor, std::try_to_lock);
    if (lk.owns_lock()) {
        if (__threadRColor.joinable())__threadRColor.join();
        __threadRColor.~thread();
        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (minV / 2.0 <= thickness)thickness = minV / 4.0;
        std::thread t(&QProgress::__threadDraw_gradient_ring, this, __currentRW / 2.0, __currentRH / 2.0, minV / 2.0, thickness);
        __threadRColor = std::move(t);
    }
}

void QProgress::Reconstruct(HDC sdc, const GRect& rect)
{

    QCtrlBaseD2DIPD::Reconstruct(sdc, rect);

    int minV = (std::min)(rect.Width, rect.Height);
    if (minV / 2.0 <= __thickness)__thickness = minV / 4.0;
    if (rect.Width > __currentRW || rect.Height > __currentRH) {
        __currentRW = rect.Width;
        __currentRH = rect.Height;
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, __currentRW, __currentRH);//��ͼ����
        cairo_t* cr = cairo_create(surface);//��ͼ����
        cairo_surface_t* surfaceBuf  = __surface;
        cairo_t* crBuf = __cr;

        __cr = cr;
        __surface = surface;
        cairo_destroy(crBuf);
        cairo_surface_destroy(surfaceBuf);
    }
    //draw_gradient_ring(__cr, __currentRW / 2.0, __currentRH / 2.0, minV / 2.0, __thickness);
    //__threadRColor.~thread();
    __threadRingColor();

}

void QProgress::__threadDraw_gradient_ring(double centerX, double centerY, double radius, double thickness)
{
   
    while (__isThreadRefresh) {
        __isThreadRefresh = false;
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, __currentRW, __currentRH);//��ͼ����
        cairo_t* cr = cairo_create(surface);//��ͼ����
      
        draw_gradient_ring(cr, centerX, centerY, radius, thickness, __ringcolor1, __ringcolor2, &__isThreadRefresh);
        cairo_t* buf = __cr;
        cairo_surface_t* bufs = __surface;
        __surface = surface;
        __cr = cr;
        cairo_destroy(buf);
        cairo_surface_destroy(bufs);
        isChange = true;
        UpdateWND();
        //draw_gradient_ring(__cr, centerX, centerY, radius, thickness, __ringcolor1, __ringcolor2, &__isThreadRefresh);
    }

}

QProgress::~QProgress()
{
    if (__threadRColor.joinable())__threadRColor.join();
    __threadRColor.~thread();
    cairo_destroy(__cr);
    cairo_surface_destroy(__surface);
}

int QProgress::Draw(HDC sdc)
{
    int ret = 0;
    if (isChange) {
        isChange = false;
        ret = 1;
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
        ID2D1Factory* pFactory = m_pD2DFactory;

        RECT rc(0, 0, m_rect.Width, m_rect.Height);
        pRenderTarget->BindDC(m_dc, &rc);//��DC���
        /*pRenderTarget->BeginDraw();*/
        switch (__style) {
        case 0: {
            DrawSLStyle();

        }break;
        case 1: {

            DrawRingStyle();
        }break;


        default:
            DrawSLStyle();
            break;

        
        
        }
      
        //pRenderTarget->EndDraw();
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}

void QProgress::DrawSLStyle()
{
    ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
    ID2D1Factory* pFactory = m_pD2DFactory;
    ID2D1SolidColorBrush* pBrush = nullptr;
    pRenderTarget->BeginDraw();

    // ��������
    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffff, 1.0), &pBrush);
    auto createPG_H = [&pFactory](ID2D1PathGeometry** pg, ID2D1GeometrySink** psk, const GRectF& rect) {
        FLOAT radius = rect.Height / 2;
        FLOAT width = rect.Width;
        FLOAT height = rect.Height;



        // ��������ͼ��
        pFactory->CreatePathGeometry(pg);

        // ��������ͼ�ε�����
        (*pg)->Open(psk);

        // ��������Բ
        (*psk)->BeginFigure(D2D1::Point2F(radius + rect.X, rect.Y + height), D2D1_FIGURE_BEGIN_FILLED);
        (*psk)->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(radius + rect.X, rect.Y), // ������
            D2D1::SizeF(radius, radius),
            0.0f, // ��ת�Ƕ�
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));

        // ���ƾ���
        (*psk)->AddLine(D2D1::Point2F(width - radius + rect.X, rect.Y)); // �������Ͻ�
        // �����Ҳ��Բ
        (*psk)->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(width - radius + rect.X, rect.Y + height), // ������
            D2D1::SizeF(radius, radius),
            0.0f, // ��ת�Ƕ�
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));

        // ����·�����ε�����
        (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
        (*psk)->Close();


    };
    auto createPG_V = [&pFactory](ID2D1PathGeometry** pg, ID2D1GeometrySink** psk, const GRectF& rect) {
        FLOAT radius = rect.Width / 2;
        FLOAT width = rect.Width;
        FLOAT height = rect.Height;
        // ����·������


        // ��������ͼ��
        pFactory->CreatePathGeometry(pg);

        // ��������ͼ�ε�����
        (*pg)->Open(psk);

        // ��������Բ
        (*psk)->BeginFigure(D2D1::Point2F(rect.X, radius + rect.Y), D2D1_FIGURE_BEGIN_FILLED);
        (*psk)->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(rect.X + width, radius + rect.Y), // ������
            D2D1::SizeF(radius, radius),
            0.0f, // ��ת�Ƕ�
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));

        // ���ƾ���
        (*psk)->AddLine(D2D1::Point2F(rect.X + width, rect.Y + height - radius)); // �������Ͻ�


        // �����Ҳ��Բ
        (*psk)->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(rect.X, rect.Y + height - radius), // ������
            D2D1::SizeF(radius, radius),
            0.0f, // ��ת�Ƕ�
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));

        // ����·�����ε�����
        (*psk)->EndFigure(D2D1_FIGURE_END_CLOSED);
        (*psk)->Close();


    };





    ID2D1SolidColorBrush* pBrush2 = nullptr;
    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(__Color & 0xffffff, ((__Color >> 24) & 0xff) / 255.0), &pBrush2);

    GRectF rect(0, 0, m_rect.Width, m_rect.Height);
    //����Բ��
    if (0 == __direction && (m_rect.Height < m_rect.Width)) {

        // ����·������
        ID2D1PathGeometry* pPathGeometry = nullptr;
        ID2D1GeometrySink* pSink = nullptr;
        createPG_H(&pPathGeometry, &pSink, rect);
        pRenderTarget->DrawGeometry(pPathGeometry, pBrush2);
        pSink->Release();
        pPathGeometry->Release();
        GRectF rect1(rect);
        rect1.Width = __currentValue / 100.0 * m_rect.Width;
        ID2D1Layer* pLayer = NULL;//����һ��ͼ��
        m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pPathGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(), 1.0), pLayer);
        createPG_H(&pPathGeometry, &pSink, rect1);


        pRenderTarget->FillGeometry(pPathGeometry, pBrush2);
        //ֹͣͼ��
        m_pRenderTarget->PopLayer();
        //pLayer->Release();
        pSink->Release();
        pPathGeometry->Release();
        pBrush2->Release();



        std::wstring text;
        switch ((int)__decimals) {
        case 0:
            text = std::format(L"{}%", std::ceil(__currentValue));
            break;
        case 1:
            text = std::format(L"{:.1f}%", __currentValue);
            break;
        case 2:
            text = std::format(L"{:.2f}%", __currentValue);
            break;
        default:
            text = std::format(L"{:.3f}%", __currentValue);
            break;
        }
        //std::wstring text =std::format(format, __currentValue);
        ID2D1SolidColorBrush* pfBrush = nullptr;
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_fontColor & 0xffffff, ((m_fontColor >> 24) & 0xff) / 255.0), &pfBrush);
        //�����ַ�
        pRenderTarget->DrawText(
            text.c_str(),
            text.length(),
            pTextFormat,             // The text format
            D2D1_RECT_F(0, 0, m_rect.Width, m_rect.Height),              // The layout rectangle
            pfBrush                   // The brush

        );
        pfBrush->Release();
    }
    else if((m_rect.Height >= m_rect.Width)) {
        std::wstring text;
        switch ((int)__decimals) {
        case 0:
            text = std::format(L"{}%", std::ceil(__currentValue));
            break;
        case 1:
            text = std::format(L"{:.1f}%", __currentValue);
            break;
        case 2:
            text = std::format(L"{:.2f}%", __currentValue);
            break;
        default:
            text = std::format(L"{:.3f}%", __currentValue);
            break;
        }
        IDWriteTextLayout* pTextLayout = nullptr;

        // ����һ���ı����ֶ���
        HRESULT hr = m_pDWriteFactory->CreateTextLayout(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            pTextFormat,
            FLT_MAX, // ����ȣ�����Ϊ FLT_MAX ��ʹ�ı����ֵĸ߶Ȳ��ܿ������
            FLT_MAX,
            &pTextLayout
        );
        DWRITE_TEXT_METRICS textMetrics;
        pTextLayout->GetMetrics(&textMetrics);

        rect.Height -= textMetrics.height;
        pTextLayout->Release();


        // ����·������
        ID2D1PathGeometry* pPathGeometry = nullptr;
        ID2D1GeometrySink* pSink = nullptr;
        createPG_V(&pPathGeometry, &pSink, rect);
        pRenderTarget->DrawGeometry(pPathGeometry, pBrush2);
        pSink->Release();
        pPathGeometry->Release();
        GRectF rect1(rect);
        rect1.Height = __currentValue / 100.0 * (m_rect.Height- m_rect .Width/2.0);
        rect1.Y = rect.Height - rect1.Height;
        ID2D1Layer* pLayer = NULL;//����һ��ͼ��
        m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pPathGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(), 1.0), pLayer);//0.8Ϊ͸����
        createPG_V(&pPathGeometry, &pSink, rect1);


        pRenderTarget->FillGeometry(pPathGeometry, pBrush2);
        //ֹͣͼ��
        m_pRenderTarget->PopLayer();
        //pLayer->Release();

        pSink->Release();
        pPathGeometry->Release();
        pBrush2->Release();



          
        //std::wstring text =std::format(format, __currentValue);
        ID2D1SolidColorBrush* pfBrush = nullptr;
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_fontColor & 0xffffff, ((m_fontColor >> 24) & 0xff) / 255.0), &pfBrush);
        //�����ַ�
        pRenderTarget->DrawText(
            text.c_str(),
            text.length(),
            pTextFormat,             // The text format
            D2D1_RECT_F(0, rect.Height, m_rect.Width, m_rect.Height),              // The layout rectangle
            pfBrush                   // The brush

        );
        pfBrush->Release();
        
        
    }

    pBrush->Release();


    pRenderTarget->EndDraw();
    
}

void QProgress::DrawRingStyle()
{



    ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
    ID2D1Factory* pFactory = m_pD2DFactory;


    pRenderTarget->BeginDraw();

    //ID2D1SolidColorBrush* pBrush;
    //pRenderTarget->CreateSolidColorBrush(
    //    D2D1::ColorF(D2D1::ColorF::SkyBlue),
    //    &pBrush
    //);
    //D2D1_POINT_2F center(150.0f,150.0f);
  
    ////float thickness = 10;
    ////float progress = 0.5;
    ////float radius = 150.0- thickness/2;
    // ����
    //pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    //pBrush->Release();

    //��������
    std::wstring text;
    switch ((int)__decimals) {
    case 0:
        text = std::format(L"{}%", ceil(__currentValue));
        break;
    case 1:
        text = std::format(L"{:.1f}%", __currentValue);
        break;
    case 2:
        text = std::format(L"{:.2f}%", __currentValue);
        break;
    default:
        text = std::format(L"{:.3f}%", __currentValue);
        break;
    }
    //std::wstring text =std::format(format, __currentValue);
    ID2D1SolidColorBrush* pfBrush = nullptr;
    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_fontColor & 0xffffff, ((m_fontColor >> 24) & 0xff) / 255.0), &pfBrush);
    //�����ַ�
    pRenderTarget->DrawText(
        text.c_str(),
        text.length(),
        pTextFormat,             // The text format
        D2D1_RECT_F(0, 0, m_rect.Width, m_rect.Height),              // The layout rectangle
        pfBrush                   // The brush

    );
    pfBrush->Release();
    pRenderTarget->EndDraw();

    // ����Ŀ��PNGͼ����棬��Ⱥ͸߶�Ϊ800x600
    cairo_surface_t* destination_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_rect.Width, m_rect.Height);
    cairo_t* destination_cr = cairo_create(destination_surface);

    // ����Ŀ�����ı���Ϊ��ɫ
    cairo_set_source_rgba(destination_cr, 1.0, 1.0, 1.0, 0);  // RGB��ɫ (��ɫ)
    cairo_paint(destination_cr);

    int WHMinV = (std::min)(m_rect.Width,m_rect.Height);

    //// ����Ŀ�����ļ�������ΪԲ��
    //double cx = m_rect.Width/2.0;  // Բ��x����
    //double cy = m_rect.Height/2.0;  // Բ��y����
    //double radius = WHMinV/2.0;  // Բ�İ뾶

    ////cairo_arc(destination_cr, cx, cy, radius, 0, 2 * M_PI);
    ////cairo_clip(destination_cr);  // ������������ΪԲ��
    //draw_circle(destination_cr, cx, cy, radius);
    //cairo_clip_preserve(destination_cr);

    //// �ڴ�Բ���ڻ���СԲ
    //draw_circle(destination_cr, cx, cy, radius- __thickness);
    //// ����һ���µ�·������ʹ���ཻ��ʽ�ü���ǰ·��
    //cairo_set_fill_rule(destination_cr, CAIRO_FILL_RULE_EVEN_ODD);
    //cairo_clip(destination_cr);
    // ����Բ����40%����
    double radius_outer = WHMinV / 2.0;
    double radius_inner = radius_outer- __thickness;
    if (radius_inner < 0)radius_inner = 1;
    double cx = m_rect.Width / 2.0;
    double cy = m_rect.Height / 2.0;
    //__currentValue+=1;
    //if (__currentValue > 100.0)__currentValue = 0.0;

    cairo_new_path(destination_cr);
    cairo_arc(destination_cr, cx, cy, radius_outer, 0, 2 * M_PI * __currentValue/100.0);
    cairo_arc_negative(destination_cr, cx, cy, radius_inner, 2 * M_PI * __currentValue / 100.0, 0);
    cairo_close_path(destination_cr);

    // ���������ƻ�������
    cairo_clip(destination_cr);


    // ��Դ��������ݻ��Ƶ�Ŀ�����
    cairo_set_source_surface(destination_cr, __surface, cx - __currentRW/2.0, cy - __currentRH/2.0);
    cairo_paint(destination_cr);

    // �����ǰ·�����Ա���佥�����ʱ���ܵ�ǰ·��������
    cairo_new_path(destination_cr);
    // ȡ��������������
    cairo_reset_clip(destination_cr);
    DrawCairoContentToHDC(m_dc,0,0,-1,-1, destination_surface,0,0);






    cairo_destroy(destination_cr);
    cairo_surface_destroy(destination_surface);




    

    //// ���������յ�Ƕ�
    //float startAngle = -90.0f;
    //float endAngle = startAngle + 360.0f * progress;

    //// ת���Ƕ�Ϊ����
    //float startAngleRad = startAngle * (3.14159265358979f / 180.0f);
    //float endAngleRad = endAngle * (3.14159265358979f / 180.0f);

    //// ���������յ�����
    //D2D1_POINT_2F startPoint = D2D1::Point2F(
    //    center.x + radius * cosf(startAngleRad),
    //    center.y + radius * sinf(startAngleRad)
    //);

    //D2D1_POINT_2F endPoint = D2D1::Point2F(
    //    center.x + radius * cosf(endAngleRad),
    //    center.y + radius * sinf(endAngleRad)
    //);










    //// ����Բ��
    //D2D1_ARC_SEGMENT arcSegment = D2D1::ArcSegment(
    //    endPoint,
    //    D2D1::SizeF(radius, radius),
    //    0.0f, // ��ת�Ƕ�
    //    D2D1_SWEEP_DIRECTION_CLOCKWISE,
    //    progress > 0.5f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
    //);

    //// ���弸��·��
    //ID2D1PathGeometry* pPathGeometry = nullptr;
    //ID2D1GeometrySink* pSink = nullptr;
    //pFactory->CreatePathGeometry(&pPathGeometry);
    //pPathGeometry->Open(&pSink);

    //pSink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_HOLLOW);
    //pSink->AddArc(arcSegment);
    //pSink->EndFigure(D2D1_FIGURE_END_OPEN);

    //pSink->Close();
    //SafeRelease(&pSink);

    //// ����Բ��
    //pRenderTarget->DrawGeometry(pPathGeometry, pBrush, thickness);

    //SafeRelease(&pPathGeometry);

}



void QProgress::SetStyle(unsigned int type)
{
    __style = type;
    isChange = true;
    UpdateWND();
}

void QProgress::SetLineTypeProgressColor(unsigned int color)
{
    __Color = color;
    isChange = true;
    UpdateWND();
}

void QProgress::SetCurrentValue(float v)
{
    __currentValue = v;
    isChange = true;
    UpdateWND();
}

void QProgress::SetDecimalPlaces(unsigned char dp)
{
    __decimals = dp;
    isChange = true;
    UpdateWND();
}

void QProgress::SetRingProgressThickness(float thickness)
{
    if (thickness <= 0)return;
    __thickness = thickness;
    __threadRingColor();
    isChange = true;
    UpdateWND();
}

unsigned int QProgress::GetStyle()
{
    return __style;
}



void QProgress::SetDirection(unsigned char d)
{
    __direction = d;
    isChange = true;
    UpdateWND();
}

void QProgress::SetRingProgressColor(unsigned int color1, unsigned int color2)
{
    __ringcolor1 = color1;
    __ringcolor2 = color2;
    //if (__threadRColor.joinable())__threadRColor.join();
    //__threadRColor.~thread();
    __threadRingColor();
    isChange = true;
    UpdateWND();
}















#endif // __QPROGRESS__
extern "C"
{
#define __EXTERN_FUN(name) QPRO##name


    //���ý���������
    __declspec(dllexport) void __EXTERN_FUN(SetDirection)(QProgress* p, unsigned char d)
    {
        return p->SetDirection(d);
    }
    //���ý�������ʽ
    __declspec(dllexport) void __EXTERN_FUN(SetStyle)(QProgress* p, unsigned int style)
    {
        return p->SetStyle(style);
    }
    //���õ�ǰֵ
    __declspec(dllexport) void __EXTERN_FUN(SetCurrentValue)(QProgress* p, float v)
    {
        return p->SetCurrentValue(v);
    }
    //�������ͽ���������ɫ
    __declspec(dllexport) void __EXTERN_FUN(SetLineTypeProgressColor)(QProgress* p, unsigned int color)
    {
        return p->SetLineTypeProgressColor(color);
    }
    //����С����ʾ��λ��
    __declspec(dllexport) void __EXTERN_FUN(SetDecimalPlaces)(QProgress* p, unsigned char dp)
    {
        return p->SetDecimalPlaces(dp);
    }
    //���û��ν���������ɫ
    __declspec(dllexport) void __EXTERN_FUN(SetRingProgressColor)(QProgress* p, unsigned int color1, unsigned int color2)
    {
        return p->SetRingProgressColor(color1, color2);
    }
    //���û��ν���������ɫ
    __declspec(dllexport) void __EXTERN_FUN(SetRingProgressThickness)(QProgress* p, float thickness)
    {
        return p->SetRingProgressThickness(thickness);
    }
    //��ý�������ʽ
    __declspec(dllexport) unsigned int __EXTERN_FUN(GetStyle)(QProgress* p)
    {
        return p->GetStyle();
    
    }

}


#ifdef __QCOLORBOX__
QColorBox::QColorPopDialog* QColorBox::m_cpDialog = nullptr;
QSlider* QColorBox::m_redQslider = nullptr;
QSlider* QColorBox::m_greeQslider = nullptr;
QSlider* QColorBox::m_blueQslider = nullptr;
QSlider* QColorBox::m_alphaQslider = nullptr;
std::mutex QColorBox::createLK;
int QColorBox::__currentCount =0;


inline void QColorBox::__ReconstructDialog(HDC dc)
{
    GRect rect = m_rect;

    rect.Height = 90;
    m_popDialogWidth = 180;
    rect.Width = m_popDialogWidth;
    std::unique_lock<std::mutex> lk(createLK);
    if (!m_cpDialog) {
        m_cpDialog = new QColorPopDialog(dc, rect);
        ++__currentCount;
        GRect rect2 = rect;
        rect2.Height = rect.Height / 4.0;
        rect2.X = 0;
        rect2.Y = 0;
        if (!m_redQslider) {
            m_redQslider = new QSlider(dc, rect2);
        }
        m_redQslider->SetValueChangeEvent(sliderVChange);
        m_redQslider->RecordAttribute(0, (long long)this);//��¼һ������
        m_redQslider->SetColor(0xffff0000);
        m_redQslider->SetRailColor(0xffff0000);
        m_cpDialog->AttachCtrl(m_redQslider);
        rect2.Y = rect2.Height;
        if (!m_greeQslider) {
            m_greeQslider = new QSlider(dc, rect2);
        }
        m_greeQslider->SetValueChangeEvent(sliderVChange);
        m_greeQslider->RecordAttribute(0, (long long)this);//��¼һ������
        m_greeQslider->SetColor(0xff00ff00);
        m_greeQslider->SetRailColor(0xff00ff00);
        m_cpDialog->AttachCtrl(m_greeQslider);
        rect2.Y = rect2.Height * 2;
        if (!m_blueQslider) {
            m_blueQslider = new QSlider(dc, rect2);
        }
        m_blueQslider->SetValueChangeEvent(sliderVChange);
        m_blueQslider->RecordAttribute(0, (long long)this);//��¼һ������
        m_blueQslider->SetColor(0xff0000ff);
        m_blueQslider->SetRailColor(0xff0000ff);
        m_cpDialog->AttachCtrl(m_blueQslider);
        rect2.Y = rect2.Height * 3;
        if (!m_alphaQslider) {
            m_alphaQslider = new QSlider(dc, rect2);
        }
        m_alphaQslider->SetValueChangeEvent(sliderVChange);
        m_alphaQslider->RecordAttribute(0, (long long)this);//��¼һ������
        m_cpDialog->AttachCtrl(m_alphaQslider);

    }

   

}

QColorBox::QColorBox(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
   
    __ReconstructDialog(winddc);
}

int QColorBox::LButtonDown(int x, int y, WPARAM wParam)
{
    m_alphaQslider->SetCurrentValue((m_color>>24)/255.0*100);
    m_redQslider->SetCurrentValue(((m_color >> 16) &0xff) / 255.0 * 100);
    m_greeQslider->SetCurrentValue(((m_color >> 8) & 0xff) / 255.0 * 100);
    m_blueQslider->SetCurrentValue(((m_color) & 0xff) / 255.0 * 100);
    //�õ���ǰ�ؼ��ֶ��ڿͻ���λ��
    QDWORD pt = (QDWORD(m_rect.X)<<32)|(QDWORD(m_rect.Y));

    m_Parent->TransferMessage(QM_TO_CLIENT_XY, (QDWORD)&pt, 0, 0);

    //���õ��ͻ�ȥ��Сд
    QDWORD size=0;
    m_Parent->TransferMessage(QM_GET_CLIENT_SIZE, (QDWORD)&size, 0, 0);
    GRect drect = m_cpDialog->GetRect();
    drect.Width = m_popDialogWidth;
    drect.Height = m_popDialogHeight;
    int w = (size >> 32) & 0xffffffff;
    int h = size  & 0xffffffff;

    int cx = pt >> 32;
    int cy = pt &0xffffffff;

    int px = cx;
    int py = cy;

    //�ж��±��Ƿ��㹻��ʾ�������
    if (py + m_rect.Height+ drect.Height > h) {
        
        py = h - drect.Height;
    }
    else {
        py = cy + m_rect.Height;
    
    }







    //�ұ��Ƿ���߽�
    if (w < px + drect.Width) {
        px = w- drect.Width;
    }
    //�������
    if(0 > px){
        px = 0;
       
    }
    //�±��Ƿ���߽�
    if (h < py + drect.Height) {
        py = h - drect.Height;
    }
    //�ϱ�����
    if (0 > py) {
        py = 0;

    }



    {
        GRect Rect(px, py, m_popDialogWidth, m_popDialogHeight);
  
        m_cpDialog->Reconstruct(m_dc, Rect);
        m_cpDialog->m_attchObj = this;
     
        GRect rect = Rect;
        GRect rect2 = rect;
        rect2.Height = rect.Height / 4.0;
        rect2.X = 0;
        rect2.Y = 0;
        m_redQslider->Reconstruct(m_dc, rect2);
        rect2.Y = rect2.Height;
        m_greeQslider->Reconstruct(m_dc, rect2);
        rect2.Y = rect2.Height * 2;
        m_blueQslider->Reconstruct(m_dc, rect2);
        rect2.Y = rect2.Height * 3;
        m_alphaQslider->Reconstruct(m_dc, rect2);

        m_cpDialog->SetLocation(GPoint(px, py));

        m_alphaQslider->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
    }
    m_redQslider->RecordAttribute(0, (long long)this);//��¼һ������
    m_greeQslider->RecordAttribute(0, (long long)this);//��¼һ������
    m_blueQslider->RecordAttribute(0, (long long)this);//��¼һ������
    m_alphaQslider->RecordAttribute(0, (long long)this);//��¼һ������
    QColorBox* p = (QColorBox*)m_cpDialog;
    p->m_Parent = this;
    p->m_HostHwnd = m_HostHwnd;
    m_cpDialog->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
    m_Parent->TransferMessage(QM_SET_POPDIALOG, (QDWORD)m_cpDialog,0,0);
    return 0;
}

int sliderVChange(QPOINTER ct, float v)
{
    QSlider* p = (QSlider*)ct;

    long long obj = 0;
    p->FindAttribute(0,&obj);
    if (!obj)return 0;

    QColorBox* qcb = (QColorBox*)obj;
    unsigned int color = qcb->GetColor();

    if (p == qcb->m_blueQslider) {
        //��ɫ�ı�
        int c = int(255 * v/100);
        color &= 0xffffff00;
        color |= c & 0xff;
    }
    else if (p == qcb->m_redQslider) {
        //��ɫ�ı�
        int c = int(255 * v / 100)<<16;
        color &= 0xff00ffff;
        color |= c & 0x00ff0000;
    }
    else if (p == qcb->m_greeQslider) {
        //��ɫ�ı�
        int c = int(255 * v / 100) << 8;
        color &= 0xffff00ff;
        color |= c & 0x0000ff00;
    }
    else if (p == qcb->m_alphaQslider) {
        //��ɫ�ı�
        int c = int(255 * v / 100) << 24;
        color &= 0x00ffffff;
        color |= c & 0xff000000;
    }
    qcb->SetColor(color);

    if (qcb->m_colorChange) {
        qcb->m_colorChange((QPOINTER)qcb, color);
    }
    return 1;
}

unsigned int QColorBox::GetColor()
{
    return m_color;
}

unsigned int QColorBox::SetColor(unsigned int color)
{
    unsigned int buf = m_color;
    m_color = color;
    isChange = true;
    if(m_Parent)m_Parent->TransferMessage(QM_UPDATE_CTRL,1,0,0);
    return buf;
}

QCOLORBOX_CHANGE QColorBox::SetColorChangeEvent(QCOLORBOX_CHANGE ev)
{
    QCOLORBOX_CHANGE buf = m_colorChange;
    m_colorChange = ev;
    return buf;
}

void QColorBox::SetPopDialogHeight(int h)
{
    m_popDialogHeight = h;
    //GRect rect = m_cpDialog->GetRect();
    //if (rect.Height == h)return;
    //rect.Height = h;
    //m_cpDialog->Reconstruct(nullptr, rect);
    //GRect rect2 = rect;
    //rect2.Height = rect.Height / 4.0;
    //rect2.X = 0;
    //rect2.Y = 0;
    //m_redQslider->Reconstruct(m_dc, rect2);
    //rect2.Y = rect2.Height;
    //m_greeQslider->Reconstruct(m_dc, rect2);
    //rect2.Y = rect2.Height*2;
    //m_blueQslider->Reconstruct(m_dc, rect2);
    //rect2.Y = rect2.Height * 3;
    //m_alphaQslider->Reconstruct(m_dc, rect2);
}

void QColorBox::SetPopDialogWidth(int w)
{
    m_popDialogWidth = w;
}

int QColorBox::Draw(HDC sdc)
{
    int ret = 0;
    if (isChange) {
        isChange = false;
        ret = 1;
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
        ID2D1Factory* pFactory = m_pD2DFactory;

        RECT rc(0, 0, m_rect.Width, m_rect.Height);
        pRenderTarget->BindDC(m_dc, &rc);//��DC���
        pRenderTarget->BeginDraw();
        //{
        //    ID2D1SolidColorBrush* ps = nullptr;
        //    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xffffff,1.0), &ps);
        //



        //    D2D1_RECT_F rect(0,0 , m_rect.Width, m_rect.Height);
        //    // ����Բ�Ǿ��εĲ���
      
        //    pRenderTarget->FillRectangle(rect, ps);
        //
        //    ps->Release();
        //
        //}


        //������ɫ
        {
            ID2D1SolidColorBrush* ps = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF((m_color)&0xffffff,((m_color>>24)&0xff)/255.0) , &ps);
            float ra = m_rect.Width * 0.2;
           
            if (m_rect.Width < m_rect.Height) {
                ra = m_rect.Height * 0.2;
                if (ra > m_rect.Width)ra = m_rect.Width;
            }
            else {
                if (ra > m_rect.Height)ra = m_rect.Height;
            }
           
          
            float ry = (m_rect.Height - ra) / 2.0;
            D2D1_RECT_F rect(0, ry, ra, ry +ra);
            // ����Բ�Ǿ��εĲ���
            D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                rect, // ����λ�úʹ�С
                ra*0.2, ra * 0.2                              // Բ�ǰ뾶��x, y��
            );
            //pRenderTarget->FillRectangle(rect, ps);
            pRenderTarget->FillRoundedRectangle(roundedRect, ps);
            ps->Release();
        
        }
        //��������

        {
            std::wstring text = std::format(L"#{:x}", m_color);
            DWRITE_TEXT_METRICS textMetrics;
            IDWriteTextLayout* pTextLayout = nullptr;
            m_pDWriteFactory->CreateTextLayout(
                text.c_str(),            // Ҫ�������ı�
                text.length(),      // �ı�����
                pTextFormat,     // �ı���ʽ
                1000.0f,         // ���õ������
                500.0f,          // ���õ����߶�
                &pTextLayout     // �����TextLayout����
            );
            pTextLayout->GetMetrics(&textMetrics);
            pTextLayout->Release();
            float textWidth = textMetrics.width;        // �ı���ʵ�ʿ��
            float textHeight = textMetrics.height;      // �ı���ʵ�ʸ߶�

            D2D1_RECT_F rect(m_rect.Width- textWidth, 0, m_rect.Width, m_rect.Height);
            ID2D1SolidColorBrush* pfBrush = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF((m_color) & 0xffffff, 1.0), &pfBrush);
            //�����ַ�
            pRenderTarget->DrawText(
                text.c_str(),
                text.length(),
                pTextFormat,             // The text format
                rect,              // The layout rectangle
                pfBrush                   // The brush

            );
            pfBrush->Release();
        
        }
        

        pRenderTarget->EndDraw();
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}

QColorBox::~QColorBox()
{
    std::unique_lock<std::mutex> lk(createLK);
    if (__currentCount < 1) {
        if (m_cpDialog) {
            delete m_cpDialog;
            m_cpDialog = nullptr;
        }
        if (m_redQslider) {
            delete m_redQslider;
            m_redQslider = nullptr;
        }
        if (m_greeQslider){
            delete m_greeQslider;
            m_greeQslider = nullptr;
        }
        if (m_blueQslider) {
            delete m_blueQslider;
            m_blueQslider = nullptr;
        }
        if (m_alphaQslider) {
            delete m_alphaQslider;
            m_alphaQslider = nullptr;
        }
    }
    --__currentCount;
}

QColorBox::QColorPopDialog::QColorPopDialog(HDC winddc, const GRect& rf) :QPopDialog(winddc, rf)
{
}

QColorBox::QColorPopDialog::~QColorPopDialog()
{
}

QDWORD QColorBox::QColorPopDialog::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_POPDIALOG_MOUSE_OUT: {

    }break;
    
    }

    return QPopDialog::TransferMessage(msgID, LParam, RParam, AddParam);
}


#endif // !__QCOLORBOX__

extern "C"
{
#define __EXTERN_FUN(name) QCOLORB##name

    __declspec(dllexport)  unsigned int __EXTERN_FUN(GetColor)(QColorBox* p)
    {
        return p->GetColor();
    }
    __declspec(dllexport)  unsigned int __EXTERN_FUN(SetColor)(QColorBox* p, unsigned int color)
    {
        return p->SetColor(color);
    }
    __declspec(dllexport)  QCOLORBOX_CHANGE __EXTERN_FUN(SetColorChangeEvent)(QColorBox* p, QCOLORBOX_CHANGE ev)
    {
        return p->SetColorChangeEvent(ev);
    }
    __declspec(dllexport)  void __EXTERN_FUN(SetPopDialogHeight)(QColorBox* p, int h)
    {
        return p->SetPopDialogHeight(h);
    }
    __declspec(dllexport)  void __EXTERN_FUN(SetPopDialogWidth)(QColorBox* p, int w)
    {
        return p->SetPopDialogWidth(w);
    }
}

#ifdef __QSELECT__

QSelect::QSelectPopDialog* QSelect::m_spdialog = nullptr;
std::mutex QSelect::createLK;
int QSelect::__currentCount =0 ;
QSelect::QSelect(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
    m_popDialogHeight = 90;
    m_popDialogWidth = rf.Width;
    GRect rect(0,0, m_popDialogWidth, m_popDialogHeight);
    std::unique_lock<std::mutex> lk(createLK);
    if (!m_spdialog) {
        m_spdialog = new QSelectPopDialog(winddc, rect);
        ++__currentCount;
    }


    ////���Դ���
    //PopListObj obj;
    //obj.text = L"text";
    //m_list.push_back(obj);
    //obj.text = L"text1";
    //m_list.push_back(obj);
    //obj.text = L"����̫��";
    //m_list.push_back(obj);
    //obj.text = L"text2";
    //obj.color1 = 0xffa000c0;
    //m_list.push_back(obj);
}

int QSelect::Draw(HDC sdc)
{

    int ret = 0;
    if (isChange) {
        isChange = false;
        ret = 1;
        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        ID2D1DCRenderTarget* pRenderTarget = m_pRenderTarget;
        ID2D1Factory* pFactory = m_pD2DFactory;

        RECT rc(0, 0, m_rect.Width, m_rect.Height);
        pRenderTarget->BindDC(m_dc, &rc);//��DC���
        pRenderTarget->BeginDraw();



        //���Ʒ������
        {

            D2D1_RECT_F rect(0, 0, m_rect.Width, m_rect.Height);
            float corner_radius = (std::min)(m_rect.Width, m_rect.Height) / 9.0;
            float max_radius = (std::min)(m_rect.Width, m_rect.Height) / 2.0;
            float radius = (std::min)(corner_radius, max_radius);


            // ����Բ�Ǿ��εĲ���
            D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                rect, // ����λ�úʹ�С
                radius, radius                              // Բ�ǰ뾶��x, y��
            );

            {//���Ʊ���
                ID2D1SolidColorBrush* psl = nullptr;
                pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_bkColor & 0xffffff, ((m_bkColor >> 24) & 0xff) / 255.0), &psl);
                D2D1_RECT_F rect(0, 0, m_rect.Width, m_rect.Height);


                pRenderTarget->FillRoundedRectangle(roundedRect, psl);

                psl->Release();
            }

  
            if (m_currentItem) {
                //����Item����

                // ���彥��ֹͣ�� (��ɫ��λ��)
                D2D1_GRADIENT_STOP gradientStops[] = {
                    { 0.0f, D2D1::ColorF(m_currentItem->color1 & 0xffffff,((m_currentItem->color1 >> 24) & 0xff) / 255.0) },  // ��ʼ�Ļ�ɫ
                    { 1.0f, D2D1::ColorF(m_currentItem->color2 & 0xffffff,((m_currentItem->color2 >> 24) & 0xff) / 255.0) }      // �����ĺ�ɫ
                };
                // ��������ֹͣ����
                ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
                pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    ARRAYSIZE(gradientStops),
                    &pGradientStopCollection);

                ID2D1LinearGradientBrush* pLinearGradientBrush = nullptr;

                D2D1_POINT_2F pt1(0, 0), pt2(rc.right, 0);
                if (1 == m_currentItem->colorD) {
                    pt2.x = 0;
                    pt2.y = m_rect.Height;
                }

                // �������Խ��仭ˢ
                pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        pt1,              // ���
                        pt2 // �յ�
                    ),
                    pGradientStopCollection,
                    &pLinearGradientBrush);

                   pRenderTarget->FillRoundedRectangle(roundedRect, pLinearGradientBrush);

                pGradientStopCollection->Release();
                pLinearGradientBrush->Release();
            }
            ID2D1SolidColorBrush* psl = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0.8), &psl);
            pRenderTarget->DrawRoundedRectangle(roundedRect, psl);
            psl->Release();


        }
        //���������������ť
        
        {
            if (m_currentItem) {
                DWRITE_TEXT_METRICS textMetrics;
                IDWriteTextLayout* pTextLayout = nullptr;
                m_pDWriteFactory->CreateTextLayout(
                    m_currentItem->text.c_str(),            // Ҫ�������ı�
                    m_currentItem->text.length(),      // �ı�����
                    pTextFormat,     // �ı���ʽ
                    m_rect.Width,         // ���õ������
                    m_rect.Height,          // ���õ����߶�
                    &pTextLayout     // �����TextLayout����
                );
                pTextLayout->GetMetrics(&textMetrics);
                pTextLayout->Release();
                float textWidth = textMetrics.width;        // �ı���ʵ�ʿ��
                float textHeight = textMetrics.height;      // �ı���ʵ�ʸ߶�

               //���ֵľ���
                D2D1_RECT_F frect(0, 0, m_rect.Width * (1 - m_btWato), m_rect.Height);
                D2D1_POINT_2F origin((frect.left + frect.right) / 2.0, (frect.top + frect.bottom) / 2.0);

                D2D1_RECT_F Textrect(origin.x - textWidth / 2.0, origin.y - textHeight / 2.0, origin.x + textWidth / 2.0, origin.y + textHeight / 2.0);
      

                //����Item����
                

                // ���彥��ֹͣ�� (��ɫ��λ��)
                D2D1_GRADIENT_STOP gradientStops[] = {
                    { 0.0f, D2D1::ColorF(m_currentItem->fcolor1 & 0xffffff,((m_currentItem->fcolor1 >> 24) & 0xff) / 255.0) },  // ��ʼ�Ļ�ɫ
                    { 1.0f, D2D1::ColorF(m_currentItem->fcolor2 & 0xffffff,((m_currentItem->fcolor2 >> 24) & 0xff) / 255.0) }      // �����ĺ�ɫ
                };
                // ��������ֹͣ����
                ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
                pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    ARRAYSIZE(gradientStops),
                    &pGradientStopCollection);

                ID2D1LinearGradientBrush* pLinearGradientBrush = nullptr;

                D2D1_POINT_2F pt1(Textrect.left, 0), pt2(Textrect.right, 0);
                if (1 == m_currentItem->fcolorD) {
                    pt1.x = 0;
                    pt1.y = Textrect.top;

                    pt2.x = 0;
                    pt2.y = Textrect.bottom;
                }

                // �������Խ��仭ˢ
                pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        pt1,              // ���
                        pt2 // �յ�
                    ),
                    pGradientStopCollection,
                    &pLinearGradientBrush);

        
                //�����ַ�
                pRenderTarget->DrawText(
                    m_currentItem->text.c_str(),
                    m_currentItem->text.length(),
                    pTextFormat,             // The text format
                    frect,              // The layout rectangle
                    pLinearGradientBrush                   // The brush

                );
                pGradientStopCollection->Release();
                pLinearGradientBrush->Release();
         
            }

            //����������
            ID2D1PathGeometry* pPathGeometry = nullptr;
            HRESULT hr = m_pD2DFactory->CreatePathGeometry(&pPathGeometry);
            if (!FAILED(hr)) {
                ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
                D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES gradientProperties =
                {
                    D2D1::Point2F(150, 100), // ���ĵ�����
                    D2D1::Point2F(0, 0), // ����ԭ��ƫ����
                    200, // X����뾶
                    200  // Y����뾶
                };
                // ��������ֹͣ������
                D2D1_GRADIENT_STOP gradientStops[2];
                gradientStops[0].position = 0.0f;
                gradientStops[0].color = D2D1::ColorF(0x000055, 0.2);

                gradientStops[1].position = 1.0f;
                gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Blue);

                // ��������ֹͣ�㼯��
                ID2D1GradientStopCollection* pGradientStops = nullptr;
                HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    2, // ����ֹͣ�������
                    &pGradientStops
                );
                m_pRenderTarget->CreateRadialGradientBrush(&gradientProperties, NULL, pGradientStops, &pRadialGradientBrush);
  
                ID2D1GeometrySink* pSink = nullptr;
                hr = pPathGeometry->Open(&pSink);

                D2D1_RECT_F TriangRect(m_rect.Width*(1- m_btWato),0, m_rect.Width, m_rect.Height);
                float cx = (TriangRect.left+ TriangRect.right)/2.0, cy = (TriangRect.top+ TriangRect.bottom)/2.0;

                //����Բ�İ뾶
                float radius = (std::min)((double)m_rect.Width* m_btWato, (double)m_rect.Height)/2.0;


                if (m_isOpen) {
                    pSink->BeginFigure(D2D1::Point2F(cx - radius * std::sin(60 * std::numbers::pi / 180.0), cy - radius * std::cos(60 * std::numbers::pi / 180.0)), D2D1_FIGURE_BEGIN_FILLED);
                    pSink->AddLine(D2D1::Point2F(cx + radius * std::sin(60 * std::numbers::pi / 180.0), cy - radius * std::cos(60 * std::numbers::pi / 180.0)));
                    pSink->AddLine(D2D1::Point2F(cx, cy + radius));
                    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                }
                else {
                  
                    pSink->BeginFigure(D2D1::Point2F(cx - radius, cy), D2D1_FIGURE_BEGIN_FILLED);
                    pSink->AddLine(D2D1::Point2F(cx + radius * std::cos(60 * std::numbers::pi / 180.0), cy -radius * std::sin(60 * std::numbers::pi / 180.0)));
                    pSink->AddLine(D2D1::Point2F(cx + radius * std::cos(60 * std::numbers::pi / 180.0), cy + radius * std::sin(60 * std::numbers::pi / 180.0)));

               /*     pSink->BeginFigure(D2D1::Point2F(cx - radius * std::cos(60 * std::numbers::pi / 180.0), cy - radius * std::sin(60 * std::numbers::pi / 180.0)), D2D1_FIGURE_BEGIN_FILLED);
                    pSink->AddLine(D2D1::Point2F(cx - radius * std::cos(60 * std::numbers::pi / 180.0), cy + radius * std::sin(60 * std::numbers::pi / 180.0)));
                    pSink->AddLine(D2D1::Point2F(cx + radius, cy));*/
                    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                }
            
                hr = pSink->Close();
                pSink->Release();
                pRadialGradientBrush->SetCenter(D2D1::Point2F(cx, cy));
                pRadialGradientBrush->SetRadiusX(radius);
                pRadialGradientBrush->SetRadiusY(radius);
                pRenderTarget->FillGeometry(pPathGeometry, pRadialGradientBrush);
                pPathGeometry->Release();
                pRadialGradientBrush->Release();
                pGradientStops->Release();
              
            }



        }

        pRenderTarget->EndDraw();
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}

QSelect::~QSelect()
{
    std::unique_lock<std::mutex> lk(createLK);
    if (__currentCount < 1) {
        delete m_spdialog;
        m_spdialog = nullptr;
    
    }

}
int QSelect::LButtonDown(int x, int y, WPARAM wParam)
{
   
    if ((this != m_spdialog->m_attchObj)) {
        {
            std::shared_mutex* pLK = m_spdialog->m_LK_list;
            if (pLK) {
                std::unique_lock<std::shared_mutex> LK(*pLK);
            }
            if (!m_isOpen) {
                m_spdialog->m_attchObj = this;
                ((QSelect*)m_spdialog)->m_Parent = this;
                ((QSelect*)m_spdialog)->m_HostHwnd = m_HostHwnd;
                QSelect* pcp = (QSelect*)m_spdialog;
                if (pcp->m_fontSize != m_itemFSize || pcp->m_fontName != m_itemFName) {
                    pcp->m_fontSize = m_itemFSize;
                    pcp->SetFontName(m_itemFName.c_str());
                }


                m_spdialog->m_LK_list.store(&m_LK_list);
                m_spdialog->m_list.store(&m_list);
                bool* open=m_spdialog->m_isOpen.load();
                if (open) {
                    *open = false;
                }
                m_spdialog->m_isOpen.store(&m_isOpen);
                //�õ���ǰ�ؼ��ֶ��ڿͻ���λ��
                QDWORD pt = (QDWORD(m_rect.X) << 32) | (QDWORD(m_rect.Y));

                m_Parent->TransferMessage(QM_TO_CLIENT_XY, (QDWORD)&pt, 0, 0);
                int cx = pt >> 32;
                int cy = pt & 0xffffffff;

                GRect drect(cx, cy + m_rect.Height, m_popDialogWidth, m_popDialogHeight);
                m_spdialog->Reconstruct(m_dc, drect);
                m_spdialog->SetLocation(GPoint(cx, cy + m_rect.Height));
                m_spdialog->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                m_spdialog->m_offsetX.store(&m_offsetX);
                m_spdialog->m_offsetY.store(&m_offsetY);
                m_spdialog->RecordAttribute(0, (long long)this);//��¼һ������
                m_spdialog->m_index = -1;
                m_spdialog->m_itemHeight = m_itemHeight;
                m_spdialog->m_selectItem = SelsctChange;


              
                m_isOpen = true;
            }
 /*           else {
                m_isOpen = false;
                m_spdialog->m_attchObj = nullptr;
            }*/
        }
        m_Parent->TransferMessage(QM_SET_POPDIALOG, QDWORD(m_spdialog), 0, 0);

    }
    else {
        m_Parent->TransferMessage(QM_SET_POPDIALOG, 0, 0, 0);
        m_isOpen = false;
        m_spdialog->m_attchObj = nullptr;
        //if (!m_isOpen) {
        //    m_Parent->TransferMessage(QM_SET_POPDIALOG, QDWORD(m_spdialog), 0, 0);
        //    m_isOpen = true;
        //}
        //else {
        //    m_Parent->TransferMessage(QM_SET_POPDIALOG,0, 0, 0);
        //    m_isOpen = false;
        //}
        //std::shared_mutex* pLK = m_spdialog->m_LK_list;
        //if (pLK) {
        //    std::unique_lock<std::shared_mutex> LK(*pLK);
        //}
        //m_Parent->TransferMessage(QM_SET_POPDIALOG, 0, 0, 0);
        //m_spdialog->m_attchObj = nullptr;

    }
  
    isChange = true;
    return 0;
}
QSelect::QSelectPopDialog::QSelectPopDialog(HDC winddc, const GRect& rf) :QPopList(winddc, rf)
{

}

QSelect::QSelectPopDialog::~QSelectPopDialog()
{
}




int SelsctChange(QPOINTER ct, int v)
{
    QSelect::QSelectPopDialog* pspd = (QSelect::QSelectPopDialog*)ct;
    QDWORD ptr = 0;
    pspd->FindAttribute(0, &ptr);
    if (!ptr)return 0;

    QSelect* ps = (QSelect*)ptr;
    int i = -1;

    for (auto& it : ps->m_list) {
        if (++i == v) {
            //ps->m_text = it.text;
            ps->m_currentItem = &it;
            ps->m_index = v;
            break;
        }

    }
    if (ps->m_selectEvent)ps->m_selectEvent((QPOINTER)ps, v);
    ps->Redraw();
    ps->UpdateWND();

    return 1;
}

void QSelect::Reconstruct(HDC sdc, const GRect& rect)
{
    QCtrlBaseD2DIPD::Reconstruct(sdc, rect);
    m_popDialogWidth = rect.Width;

}

void QSelect::SetPopDialogHeight(int h)
{
    if (h < 1)return;
    m_popDialogHeight = h;
}

void QSelect::SetPopDialogWidth(int w)
{
    if (w < 1)return;
    m_popDialogWidth = w;
}

int QSelect::InsertItem(const wchar_t* text, int index)
{
    std::unique_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index = m_list.size();
    if (index > m_list.size())return -1;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    PopListObj obj;
    obj.text = text;
    m_list.insert(it, obj);
    return 1;
}

int QSelect::RemoveItem(int index)
{
    std::unique_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)return -1;
    if (index > m_list.size() - 1)return -2;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    m_list.erase(it);
    if (m_index == index) {
        m_currentItem = nullptr;
    }
    if (m_index > index) {
        m_index = index-1;
        if (m_index > -1) {
            auto it = m_list.begin();
            std::advance(it, m_index); 
            m_currentItem = &(*it);
         
        }
    }
    return 1;
}

void QSelect::SetButtonRatio(float rat)
{
    m_btWato = rat;
}

void QSelect::SetItemHeight(float h)
{
    m_itemHeight = h;
}

void QSelect::SetItemFontName(const wchar_t* nm)
{
    m_itemFName = nm;
}

void QSelect::SetItemFontSize(float size)
{
    m_itemFSize = size;
}

const wchar_t* QSelect::GetSelectedText()
{
    if (!m_currentItem)return L"";

    return   m_currentItem->text.c_str();
}

const wchar_t* QSelect::GetItemText(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)return nullptr;
    if (index > m_list.size() - 1)return nullptr;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������

    return it->text.c_str();
}

void QSelect::SetItemBKColor(int index, unsigned int c1, unsigned int c2, unsigned char d)
{
    std::unique_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index = m_list.size()-1;
    if (index > m_list.size() - 1 || index < 0)return;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    it->color1 = c1;
    it->color2 = c2;
    it->colorD = d;


}

void QSelect::SetItemFontColor(int index, unsigned int c1, unsigned int c2, unsigned char d)
{
    std::unique_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index = m_list.size()-1;
    if (index > m_list.size() - 1 || index < 0)return;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    it->fcolor1 = c1;
    it->fcolor2 = c2;
    it->fcolorD = d;
}

QSELECT_SELECTCHANGE QSelect::SetSelectChangeEvent(QSELECT_SELECTCHANGE ev)
{
    QSELECT_SELECTCHANGE buf = m_selectEvent;
    m_selectEvent = ev;
    return buf;
}

size_t QSelect::GetItemCount()
{
    std::shared_lock<std::shared_mutex> lk(m_LK_list);
    return m_list.size();
}

int QSelect::GetIndex()
{
    return m_index;
}

int QSelect::GetItemFontColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2)
{
    std::shared_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index = m_list.size() - 1;
    if (index > m_list.size() - 1 || index < 0)return -2;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    if (dire) {
        *dire = it->colorD;
    }
    if (pc1) {
        *pc1 = it->color1;
    }
    if (dire) {
        *pc2 = it->color2;
    }
    return 1;
}

int QSelect::GetItemBKColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2)
{
    std::shared_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index = m_list.size() - 1;
    if (index > m_list.size() - 1 || index < 0)return -2;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    if (dire) {
        *dire = it->fcolorD;
    }
    if (pc1) {
        *pc1 = it->fcolor1;
    }
    if (dire) {
        *pc2 = it->fcolor2;
    }
    return 1;
}

int QSelect::SetCurrentItem(int index)
{
    std::shared_lock<std::shared_mutex> lk(m_LK_list);
    if (index < 0)index= m_list.size()-1;
    if (index > m_list.size() - 1 || index < 0)return -2;
    auto it = m_list.begin();
    std::advance(it, index); // �ƶ�������
    //m_text = it->text;
    m_currentItem = &(*it);
    m_index = index;
    isChange = true;
    UpdateWND();
    return 1;
}



#endif // __QSELECT__

extern "C"
{
#define __EXTERN_FUN(name) QSELECT##name

    //���õ���߶�
    __declspec(dllexport) void __EXTERN_FUN(SetPopDialogHeight)(QSelect* p, int h)
    {
        return p->SetPopDialogHeight(h);

    }
    //���õ�����
    __declspec(dllexport) void __EXTERN_FUN(SetPopDialogWidth)(QSelect* p, int w)
    {
        return p->SetPopDialogWidth(w);
    }
    //����һ����Ŀ
    __declspec(dllexport) int __EXTERN_FUN(InsertItem)(QSelect* p, const wchar_t* text, int index)
    {
        return p->InsertItem(text, index);
    }
    //�Ƴ�һ����Ŀ
    __declspec(dllexport) int __EXTERN_FUN(RemoveItem)(QSelect* p, int index)
    {
        return p->RemoveItem(index);
    }
    //����������ť�Ŀ�ȱ���
    __declspec(dllexport) void __EXTERN_FUN(SetButtonRatio)(QSelect* p, float rat)
    {
        return p->SetButtonRatio(rat);
    }
    //������Ŀ�߶�
    __declspec(dllexport) void __EXTERN_FUN(SetItemHeight)(QSelect* p, float h)
    {
        return p->SetItemHeight(h);
    }
    //������Ŀ��������
    __declspec(dllexport) void __EXTERN_FUN(SetItemFontName)(QSelect* p, const wchar_t* nm)
    {
        return p->SetItemFontName(nm);
    }
    //������Ŀ�����С
    __declspec(dllexport) void __EXTERN_FUN(SetItemFontSize)(QSelect* p, float size)
    {
        return p->SetItemFontSize(size);
    }
    //��ȡѡ�е���Ŀ�ı�
    __declspec(dllexport) const wchar_t* __EXTERN_FUN(GetSelectedText)(QSelect* p)
    {
        return p->GetSelectedText();
    }
    //��ȡ��Ŀ�ı�
    __declspec(dllexport) const wchar_t* __EXTERN_FUN(GetItemText)(QSelect* p, int index)
    {
        return p->GetItemText(index);
    }
    //����ָ����Ŀ������ɫ
    __declspec(dllexport) void __EXTERN_FUN(SetItemBKColor)(QSelect* p, int index, unsigned int c1, unsigned int c2, unsigned char d = 0)
    {
        return p->SetItemBKColor(index, c1, c2, d);
    }
    //����ָ����Ŀ������ɫ
    __declspec(dllexport) void __EXTERN_FUN(SetItemFontColor)(QSelect* p, int index, unsigned int c1, unsigned int c2, unsigned char d = 0)
    {
        return p->SetItemFontColor(index, c1, c2, d);
    }
    //������Ŀ��ѡ��ʱ�ص�����
    __declspec(dllexport) QSELECT_SELECTCHANGE __EXTERN_FUN(SetSelectChangeEvent)(QSelect* p, QSELECT_SELECTCHANGE ev)
    {
        return p->SetSelectChangeEvent(ev);
    }
    //��ȡ��Ŀ��
    __declspec(dllexport) size_t __EXTERN_FUN(GetItemCount)(QSelect* p)
    {
        return p->GetItemCount();
    }
    //���õ�ǰѡ����Ŀ
    __declspec(dllexport) int  __EXTERN_FUN(SetCurrentItem)(QSelect* p, int index)
    {
        return p->SetCurrentItem(index);
    }

    //��ȡ��ǰѡ������
    __declspec(dllexport) int __EXTERN_FUN(GetIndex)(QSelect* p)
    {
        return p->GetIndex();
    }
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    __declspec(dllexport) int __EXTERN_FUN(GetItemFontColor)(QSelect* p,int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2)
    {
        return p->GetItemFontColor(index, dire, pc1, pc2);
    }
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    __declspec(dllexport) int __EXTERN_FUN(GetItemBKColor)(QSelect* p,int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2)
    {
        return p->GetItemBKColor(index, dire, pc1, pc2);
    }


}
