#include "QFProgressBarCar.h"
#include "../QFSource/resource.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <format>
QFPBCM QProgressBarCar::__qm;
HMODULE QFPBCM::__srcDLL = NULL;
Gdiplus::GraphicsPath* QFPBCM::__CarPath = NULL;
Gdiplus::Bitmap* DLLPictureSource(HMODULE hm, const char* typeName, int srcName, int* err = NULL)
{
    Gdiplus::Bitmap* pBitmap = NULL;
    if (hm) {
        HRSRC hResource = FindResourceA(hm, MAKEINTRESOURCEA(srcName), typeName);
        if (hResource == NULL) {
            if (err)
                *err = 1;
            return pBitmap;
        }
        HGLOBAL hGlobal = LoadResource(hm, hResource);
        if (hGlobal == NULL) {
            // 处理资源加载失败的情况
            if (err)
                *err = 2;
            return pBitmap;
        }

        LPVOID pData = LockResource(hGlobal);
        DWORD dwSize = SizeofResource(hm, hResource);

        // 使用内存流加载图片资源
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
        if (hMem == NULL) {
            if (err)
                *err = 3;
            return pBitmap;
        }

        LPVOID pMem = GlobalLock(hMem);
        CopyMemory(pMem, pData, dwSize);
        GlobalUnlock(hMem);

        IStream* pStream = nullptr;
        if (SUCCEEDED(CreateStreamOnHGlobal(hMem, TRUE, &pStream)))
        {
            pBitmap = new Gdiplus::Bitmap(pStream);
            pStream->Release();
        }

        GlobalFree(hMem);
    }

    return pBitmap;
}

//std::string GetCurrentModuleAddr()
//{
//
//    HMODULE hCaller = NULL;
//    char szModuleName[MAX_PATH];
//    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)(&GetCurrentModuleAddr), &hCaller);//GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
//    GetModuleFileNameA(hCaller, szModuleName, _countof(szModuleName));
//
//    std::string strDllPathBuff = szModuleName;
//
//    auto nPathTruncLoc = strDllPathBuff.find_last_of('\\');
//    strDllPathBuff = strDllPathBuff.substr(0, nPathTruncLoc + 1);
//    return strDllPathBuff;
//}

void ShrinkPath(Gdiplus::GraphicsPath& path, float scale)
{
    Gdiplus::Matrix matrix;
    matrix.Scale(scale, scale);
    path.Transform(&matrix);
}

