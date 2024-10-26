#include "QFPButton.h"

QPButton::QPButton(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
    //LoadBitmapFromFile(L"./Tabico.png", m_pRenderTarget, m_pWICFactory, &pBitmap);
}

QPButton::~QPButton()
{
    if (pBitmap) {
        pBitmap->Release();
        pBitmap = nullptr;
    
    }
}

int QPButton::Draw(HDC sdc)
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
        //ID2D1SolidColorBrush* pBrush;
        //pRenderTarget->CreateSolidColorBrush(
        //    D2D1::ColorF(D2D1::ColorF::SkyBlue),
        //    &pBrush
        //);
        // //����
        //pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        //pBrush->Release();
        

        // ���� PathGeometry ����
        ID2D1PathGeometry* pPathGeometry = nullptr;
        pFactory->CreatePathGeometry(&pPathGeometry);

        // ���� GeometrySink
        ID2D1GeometrySink* pSink = nullptr;
        pPathGeometry->Open(&pSink);

        float xRadius = __xRadius, yRadius = __yRadius;

        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (xRadius > m_rect.Width /2.0 - __margin) {
            xRadius = m_rect.Width / 2.0 - __margin;
        
        }
        if (yRadius > m_rect.Height / 2.0 - __margin) {
            yRadius = m_rect.Height / 2.0 - __margin;

        }
        // ����Բ�Ǿ��εĲ���
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width- __margin, m_rect.Height- __margin), // ����λ�úʹ�С
            xRadius, yRadius                              // Բ�ǰ뾶��x, y��
        );

        // ʹ�� Sink ��ʼ����Բ�Ǿ���
        pSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // ���
            D2D1_FIGURE_BEGIN_FILLED
        );

        // ��������ͻ���������Բ�Ǿ���
        pSink->AddLine(D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.top));
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.top + roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pSink->AddLine(D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.bottom - roundedRect.radiusY));
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.bottom),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pSink->AddLine(D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.bottom));
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.bottom - roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pSink->AddLine(D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.top + roundedRect.radiusY));
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));

        // �������ر�·��
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pSink->Close();

   

        //pRenderTarget->CreateSolidColorBrush(
        //    D2D1::ColorF(D2D1::ColorF::Red),
        //    &pPBrush
        //);
        D2D1_RECT_F oPathBounds;
        pPathGeometry->GetBounds(D2D1::Matrix3x2F::Identity(), &oPathBounds);
        std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

        long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now- __startTime).count();
        long long cyc = 300;
        if(dta< cyc)
        {

            float xd = m_rect.Width-oPathBounds.right;
            float yd = m_rect.Height - oPathBounds.bottom;
            float mind = (std::min)(xd,yd);

            D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Scale(__ShrinkageRatio, __ShrinkageRatio, D2D1::Point2F(m_rect.Width / 2.0, m_rect.Height / 2.0));
            // 3. �����任��ļ���
            ID2D1TransformedGeometry* pTransformedGeometry = nullptr;
            pFactory->CreateTransformedGeometry(pPathGeometry, scaleMatrix, &pTransformedGeometry);
            // 2. ��ȡ·���ı߽����
            D2D1_RECT_F pathBounds;
            pTransformedGeometry->GetBounds(D2D1::Matrix3x2F::Identity(), &pathBounds);
            //__ShrinkageRatio += 0.05;
            __ShrinkageRatio = 1.0+ (mind* dta / cyc)/(std::min)(oPathBounds.right- oPathBounds.left, oPathBounds.bottom- oPathBounds.top);
            D2D1_RECT_F outerRect(0, 0, m_rect.Width, m_rect.Height);
            bool isContained =
                pathBounds.left >= outerRect.left &&
                pathBounds.top >= outerRect.top &&
                pathBounds.right <= outerRect.right &&
                pathBounds.bottom <= outerRect.bottom;

            if (isContained) {
                // innerRect ��ȫ�� outerRect ��
                //OutputDebugString(L"Inner rectangle is contained within the outer rectangle.\n");
            }
            else {
                __ShrinkageRatio = 1.0;
                //�����˾Ͳ��ڲ�����
                std::chrono::seconds ten_seconds(10);
                __startTime = std::chrono::high_resolution_clock::now() - ten_seconds;
                // innerRect ����ȫ�� outerRect ��
                //OutputDebugString(L"Inner rectangle is NOT contained within the outer rectangle.\n");
            }
        

            //pRenderTarget->DrawGeometry(pTransformedGeometry, pPBrush);
            //pPBrush->Release();
            ID2D1RadialGradientBrush* pRadialGradientBrush = nullptr;
            ID2D1GradientStopCollection* pGradientStops = nullptr;

            D2D1_GRADIENT_STOP gradientStops[] = {
            { 0.0f, D2D1::ColorF(D2D1::ColorF::Black,0.3) },
            { 1.0f, D2D1::ColorF(__BKColor & 0xffffff,0.2) }
            };

            // ��������ֹͣ����
            pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                ARRAYSIZE(gradientStops),
                &pGradientStops
            );

            // �������򽥱仭ˢ
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientBrushProperties = {
                D2D1::Point2F(m_rect.Width / 2.0,m_rect.Height / 2.0), // ��������
                D2D1::Point2F(0,0), // �������
                m_rect.Width / 2.0, // �ڲ��뾶
                m_rect.Height / 2.0 // �ⲿ�뾶
            };

            pRenderTarget->CreateRadialGradientBrush(
                radialGradientBrushProperties,
                pGradientStops,
                &pRadialGradientBrush
            );
            pRenderTarget->FillGeometry(pTransformedGeometry, pRadialGradientBrush);

            pRadialGradientBrush->Release();
            pGradientStops->Release();






            pTransformedGeometry->Release();

            //֪ͨ��Ҫˢ��
            m_Parent->TransferMessage(QM_UPDATE_CTRL,1,0,0);
            isChange = true;
        }



        {
            ID2D1SolidColorBrush* pPBrush;
            pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(__BKColor&0xffffff,(__BKColor>>24)/255.0),
                &pPBrush
            );
            pRenderTarget->FillGeometry(pPathGeometry, pPBrush);
            pPBrush->Release();

            if (1 != m_Status) {
                //�����겻�ڿؼ��ϼ���һ���ɫ
                pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(0x000000,0.1),
                    &pPBrush
                );
                pRenderTarget->FillGeometry(pPathGeometry, pPBrush);
                pPBrush->Release();
            
            }

        
        }
        {
            ID2D1SolidColorBrush* pfBrush;
            pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(m_fontColor&0xffffff,(m_fontColor>>24)/255.0),
                &pfBrush
            );
            // ��������
            D2D1_RECT_F pathBounds;
            pPathGeometry->GetBounds(D2D1::Matrix3x2F::Identity(), &pathBounds);
            D2D1_RECT_F textBounds = pathBounds;

            D2D1_RECT_F picBounds = pathBounds;
            if (pBitmap) {
             
                float xrd = xRadius;
                float yrd = yRadius;
                xrd = 0;
                float tWidth = pathBounds.right - pathBounds.left - 2 * xrd;
                float tHeight = pathBounds.bottom - pathBounds.top;
                switch (__picType) {
                case 0: {

                    picBounds.left = xrd + pathBounds.left;
                    picBounds.right = xrd + tWidth*2 / 5.0 + pathBounds.left;
                    picBounds.top = pathBounds.top;
                    picBounds.bottom = pathBounds.bottom;
                    //�ַ��ı�����Ҳ������
                    textBounds.left = picBounds.right;
                    textBounds.right = textBounds.left + tWidth * 3/5.0;
                    textBounds.top = pathBounds.top;
                    textBounds.bottom = pathBounds.bottom;


                }
                    break;
                case 3: {

                    textBounds.left = xrd+ pathBounds.left;
                    textBounds.right = pathBounds.right - xrd;
                    textBounds.top = pathBounds.top;
                    textBounds.bottom = pathBounds.top+tHeight*0.5;

                    picBounds.left = xrd + pathBounds.left;
                    picBounds.right = pathBounds.right - xrd;
                    picBounds.top = textBounds.bottom;
                    picBounds.bottom = pathBounds.bottom;

                }
                    break;

                case 1:
                    picBounds.left = xrd + pathBounds.left;
                    picBounds.right = xrd + tWidth * 2 / 5.0 + pathBounds.left;
                    picBounds.top = pathBounds.top;
                    picBounds.bottom = pathBounds.bottom;
               
                    textBounds.left = picBounds.right;
                    textBounds.right = textBounds.left + tWidth * 3 / 5.0;
                    textBounds.top = pathBounds.top;
                    textBounds.bottom = pathBounds.bottom;


                    break;

                case 2:


                    picBounds.left = xrd + pathBounds.left;
                    picBounds.right = pathBounds.right - xrd;
                    picBounds.top = pathBounds.top;
                    picBounds.bottom = pathBounds.top + tHeight * 0.5;

                    textBounds.left = xrd + pathBounds.left;
                    textBounds.right = pathBounds.right - xrd;
                    textBounds.top = picBounds.bottom;
                    textBounds.bottom = pathBounds.bottom;
                    break;

                case 4:
     

                    break;


                }

                float rad = (std::min)(picBounds.right - picBounds.left, picBounds.bottom - picBounds.top) / 2.0;

         /*       float radx =(picBounds.right - picBounds.left) / 2.0;
               float rady = (picBounds.bottom - picBounds.top) / 2.0;*/
                //ͼ������
                ID2D1EllipseGeometry* pEllipseGeometry = NULL;
                ID2D1Layer* pLayer = NULL;//����һ��ͼ��
                float cx = (picBounds.left + picBounds.right) / 2.0;
                float cy = (picBounds.top + picBounds.bottom) / 2.0;
                if (4 != __picType) {
                    m_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(cx, cy),
                        rad, rad)
                        , &pEllipseGeometry);

                    m_pRenderTarget->CreateLayer(NULL, &pLayer);
                    //��ʼͼ��
                    m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pEllipseGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::IdentityMatrix(), 0.8), pLayer);//0.8Ϊ͸����
             
                    //����ͼ���ֵ��ʽ����ֹ����ʧ�棬����ӽ��������ص��ĸ�λͼ�����в�����ɫ��
                    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
                    picBounds.right = cx + rad-2;
                    picBounds.bottom = cy + rad-2;
                    picBounds.left = cx - rad+2;
                    picBounds.top = cy - rad+2;
                    // ����λͼ�����쵽Ŀ�����
                    m_pRenderTarget->DrawBitmap(
                        pBitmap,          // λͼ����
                        picBounds,  // Ŀ�����
                        1.0f,             // ��͸���ȣ�1.0��ʾ��ȫ��͸����
                        interpolationMode // ��ֵģʽ
                    );

                    //ֹͣͼ��
                    m_pRenderTarget->PopLayer();

                    SafeRelease(&pLayer);
                    SafeRelease(&pEllipseGeometry);
                }
                else {
            

                    m_pRenderTarget->CreateLayer(NULL, &pLayer);
                    //��ʼͼ��
                    m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pPathGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::IdentityMatrix(), 0.8), pLayer);//0.8Ϊ͸����

                    //����ͼ���ֵ��ʽ����ֹ����ʧ�棬����ӽ��������ص��ĸ�λͼ�����в�����ɫ��
                    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
                    // ����λͼ�����쵽Ŀ�����
                    m_pRenderTarget->DrawBitmap(
                        pBitmap,          // λͼ����
                        picBounds,  // Ŀ�����
                        1.0f,             // ��͸���ȣ�1.0��ʾ��ȫ��͸����
                        interpolationMode // ��ֵģʽ
                    );

                    //ֹͣͼ��
                    m_pRenderTarget->PopLayer();

                    SafeRelease(&pLayer);
                    SafeRelease(&pEllipseGeometry);
                
                }
            }




            //�����ַ�
            pRenderTarget->DrawText(
                __text.c_str(),
                __text.length(),
                pTextFormat,             // The text format
                textBounds,              // The layout rectangle
                pfBrush                   // The brush

            );
            pfBrush->Release();
        
        }


        // �ͷ���Դ
        pSink->Release();
        pPathGeometry->Release();


        pRenderTarget->EndDraw();
    }
    QCtrlBaseD2DIPD::Draw(sdc);

    return ret;
}

