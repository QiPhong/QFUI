
#include "QFCtrlBase.h"

QCtrlBase::QCtrlBase(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
    QCreateDC(winddc);

}
QCtrlBase::~QCtrlBase()
{
    ReleaseDC();

}

int QCtrlBase::Draw(HDC sdc)
{
    //添加绘制代码

    //****************************************
    return QWND::Draw(sdc);


}
void QCtrlBase::Reconstruct(HDC sdc, const GRect& rect)
{
    /*QWND::Reconstruct(sdc, rect);*/
    //ReleaseDC();
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;

    //QCreateDC(sdc);
    Reconstruct(sdc);

}
void QCtrlBase::Reconstruct(HDC sdc)
{
    QWND::Reconstruct(sdc);
    //ReleaseDC();
    //QCreateDC(sdc);
    ReconstructDC(m_bdc,m_dc, m_bhB,m_rect);
    ReconstructDC(m_bkdc, m_dc, m_bkhB, m_rect);
    isChange = true;
}
void QCtrlBase::SetLocation(const GPoint& point)
{
    QWND::SetLocation(point);

}
void QCtrlBase::SetSize(const GSize& size, HDC dc)
{
    QWND::SetSize(size, dc);

}

inline void QCtrlBase::QCreateDC(HDC winddc)
{
    //创建缓冲DC
    m_bdc = CreateCompatibleDC(winddc);
    m_bhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bdc, m_bhB);
    //创建缓冲BKDC
    m_bkdc = CreateCompatibleDC(winddc);
    m_bkhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bkdc, m_bkhB);
}
inline void QCtrlBase::ReleaseDC()
{
    //释放缓冲dc
    DeleteObject(m_bhB);
    m_bhB = NULL;
    DeleteDC(m_bdc);
    m_bdc = NULL;

    //释放背景dc
    DeleteObject(m_bkhB);
    m_bkhB = NULL;
    DeleteDC(m_bkdc);
    m_bkdc = NULL;

}
void QCtrlBase::UpdateBK(const GBrush& brush)
{
    if (m_bkdc) {
        Gdiplus::Graphics graphics(m_bkdc);
        graphics.FillRectangle(&brush, 0, 0, m_rect.Width, m_rect.Height);

    }

}
QDWORD QCtrlBase::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if(m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);
        
        }


    }break;
    
    }

    return QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}
HRESULT LoadBitmapFromFile(PCWSTR uri, ID2D1DCRenderTarget* prt, IWICImagingFactory* pcif, ID2D1Bitmap** pBitmap)
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;

    hr = pcif->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        hr = pcif->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
    }
    if (SUCCEEDED(hr))
    {
        hr = prt->CreateBitmapFromWicBitmap(pConverter, NULL, pBitmap);
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);

    return hr;
}



QContainer::QContainer(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    __WNDAttribute.Attribute.isContainerCtrl = 1;//容器控件
}

QContainer::~QContainer()
{
}

QDWORD QContainer::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{


    return QCtrlBase::TransferMessage(msgID, LParam, RParam, AddParam);
}
HCURSOR QLayout::m_hCursorBuff = NULL;
QLayout::QLayout(HDC winddc, const GRect& rf):QWND(winddc,rf)
{
    __WNDAttribute.Attribute.isLayout = 1;//这是一个布局器
    if (!m_hCursorBuff) {
        m_hCursorBuff = LoadCursor(NULL, IDC_ARROW);
    }
    __ct = std::chrono::high_resolution_clock::now();
}

QLayout::~QLayout()
{
}



QCtrlBaseD2D::QCtrlBaseD2D(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
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

    RECT rc(0, 0, rf.Width, rf.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//绑定DC输出

    //创建一个图层
    m_pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory));

}
QCtrlBaseD2D::~QCtrlBaseD2D()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
    SafeRelease(&pLayer);

}

