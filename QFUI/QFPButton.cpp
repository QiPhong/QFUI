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
        pRenderTarget->BindDC(m_dc, &rc);//绑定DC输出
        pRenderTarget->BeginDraw();
        //ID2D1SolidColorBrush* pBrush;
        //pRenderTarget->CreateSolidColorBrush(
        //    D2D1::ColorF(D2D1::ColorF::SkyBlue),
        //    &pBrush
        //);
        // //清屏
        //pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        //pBrush->Release();
        

        // 创建 PathGeometry 对象
        ID2D1PathGeometry* pPathGeometry = nullptr;
        pFactory->CreatePathGeometry(&pPathGeometry);

        // 创建 GeometrySink
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
        // 定义圆角矩形的参数
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width- __margin, m_rect.Height- __margin), // 矩形位置和大小
            xRadius, yRadius                              // 圆角半径（x, y）
        );

        // 使用 Sink 开始绘制圆角矩形
        pSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // 起点
            D2D1_FIGURE_BEGIN_FILLED
        );

        // 添加线条和弧线来构建圆角矩形
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

        // 结束并关闭路径
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
            // 3. 创建变换后的几何
            ID2D1TransformedGeometry* pTransformedGeometry = nullptr;
            pFactory->CreateTransformedGeometry(pPathGeometry, scaleMatrix, &pTransformedGeometry);
            // 2. 获取路径的边界矩形
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
                // innerRect 完全在 outerRect 内
                //OutputDebugString(L"Inner rectangle is contained within the outer rectangle.\n");
            }
            else {
                __ShrinkageRatio = 1.0;
                //超出了就不在播放了
                std::chrono::seconds ten_seconds(10);
                __startTime = std::chrono::high_resolution_clock::now() - ten_seconds;
                // innerRect 不完全在 outerRect 内
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

            // 创建渐变停止集合
            pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                ARRAYSIZE(gradientStops),
                &pGradientStops
            );

            // 创建径向渐变画刷
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientBrushProperties = {
                D2D1::Point2F(m_rect.Width / 2.0,m_rect.Height / 2.0), // 渐变中心
                D2D1::Point2F(0,0), // 渐变起点
                m_rect.Width / 2.0, // 内部半径
                m_rect.Height / 2.0 // 外部半径
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

            //通知还要刷新
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
                //如果鼠标不在控件上加上一层黑色
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
            // 绘制文字
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
                    //字符文本矩形也更新下
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
                //图标在左
                ID2D1EllipseGeometry* pEllipseGeometry = NULL;
                ID2D1Layer* pLayer = NULL;//创建一个图层
                float cx = (picBounds.left + picBounds.right) / 2.0;
                float cy = (picBounds.top + picBounds.bottom) / 2.0;
                if (4 != __picType) {
                    m_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(cx, cy),
                        rad, rad)
                        , &pEllipseGeometry);

                    m_pRenderTarget->CreateLayer(NULL, &pLayer);
                    //开始图层
                    m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pEllipseGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::IdentityMatrix(), 0.8), pLayer);//0.8为透明度
             
                    //是则图像差值方式，防止拉伸失真，从最接近呈现像素的四个位图像素中插入颜色。
                    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
                    picBounds.right = cx + rad-2;
                    picBounds.bottom = cy + rad-2;
                    picBounds.left = cx - rad+2;
                    picBounds.top = cy - rad+2;
                    // 绘制位图并拉伸到目标矩形
                    m_pRenderTarget->DrawBitmap(
                        pBitmap,          // 位图对象
                        picBounds,  // 目标矩形
                        1.0f,             // 不透明度（1.0表示完全不透明）
                        interpolationMode // 插值模式
                    );

                    //停止图层
                    m_pRenderTarget->PopLayer();

                    SafeRelease(&pLayer);
                    SafeRelease(&pEllipseGeometry);
                }
                else {
            

                    m_pRenderTarget->CreateLayer(NULL, &pLayer);
                    //开始图层
                    m_pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pPathGeometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::IdentityMatrix(), 0.8), pLayer);//0.8为透明度

                    //是则图像差值方式，防止拉伸失真，从最接近呈现像素的四个位图像素中插入颜色。
                    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
                    // 绘制位图并拉伸到目标矩形
                    m_pRenderTarget->DrawBitmap(
                        pBitmap,          // 位图对象
                        picBounds,  // 目标矩形
                        1.0f,             // 不透明度（1.0表示完全不透明）
                        interpolationMode // 插值模式
                    );

                    //停止图层
                    m_pRenderTarget->PopLayer();

                    SafeRelease(&pLayer);
                    SafeRelease(&pEllipseGeometry);
                
                }
            }




            //绘制字符
            pRenderTarget->DrawText(
                __text.c_str(),
                __text.length(),
                pTextFormat,             // The text format
                textBounds,              // The layout rectangle
                pfBrush                   // The brush

            );
            pfBrush->Release();
        
        }


        // 释放资源
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
        // 创建 PathGeometry 对象

        pFactory->CreatePathGeometry(&pBTPathGeometry);

        // 创建 GeometrySink
      
        pBTPathGeometry->Open(&pBTSink);

        float xRadius = __xRadius, yRadius = __yRadius;

        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (xRadius > (m_rect.Width - __margin) / 2.0) {
            xRadius = (m_rect.Width - __margin) / 4.0;

        }
        if (yRadius > (m_rect.Height - __margin) / 2.0) {
            yRadius = (m_rect.Height - __margin) / 4.0;

        }
        // 定义圆角矩形的参数
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width - __margin, m_rect.Height - __margin), // 矩形位置和大小
            xRadius, yRadius                              // 圆角半径（x, y）
        );

        // 使用 Sink 开始绘制圆角矩形
        pBTSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // 起点
            D2D1_FIGURE_BEGIN_FILLED
        );

        // 添加线条和弧线来构建圆角矩形
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

        // 结束并关闭路径
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
        // 创建 PathGeometry 对象

        pFactory->CreatePathGeometry(&pBTPathGeometry);

        // 创建 GeometrySink

        pBTPathGeometry->Open(&pBTSink);

        float xRadius = __xRadius, yRadius = __yRadius;

        int minV = (std::min)(m_rect.Width, m_rect.Height);
        if (xRadius > (m_rect.Width - __margin) / 2.0) {
            xRadius = (m_rect.Width - __margin) / 4.0;

        }
        if (yRadius > (m_rect.Height - __margin) / 2.0) {
            yRadius = (m_rect.Height - __margin) / 4.0;

        }
        // 定义圆角矩形的参数
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(__margin, __margin, m_rect.Width - __margin, m_rect.Height - __margin), // 矩形位置和大小
            xRadius, yRadius                              // 圆角半径（x, y）
        );

        // 使用 Sink 开始绘制圆角矩形
        pBTSink->BeginFigure(
            D2D1::Point2F(roundedRect.rect.left + roundedRect.radiusX, roundedRect.rect.top), // 起点
            D2D1_FIGURE_BEGIN_FILLED
        );

        // 添加线条和弧线来构建圆角矩形
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

        // 结束并关闭路径
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
    //用户接口
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