int QPButton::LButtonDown(int x, int y, WPARAM wParam)
{
    BOOL contains = FALSE;
    auto now = std::chrono::high_resolution_clock::now();
    long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __checkPathTime).count();
    if (dta < 2000 && pBTPathGeometry) {
       
        pBTPathGeometry->FillContainsPoint(D2D1_POINT_2F(x - m_rect.X, y - m_rect.Y), nullptr, &contains);
    }
    else{
        if (pBTPathGeometry) {
            pBTPathGeometry->Release();
            pBTSink->Release();
        }
        __checkPathTime = now;
        ID2D1Factory* pFactory = m_pD2DFactory;
        // ���� PathGeometry ����

        pFactory->CreatePathGeometry(&pBTPathGeometry);

        // ���� GeometrySink
      
        pBTPathGeometry->Open(&pBTSink);

        float xRadius = __xRadius, yRadius = __yRadius;

        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (xRadius > (m_rect.Width - __margin) / 2.0) {
            xRadius = (m_rect.Width - __margin) / 4.0;

        }
        if (yRadius > (m_rect.Height - __margin) / 2.0) {
            yRadius = (m_rect.Height - __margin) / 4.0;

        }
        // ����Բ�Ǿ��εĲ���
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width - __margin, m_rect.Height - __margin), // ����λ�úʹ�С
            xRadius, yRadius                              // Բ�ǰ뾶��x, y��
        );

        // ʹ�� Sink ��ʼ����Բ�Ǿ���
        pBTSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // ���
            D2D1_FIGURE_BEGIN_FILLED
        );

        // ��������ͻ���������Բ�Ǿ���
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.top));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.top + roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.bottom - roundedRect.radiusY));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.bottom),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.bottom));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.bottom - roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.top + roundedRect.radiusY));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));

        // �������ر�·��
        pBTSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pBTSink->Close();



        pBTPathGeometry->FillContainsPoint(D2D1_POINT_2F(x - m_rect.X, y - m_rect.Y), nullptr, &contains);
   
   
    }
   
    if (contains) {
        __startTime = std::chrono::high_resolution_clock::now();
        __ShrinkageRatio = 1.01;
        if (m_clickEvent) {
            m_clickEvent(this);
        
        }

    }

    isChange = true;
    UpdateWND();
    return 0;
}