int QCtrlBaseD2D::Draw(HDC sdc)
{
    //添加绘制代码

    //****************************************
    return QWND::Draw(sdc);


}
void QCtrlBaseD2D::Reconstruct(HDC sdc, const GRect& rect)
{
    /*QWND::Reconstruct(sdc, rect);*/
    //ReleaseDC();
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;
    //QCreateDC(sdc);
    Reconstruct(sdc);
 
    RECT rc(0, 0, m_rect.Width, m_rect.Height);
    m_pRenderTarget->BindDC(m_dc, &rc);//绑定DC输出

}
void QCtrlBaseD2D::Reconstruct(HDC sdc)
{

    QWND::Reconstruct(sdc);
    isChange = true;
}
void QCtrlBaseD2D::SetLocation(const GPoint& point)
{
    QWND::SetLocation(point);

}
void QCtrlBaseD2D::SetSize(const GSize& size, HDC dc)
{
    QWND::SetSize(size, dc);

}

QDWORD QCtrlBaseD2D::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
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






std::shared_ptr < D2DObjManage<ID2D1Factory>> QCtrlBaseD2DIPD::__pD2DFactory_SP;
std::shared_ptr < D2DObjManage<ID2D1DCRenderTarget>> QCtrlBaseD2DIPD::__pRenderTarget_SP;
std::shared_ptr < D2DObjManage<IDWriteFactory>> QCtrlBaseD2DIPD::__pDWriteFactory_SP;
std::shared_ptr < D2DObjManage<IWICImagingFactory>> QCtrlBaseD2DIPD::__pWICFactory_SP;
std::mutex QCtrlBaseD2DIPD::__SPMakeLK;

QCtrlBaseD2DIPD::QCtrlBaseD2DIPD(HDC winddc, const GRect& rf) :QFUIBase(winddc, rf)
{
    std::unique_lock<std::mutex> lk(__SPMakeLK);
    if (!__pD2DFactory_SP) {
        ID2D1Factory* d2df = nullptr;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2df);
        __pD2DFactory_SP = std::make_shared<D2DObjManage<ID2D1Factory>>(d2df);
        
    }
    m_pD2DFactory_SP = __pD2DFactory_SP;
    m_pD2DFactory = m_pD2DFactory_SP.get()->ptr;

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    if (!__pRenderTarget_SP) {

        ID2D1DCRenderTarget* d2drt = nullptr;
        HRESULT hr = m_pD2DFactory->CreateDCRenderTarget(&props, &d2drt);
        __pRenderTarget_SP = std::make_shared<D2DObjManage<ID2D1DCRenderTarget>>(d2drt);
    }
    m_pRenderTarget_SP = __pRenderTarget_SP;
    m_pRenderTarget = m_pRenderTarget_SP.get()->ptr;

    if (!__pDWriteFactory_SP) {
        IDWriteFactory* idwf = nullptr;
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&idwf));
        __pDWriteFactory_SP = std::make_shared<D2DObjManage<IDWriteFactory>>(idwf);

    }
    m_pDWriteFactory_SP = __pDWriteFactory_SP;
    m_pDWriteFactory = m_pDWriteFactory_SP.get()->ptr;

    m_pDWriteFactory->CreateTextFormat(
        m_fontName.c_str(),                // Font family
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,                   // Font size
        L"en-us",                // Locale
        &pTextFormat
    );


    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中
    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    pTextFormat->SetTrimming(&trimming, nullptr);//防止溢出布局框



 
    if (!__pWICFactory_SP) {
        IWICImagingFactory* iwiciif = nullptr;
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&iwiciif));
        __pWICFactory_SP = std::make_shared<D2DObjManage<IWICImagingFactory>>(iwiciif);
    }
    m_pWICFactory_SP = __pWICFactory_SP;
    m_pWICFactory = m_pWICFactory_SP.get()->ptr;
}