QProgressBarCar::QProgressBarCar(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
    QCreateDC(winddc);
    //std::unique_lock<std::mutex> ul(__lock);
    //初始化路径
    LoadResou();
    //if (!__qm.__CarPath) {
    //    //1读取DLL
    //    if (!__qm.__srcDLL) {
    //        __qm.__srcDLL = LoadLibraryA((GetCurrentModuleAddr()+"QFSource.dll").c_str());
    //    }
    //    int errCode = 0;
    //    Gdiplus::Bitmap* pBm = DLLPictureSource(__qm.__srcDLL,"PNG",IDB_PBC_PNG,&errCode);
    //    if (NULL == pBm) {
    //        MessageBoxA(0, ("QProgressBarCar的资源加载失败:" + std::to_string(errCode)).c_str(), "QProgressBarCar", 0);
    //        return;
    //    }
    //    Gdiplus::GraphicsPath path;
    //    int width = pBm->GetWidth();
    //    int height = pBm->GetHeight();
    //    bool isPathStarted = false;
    //    std::vector<Gdiplus::PointF> point;
    //    for (int x = 0; x < width; x++) {
    //        int state = 0;
    //        for (int y = 0; y < height; y++) {//int x = 0; x < width; x++
    //            Gdiplus::Color pixelColor;
    //            //bitmap.GetPixel(x, y, &pixelColor);
    //            pBm->GetPixel(x, y, &pixelColor);
    //            if (pixelColor.GetValue() == Gdiplus::Color::Black) {
    //                point.push_back(Gdiplus::PointF(x, y));
    //                break;
    //            }

    //        }

    //    }
    //    for (int x = width - 1; x > 0; --x) {

    //        for (int y = height - 1; y > 0; --y) {//int x = width-1; x > 0; --x
    //            Gdiplus::Color pixelColor;
    //            //bitmap.GetPixel(x, y, &pixelColor);
    //            pBm->GetPixel(x, y, &pixelColor);
    //            if (pixelColor.GetValue() == Gdiplus::Color::Black) {
    //                point.push_back(Gdiplus::PointF(x, y));
    //                break;
    //            }

    //        }

    //    }
    //    //if (1 < point.size()) {
    //    //    int x = point[0].first;
    //    //    int y = point[0].second;
    //    //    for (int i = 0;++i < point.size();) {
    //    //        path.AddLine(x, y, point[i].first, point[i].second);
    //    //        x = point[i].first;
    //    //        y = point[i].second;
    //    //    }
    //    //    path.CloseFigure();
    //    //}
    //    path.Flatten(NULL, 0.02);
    //    path.AddBeziers(point.data(), point.size());
    //
    //    path.CloseFigure();
    //    __qm.__CarPath = path.Clone();
    //}
    //m_CarPath = __qm.__CarPath->Clone();
  
    //m_CarPath->GetBounds(&m_path_bounds);
    //pdcW = ceil(m_path_bounds.X + m_path_bounds.Width);
    //pdcH = ceil(m_path_bounds.Y + m_path_bounds.Height);
    //     
    ////创建缓冲DC
    ////m_pdc = CreateCompatibleDC(winddc);
    ////m_phB = CreateCompatibleBitmap(winddc, pdcW, pdcH);
    ////SelectObject(m_pdc, m_phB);
    //Gdiplus::Matrix matrix;
    //matrix.Translate(-m_path_bounds.X, -m_path_bounds.Y);
    //m_CarPath->Transform(&matrix);

    //auto w=std::max<Gdiplus::REAL>(m_path_bounds.Width, m_path_bounds.Height);
    //auto w2 = std::max<Gdiplus::REAL>(m_rect.Width, m_rect.Height);
    ////Gdiplus::Matrix matrix;
    ////matrix.Translate(bounds.X, bounds.Y);
    ////m_CarPath->Transform(&matrix);
    //ShrinkPath(*m_CarPath, w2/w);
    fBrush = new Gdiplus::SolidBrush(Gdiplus::Color(200,128,128,128));
    m_gsf = Gdiplus::StringFormat::GenericTypographic()->Clone();
    m_gsf->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//垂直居中
    m_gsf->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//水平居中
    m_gsf->SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);

}

QProgressBarCar::~QProgressBarCar()
{
    if(m_CarPath)
        delete m_CarPath;
    ReleaseDC();
    ////释放缓冲dc
    //if (m_phB) {
    //    DeleteObject(m_phB);
    //    m_phB = NULL;
    //}
    //if (m_pdc) {
    //    DeleteDC(m_pdc);
    //    m_pdc = NULL;
    //}
    if (fBrush)delete fBrush;
    if (m_gsf)delete m_gsf;
}