int QPButton::MouseMove(int x, int y, WPARAM wParam)
{

    BOOL contains = FALSE;
    auto now = std::chrono::high_resolution_clock::now();
    long long dta = std::chrono::duration_cast<std::chrono::milliseconds>(now - __checkPathTime).count();
    if (dta < 2000 && pBTPathGeometry) {

        pBTPathGeometry->FillContainsPoint(D2D1_POINT_2F(x - m_rect.X, y - m_rect.Y), nullptr, &contains);
    }
    else {
        if (pBTPathGeometry) {
            pBTPathGeometry->Release();
            pBTSink->Release();
        }
        __checkPathTime = now;
        ID2D1Factory* pFactory = m_pD2DFactory;
        // ���� PathGeometry ����

        pFactory->CreatePathGeometry(&pBTPathGeometry);

        // ���� GeometrySink

        pBTPathGeometry->Open(&pBTSink);

        float xRadius = __xRadius, yRadius = __yRadius;

        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (xRadius > (m_rect.Width - __margin) / 2.0) {
            xRadius = (m_rect.Width - __margin) / 4.0;

        }
        if (yRadius > (m_rect.Height - __margin) / 2.0) {
            yRadius = (m_rect.Height - __margin) / 4.0;

        }
        // ����Բ�Ǿ��εĲ���
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width - __margin, m_rect.Height - __margin), // ����λ�úʹ�С
            xRadius, yRadius                              // Բ�ǰ뾶��x, y��
        );

        // ʹ�� Sink ��ʼ����Բ�Ǿ���
        pBTSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // ���
            D2D1_FIGURE_BEGIN_FILLED
        );

        // ��������ͻ���������Բ�Ǿ���
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.top));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.top + roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.right, roundedRect.rect.bottom - roundedRect.radiusY));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.right - roundedRect.radiusX, roundedRect.rect.bottom),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.bottom));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.bottom - roundedRect.radiusY),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));
        pBTSink->AddLine(D2D1::Point2F(roundedRect.rect.left, roundedRect.rect.top + roundedRect.radiusY));
        pBTSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top),
            D2D1::SizeF(roundedRect.radiusX, roundedRect.radiusY),
            0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
        ));

        // �������ر�·��
        pBTSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pBTSink->Close();



        pBTPathGeometry->FillContainsPoint(D2D1_POINT_2F(x - m_rect.X, y - m_rect.Y), nullptr, &contains);
    }


    if (contains) {
        m_Status = 1;

    }
    else {
        m_Status = 0;
    }
  
    isChange = true;
    UpdateWND();
    return 0;
}