QCtrlBaseD2DIPD::~QCtrlBaseD2DIPD()
{
    std::unique_lock<std::mutex> lk(__SPMakeLK);
    //SafeRelease(&m_pD2DFactory);
    //SafeRelease(&m_pRenderTarget);
    SafeRelease(&pTextFormat);
    //SafeRelease(&pDWriteFactory);
    m_pD2DFactory_SP.reset();
    m_pRenderTarget_SP.reset();
    m_pDWriteFactory_SP.reset();
    m_pWICFactory_SP.reset();

    if (1 >= __pD2DFactory_SP.use_count()) {
        __pD2DFactory_SP.reset();
    }
    if (1 >= __pRenderTarget_SP.use_count()) {
        __pRenderTarget_SP.reset();
    }
    if (1 >= __pDWriteFactory_SP.use_count()) {
        __pDWriteFactory_SP.reset();
    }
    if (1 >= __pWICFactory_SP.use_count()) {
        __pWICFactory_SP.reset();
    }
}

int QCtrlBaseD2DIPD::Draw(HDC sdc)
{
    return QWND::Draw(sdc);
}

void QCtrlBaseD2DIPD::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;
    //QCreateDC(sdc);
    Reconstruct(sdc);
}

void QCtrlBaseD2DIPD::Reconstruct(HDC sdc)
{
    QWND::Reconstruct(sdc);
    isChange = true;
}

void QCtrlBaseD2DIPD::SetLocation(const GPoint& point)
{
    QWND::SetLocation(point);
}

void QCtrlBaseD2DIPD::SetSize(const GSize& size, HDC dc)
{
    QWND::SetSize(size, dc);
}

QDWORD QCtrlBaseD2DIPD::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
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
void QCtrlBaseD2DIPD::SetFontName(const wchar_t* t)
{
    m_fontName = t;
    IDWriteTextFormat* pTFormat = NULL;
    m_pDWriteFactory->CreateTextFormat(
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
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中
    pTextFormat = pTFormat;
    SafeRelease(&buff);
    isChange = true;
    UpdateWND();
}

void QCtrlBaseD2DIPD::SetFontSize(double size)
{
    if (size <= 0)return;
    m_fontSize = size;
    IDWriteTextFormat* pTFormat = NULL;
    m_pDWriteFactory->CreateTextFormat(
        m_fontName.c_str(),                // Font family
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,                   // Font size
        L"en-us",                // Locale
        &pTFormat
    );
    pTFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//将段落中心和布局框中心重合，
    pTFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);//指示单词保留在同一行中，即使它溢出布局框。（保持单行）
    // Set paragraph alignment to center
    pTFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//上下居中
    IDWriteTextFormat* buff = pTextFormat;

    pTextFormat = pTFormat;
    SafeRelease(&buff);
    isChange = true;
    UpdateWND();
}



void QCtrlBaseD2DIPD::SetFontColor(unsigned int color)
{
    m_fontColor = color;
    isChange = true;
    UpdateWND();
}

extern "C"
{
    __declspec(dllexport) void QCBDDSetFontSize(QCtrlBaseD2DIPD* p, double size)
    {
    
        return p->SetFontSize(size);
    }

    __declspec(dllexport) void QCBDDSetFontName(QCtrlBaseD2DIPD* p, const wchar_t* t)
    {
        return p->SetFontName(t);
    
    }
    __declspec(dllexport) void QCBDDSetFontColor(QCtrlBaseD2DIPD* p, unsigned int color)
    {
    
        return p->SetFontColor(color);
    }

}


void QSCWnd::RemoveCtrl(QWND* w)
{
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    auto it = std::find(m_QWNDs.begin(), m_QWNDs.end(), w);
    if (it != m_QWNDs.end()) {
        m_QWNDs.erase(it);

    }
}
int QSCWnd::GetSubCtrlCount()
{
    return m_QWNDs.size();
}
QWND* QSCWnd::GetSubCtrl(int index)
{
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    if (index<0 || index> m_QWNDs.size() - 1)return nullptr;

    return m_QWNDs[index];
}
void QSCWnd::AttachCtrl(QWND* w)
{
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    m_QWNDs.push_back(w);
}