int QProgressBarCar::Draw(HDC sdc)
{

    int ret = 0;
    if (m_ischange) {
        std::unique_lock<std::mutex> lk(m_drawLock);
        m_ischange = false;
        ret = 1;
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
        {
            //StretchBlt(m_pdc, 0, 0, pdcW, pdcH, m_bdc,0,0, m_rect.Width, m_rect.Height, SRCCOPY);

            //Gdiplus::Graphics graphics(m_pdc);
            Gdiplus::Graphics graphics(m_bdc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
            
            Gdiplus::GraphicsPath& path = *m_CarPath;
            // 绘制车身曲线
            Gdiplus::Region carRegion(&path);
            
            

            
            Gdiplus::RectF bounds;
            path.GetBounds(&bounds);
            Gdiplus::RectF atoRect = bounds;
            atoRect.Width = atoRect.Width * m_rat / 100.0;
            Gdiplus::Region leftHalfRegion(atoRect);

            leftHalfRegion.Intersect(&carRegion);
            // 创建GDI+画刷
            Gdiplus::SolidBrush brush(Gdiplus::Color::Gray);
            Gdiplus::Pen pen(Gdiplus::Color::Black, 0.05);
            // 填充车身区域
            //graphics.FillPath(&brush, &path);
            graphics.FillRegion(fBrush, &leftHalfRegion);
            graphics.DrawPath(&pen, &path);
            Gdiplus::Font drawFont(drawFontName.c_str(), drawFontSize);
            Gdiplus::SolidBrush brush2(Gdiplus::Color((drawFontColor>>24)&0xff, (drawFontColor >> 16) & 0xff, (drawFontColor >> 8) & 0xff, drawFontColor  & 0xff));
            
            
            std::wstring str;
            if (isShowDec) {
        /*        std::wstring vstr = std::to_wstring(m_rat);
                str = vstr.substr(0, vstr.find(L".") + 3) + L"%";*/
                str = std::format(L"{0:.2f}", m_rat) + L"%";;
            }
            else {
                str = std::format(L"{0:d}", (int)m_rat) + L"%";;
            }
            //StretchBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, m_pdc, 0, 0, pdcW, pdcH, SRCCOPY);
            graphics.DrawString(str.c_str(), -1, &drawFont, bounds, m_gsf, &brush2);
   
        }
        


        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);
    }
    QWND::Draw(sdc);
    return ret;
}

void QProgressBarCar::SetFontSize(double size)
{
    drawFontSize = size;
    m_ischange = true;
    UpdateWND();
}

void QProgressBarCar::SetFontName(const wchar_t* name)
{
    drawFontName = name;
    m_ischange = true;
    UpdateWND();
}

void QProgressBarCar::SetFontColor(unsigned int colo)
{
    drawFontColor = colo;
    m_ischange = true;
    UpdateWND();
}

void QProgressBarCar::IsShowDec(bool isShow)
{
    isShowDec = isShow;
    m_ischange = true;
    UpdateWND();
}

void QProgressBarCar::SetFillBrush(Gdiplus::Brush* brush)
{
    if (fBrush)delete fBrush;
    fBrush = brush->Clone();
}

void QProgressBarCar::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    std::unique_lock<std::mutex> lk(m_drawLock);
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;

    Reconstruct(sdc);
}

void QProgressBarCar::Reconstruct(HDC sdc)
{
    QWND::Reconstruct(sdc);
    ReconstructDC(m_bdc, m_dc, m_bhB, m_rect);
    LoadResou();
}

QDWORD QProgressBarCar::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID) {
    case QM_UPDATE_CTRL: {
        m_ischange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }


    }break;

    }

    return QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}

void QProgressBarCar::SetPercentage(double percent)
{
    if (percent < 0)percent = 0;
    else if (percent > 100)percent = 100;
    m_rat = percent;
    m_ischange = true;
    UpdateWND();
}

double QProgressBarCar::GetPercentage()
{
    return m_rat;
}



inline void QProgressBarCar::QCreateDC(HDC winddc)
{
    //创建缓冲DC
    m_bdc = CreateCompatibleDC(winddc);
    m_bhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bdc, m_bhB);
}

inline void QProgressBarCar::ReleaseDC()
{
    //释放缓冲dc
    DeleteObject(m_bhB);
    m_bhB = NULL;
    DeleteDC(m_bdc);
    m_bdc = NULL;
}