int QPButton::MouseMoveOut()
{
    m_Status = 0;
    isChange = true;
    UpdateWND();
    return 0;
}



const wchar_t* QPButton::GetText()
{
    return __text.c_str();
}

void QPButton::SetText(const wchar_t* t)
{
    __text = t;
    isChange = true;
    UpdateWND();
}

void QPButton::SetClickEvent(QFBUTTONCLICK fun)
{
    m_clickEvent = fun;
}

float QPButton::SetMargin(float margin)
{
    float buf = __margin;
    __margin = margin;
    isChange = true;
    UpdateWND();
    return buf;
}

unsigned int QPButton::SetBKColor(unsigned int color)
{
    unsigned int c = __BKColor;

    __BKColor = color;
    isChange = true;
    UpdateWND();
    return c;
}

unsigned char QPButton::SetPicType(unsigned char type)
{
    unsigned char t = __picType;
    __picType = type;
    isChange = true;
    UpdateWND();
    return t;
}

int QPButton::LoadPicture(const wchar_t* path)
{
    ID2D1Bitmap* pB = nullptr;
    LoadBitmapFromFile(path, m_pRenderTarget, m_pWICFactory, &pB);
    if (pB) {
        if (pBitmap)pBitmap->Release();
        pBitmap = pB;
    }
    isChange = true;
    UpdateWND();
    return 1;
}