void QProgressBarCar::LoadResou()
{
    std::unique_lock<std::mutex> ul(__lock);
    if (m_CarPath) {
        delete m_CarPath;
        m_CarPath = NULL;
    }
    //初始化路径
    if (!__qm.__CarPath) {
        //1读取DLL
        if (!__qm.__srcDLL) {
            __qm.__srcDLL = LoadLibraryA((GetCurrentModuleAddr() + "QFSource.dll").c_str());
        }
        int errCode = 0;
        Gdiplus::Bitmap* pBm = DLLPictureSource(__qm.__srcDLL, "PNG", IDB_PBC_PNG, &errCode);
        if (NULL == pBm) {
            MessageBoxA(0, ("QProgressBarCar的资源加载失败:" + std::to_string(errCode)).c_str(), "QProgressBarCar", 0);
            return;
        }
        Gdiplus::GraphicsPath path;
        int width = pBm->GetWidth();
        int height = pBm->GetHeight();
        bool isPathStarted = false;
        std::vector<Gdiplus::PointF> point;
        for (int x = 0; x < width; x++) {
            int state = 0;
            for (int y = 0; y < height; y++) {//int x = 0; x < width; x++
                Gdiplus::Color pixelColor;
                //bitmap.GetPixel(x, y, &pixelColor);
                pBm->GetPixel(x, y, &pixelColor);
                if (pixelColor.GetValue() == Gdiplus::Color::Black) {
                    point.push_back(Gdiplus::PointF(x, y));
                    break;
                }

            }

        }
        for (int x = width - 1; x > 0; --x) {

            for (int y = height - 1; y > 0; --y) {//int x = width-1; x > 0; --x
                Gdiplus::Color pixelColor;
                //bitmap.GetPixel(x, y, &pixelColor);
                pBm->GetPixel(x, y, &pixelColor);
                if (pixelColor.GetValue() == Gdiplus::Color::Black) {
                    point.push_back(Gdiplus::PointF(x, y));
                    break;
                }

            }

        }

        path.Flatten(NULL, 0.02);
        path.AddBeziers(point.data(), point.size());

        path.CloseFigure();
        __qm.__CarPath = path.Clone();
    }
    m_CarPath = __qm.__CarPath->Clone();

    m_CarPath->GetBounds(&m_path_bounds);
    pdcW = ceil(m_path_bounds.X + m_path_bounds.Width);
    pdcH = ceil(m_path_bounds.Y + m_path_bounds.Height);

    Gdiplus::Matrix matrix;
    matrix.Translate(-m_path_bounds.X, -m_path_bounds.Y);
    m_CarPath->Transform(&matrix);

    auto w = std::max<Gdiplus::REAL>(m_path_bounds.Width, m_path_bounds.Height);
    auto w2 = std::max<Gdiplus::REAL>(m_rect.Width, m_rect.Height);

    ShrinkPath(*m_CarPath, w2 / w);
}

QFPBCM::~QFPBCM()
{
    if (__CarPath) {
        delete __CarPath;
        __CarPath = NULL;
    }
    if (__srcDLL) {
        FreeLibrary(__srcDLL);
        __srcDLL = NULL;
    }
}

extern "C"
__declspec(dllexport) void QFBCSetPercentage(QProgressBarCar * wnd,double percent)
{
    return wnd->SetPercentage(percent);
}

extern "C"
__declspec(dllexport) double QFBCGetPercentage(QProgressBarCar * wnd)
{
    return wnd->GetPercentage();
}

extern "C"
__declspec(dllexport) void QFBCSetFontSize(QProgressBarCar * wnd,double size)
{
    return wnd->SetFontSize(size);
}
extern "C"
__declspec(dllexport) void QFBCSetFontName(QProgressBarCar * wnd, const wchar_t* name)
{
    return wnd->SetFontName(name);
}
extern "C"
__declspec(dllexport) void QFBCSetFontColor(QProgressBarCar * wnd, int colo)
{
    return wnd->SetFontColor(colo);
}
extern "C"
__declspec(dllexport) void QFBCIsShowDec(QProgressBarCar * wnd, bool isShow)
{
    return wnd->IsShowDec(isShow);
}

extern "C"
__declspec(dllexport) void QFBCSetFillBrush(QProgressBarCar * wnd, Gdiplus::Brush * brush)
{
    return wnd->SetFillBrush(brush);
}