const wchar_t* QPButton::GetPicturePath()const
{
    return mapPath.c_str();
}

float QPButton::SetXRadius(float rx)
{
    float r = __xRadius;
    __xRadius = rx;
    isChange = true;
    UpdateWND();
    return r;
}

float QPButton::SetYRadius(float ry)
{
    float r = __yRadius;
    __yRadius = ry;
    isChange = true;
    UpdateWND();
    return r;
}








extern "C"
{

#define __EXTERN_FUN(name) QBT2##name
    //�û��ӿ�
    __declspec(dllexport) const wchar_t* __EXTERN_FUN(GetText)(QPButton*p)
    {
        return p->GetText();
    }
    __declspec(dllexport) void  __EXTERN_FUN(SetText)(QPButton* p,const wchar_t* t)
    {
        return p->SetText(t);
    }
    __declspec(dllexport) void  __EXTERN_FUN(SetClickEvent)(QPButton* p, QFBUTTONCLICK fun)
    {
        return p->SetClickEvent(fun);
    }
    __declspec(dllexport) float  __EXTERN_FUN(SetMargin)(QPButton* p, float margin)
    {
        return p->SetMargin(margin);
    
    }
    __declspec(dllexport) unsigned int  __EXTERN_FUN(SetBKColor)(QPButton* p, unsigned int color)
    {
        return p->SetBKColor(color);
    
    }
    __declspec(dllexport) unsigned char  __EXTERN_FUN(SetPicType)(QPButton* p, unsigned char type)
    {
        return p->SetPicType(type);
    
    }
    __declspec(dllexport) int  __EXTERN_FUN(LoadPicture)(QPButton* p, const wchar_t* path)
    {
        return p->LoadPicture(path);
    
    }
    __declspec(dllexport) const wchar_t* __EXTERN_FUN(GetPicturePath)(QPButton* p)
    {
        return p->GetPicturePath();
    
    }
    __declspec(dllexport) float  __EXTERN_FUN(SetXRadius)(QPButton* p, float rx)
    {
        return p->SetXRadius(rx);
    
    }
    __declspec(dllexport) float  __EXTERN_FUN(SetYRadius)(QPButton* p, float ry)
    {
    
        return p->SetYRadius(ry);
    }



}
