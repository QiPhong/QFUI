
#include "QFCATLCtrl.h"
#include <format>
#include <cmath>
//QCATLBarChart的函数定义
#if 1
inline void QCATLBarChart::QCreateDC(HDC winddc)
{
    //创建曲线DC
    m_curveDC = CreateCompatibleDC(winddc);
    m_curveHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_curveDC, m_curveHB);
}

inline void QCATLBarChart::ReleaseDC()
{
    DeleteObject(m_curveHB);
    m_curveHB = NULL;
    DeleteDC(m_curveDC);
    m_curveDC = NULL;
}

QCATLBarChart::QCATLBarChart(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    QCreateDC(winddc);
    //Test();
}

QCATLBarChart::~QCATLBarChart()
{
    ReleaseDC();
    std::unique_lock<std::shared_mutex> lk(m_lock);
}

int QCATLBarChart::Draw(HDC sdc)
{
    bool flag = 0;

    //添加绘制代码
    if (isChange) {
        flag = 1;
        isChange = false;
        //1.sdc中对应的位置拷贝过来
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);

        //2.绘制坐标轴和刻度
        {
            Gdiplus::Graphics graphics(m_bdc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
            GPen pen(m_ScaleColor, m_lineWidth);
            graphics.DrawLine(&pen, GPointF(m_ScaleWidth, 0.0), GPointF(m_ScaleWidth, (float)m_rect.Height - m_ScaleHeight));
            graphics.DrawLine(&pen, GPointF(m_ScaleWidth, GetYAxisBegin()), GPointF(GetXAxisEnd(), GetYAxisBegin()));

            //绘制y轴刻度
            float TotalSize = m_MaxSc - m_MinSc;//
            float tLen = GetYAxisLength();//绘制的总长度
            float sL = m_ScaleWidth * 0.1;//小刻度的长度
            float mL = m_ScaleWidth * 0.3;//大刻度的长度
            float sx = m_ScaleWidth;//y轴开始的x坐标
            if (m_YAxis_SW < m_lineWidth * 2)m_YAxis_SW = m_lineWidth * 2;
            float TotaSeg = tLen / m_YAxis_SW;    //总段数
            float SegLen = TotalSize / TotaSeg;  //每一段自增的值
            int i = 0;

            GFont drawFont(m_fontName.c_str(), m_fontSize);
            GREAL FontHeight = drawFont.GetHeight(&graphics);
            for (float cp = tLen; cp > m_YAxis_SW; cp -= m_YAxis_SW, ++i) {
                if (!(i % 5)) {
                    graphics.DrawLine(&pen, GPointF(sx, cp), GPointF(sx - mL, cp));

                    GRectF tRect(0, cp - FontHeight / 2, m_ScaleWidth - mL, FontHeight);
                    if (0 == i) {
                        tRect.Y = cp - FontHeight;

                    }

                    GStringFormat gsf(0, 0);
                    gsf.SetAlignment(sAlign);//水平居中
                    gsf.SetLineAlignment(slAlign);//垂直居中
                    gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                    GFont drawFont(m_fontName.c_str(), m_fontSize);
                    Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                    std::wstring str = std::format(L"{:.2f}", m_MinSc + SegLen * i);

                    graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, tRect, &gsf, &gsb);
                }
                else {
                    graphics.DrawLine(&pen, GPointF(sx, cp), GPointF(sx - sL, cp));
                }
            }



        }
        //3.绘制柱体
        {
            StretchBlt(m_curveDC, 0, 0, m_rect.Width- m_ScaleWidth, m_rect.Height,
                m_bdc, m_ScaleWidth, 0, m_rect.Width- m_ScaleWidth, m_rect.Height, SRCCOPY);
            Gdiplus::Graphics graphics(m_curveDC);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            GPen pen1(GColor(0xff, 0, 0, 0));
            pen1.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDotDot);
            GStringFormat gsf(0, 0);
            gsf.SetAlignment(sAlign);//水平居中
            gsf.SetLineAlignment(slAlign);//垂直居中
            //gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
            Gdiplus::SolidBrush gsb(GColor(0xff, 0xff, 0, 0));
            Gdiplus::SolidBrush fgsb(GColor(0xff, 0xff, 0, 0));
            {
                //float TotalSize = m_MaxSc - m_MinSc;//
                //float tLen = GetYAxisLength();//绘制的总长度
                //float sL = m_ScaleWidth * 0.1;//小刻度的长度
                //float mL = m_ScaleWidth * 0.3;//大刻度的长度
                //float sx = m_ScaleWidth;//y轴开始的x坐标
                float sxx = m_startPos;//开始的横坐标
                GFont drawFont(m_fontName.c_str(), m_fontSize);
                std::shared_lock<std::shared_mutex> lk(m_lock);
                for (auto& it : m_data) {
                    for (auto& bc : it) {
                        if ((sxx + m_bcWidth) > 0) {
                            gsb.SetColor(bc.color);
                            if (bc.v < m_MaxSc) {
                                if (0 == m_MaxSc - m_MinSc)continue;
                                float height = GetYAxisLength() * (bc.v - m_MinSc) / (m_MaxSc - m_MinSc);
                                GRectF rect(sxx, GetYAxisLength() - height, m_bcWidth, height);
                                graphics.FillRectangle(&gsb, rect);
                                if (L"" != bc.text) {
                                    fgsb.SetColor(bc.tcolor);
                                    graphics.DrawString(bc.text.c_str(), (INT)bc.text.length(), &drawFont, rect, &gsf, &fgsb);
                                }

                            }
                            else {
                                GRectF rect(sxx, 0.0f, m_bcWidth, (float)GetYAxisLength());
                                graphics.FillRectangle(&gsb, rect);
                                if (L"" != bc.text) {
                                    fgsb.SetColor(bc.tcolor);
                                    graphics.DrawString(bc.text.c_str(), (INT)bc.text.length(), &drawFont, rect, &gsf, &fgsb);
                                }
                            }
                        }
                        sxx += m_bcWidth;
                    }
                    sxx += m_gridWidth;

                }
            }
 

            {
            
                double y = m_mousey - m_rect.GetTop();
                double x = m_mousex - m_rect.X- m_ScaleWidth+8;
                float y_ = TransforPointY(y);
                graphics.DrawLine(&pen1, GPointF(0, y), GPointF(GetXAxisEnd(), y));
            
                GFont drawFont(m_fontName.c_str(), m_fontSize);
                Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                std::wstring str = std::format(L"{:.2f}", y_);
                GPointF pt(x, y);
                graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, pt, &gsb);

                StretchBlt(m_bdc, m_ScaleWidth, 0, m_rect.Width - m_ScaleWidth, m_rect.Height - m_ScaleHeight,
                    m_curveDC, 0, 0, m_rect.Width - m_ScaleWidth, m_rect.Height - m_ScaleHeight, SRCCOPY);
            }



        }


        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);

    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;
}

void QCATLBarChart::SetFontName(const wchar_t* name)
{
    m_fontName = name;

    isChange = true;
}

void QCATLBarChart::SetFontSize(float size)
{
    m_fontSize = size;

    isChange = true;
}

int QCATLBarChart::MouseMove(int x, int y, WPARAM wParam)
{
    m_mousex = x;
    m_mousey = y;
    isChange = true;
    if (__isDown) {
        m_startPos = __lastXPt + x - m_rect.X;
        float w = 0;
        {
            std::shared_lock<std::shared_mutex> lk(m_lock);
            for (auto& it : m_data) {
                w += it.size() * m_bcWidth + m_gridWidth;

            }
        }

        int maxR = m_rect.Width - w;
        if (m_startPos >= 0) {
            m_startPos = 0;
            __lastXPt = m_rect.X - x;//拉不动了，重新计算按下位置以便于能迅速反方向拉

        }
        else if (m_startPos < maxR) {

            if (maxR <= 0) {
                //最后一个的底边不能小于控件底边
                m_startPos = maxR;
            }
            else {
                m_startPos = 0;
            }
            __lastXPt = m_rect.X + m_startPos - x;//拉不动了，重新计算按下位置以便于能迅速反方向拉
        }

        isChange = true;
    }


    return 0;
}

int QCATLBarChart::LButtonDown(int x, int y, WPARAM wParam)
{
    __isDown = true;
    int ret = 0;
    {
        // m_LDownPosY=y-m_rect.Y-RollPointVertical;
        __lastXPt = m_rect.X + m_startPos - x;
        ret = 1;//要求捕获鼠标

    }
    isChange = true;
    return ret;
}

int QCATLBarChart::LButtonUp(int x, int y, WPARAM wParam)
{
    __isDown = false;
    isChange = true;
    return 0;
}

int QCATLBarChart::MouseMoveOut()
{
    m_mousex = -1;
    m_mousey = -1;
    isChange = true;


    return 0;
}

int QCATLBarChart::MouseWheel(int x, int y, WPARAM wParam)
{
    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    int dx = x - m_rect.X;
    int dy = y - m_rect.Y;
    GRectF rect(m_ScaleWidth,0, GetXAxisLength(), GetYAxisLength());
    if (rect.Contains(dx, dy)) {
        m_bcWidth += zDelta * 0.01;
        m_gridWidth+= zDelta * 0.01;
        if (m_bcWidth <= 0)m_bcWidth = 1;
        if (m_gridWidth <= 0)m_gridWidth = 1;
        isChange = true;
    }
    else if (GRectF rect(0, 0, m_ScaleWidth, GetYAxisLength()); rect.Contains(dx, dy)) {
        
        m_YAxis_SW+= zDelta * 0.001;
        if (m_YAxis_SW <= 0)m_YAxis_SW = 1;
        isChange = true;
    }

    return 0;
}

void QCATLBarChart::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    QCtrlBase::Reconstruct(sdc, rect);
    ReconstructDC(m_curveDC, m_dc, m_curveHB, rect);
    m_ischange = true;
}





int QCATLBarChart::InsertBarChart(int grouId, int index,  CBCVT v, int color, const wchar_t* text, int tcolor)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size()-1 )return -1;//大于最大插入位置
    if (index > 0 && index > m_data[grouId].size())return -2;//大于最大插入位置
    if (-1 == index)index = m_data[grouId].size();//-1 为向尾插
    CATLBC c = { color ,v ,text ,tcolor };
    m_data[grouId].push_back(c);
    isChange = true;
    return 1;
}

int QCATLBarChart::InsertGroup(int grouId)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (grouId > 0 && grouId > m_data.size())return -1;//位置不合法
    if (-1 == grouId)grouId = m_data.size();
    m_data.insert(m_data.begin()+grouId,std::vector<CATLBC>());
    isChange = true;
    return 1;
}

int QCATLBarChart::RemoveGroup(int grouId)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    m_data.erase(m_data.begin()+grouId);
    isChange = true;
    return 1;
}

int QCATLBarChart::SetBarChartValue(int grouId, int index, CBCVT v)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    if (index < 0 || index > m_data[grouId].size() - 1 )return -2;//位置不合法
    m_data[grouId][index].v = v;
    isChange = true;
    return 1;
}

int QCATLBarChart::RemoveBarChart(int grouId, int index)
{
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    if (index < 0 || index > m_data[grouId].size() - 1 )return -2;//位置不合法
    m_data[grouId].erase(m_data[grouId].begin()+ index);
    isChange = true;
    return 1;
}

int QCATLBarChart::SetBarChartColor(int grouId, int index, unsigned int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    if (index < 0 || index > m_data[grouId].size() - 1 )return -2;//位置不合法
    m_data[grouId][index].color = color;
    isChange = true;
    return 1;
}
int QCATLBarChart::SetBarChartTextColor(int grouId, int index, int color)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    if (index < 0 || index > m_data[grouId].size() - 1 )return -2;//位置不合法
    m_data[grouId][index].tcolor = color;
    isChange = true;
    return 1;
}
int QCATLBarChart::SetBarChartText(int grouId, int index, const wchar_t* text)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (grouId < 0 || grouId > m_data.size() - 1 )return -1;//位置不合法
    if (index < 0 || index > m_data[grouId].size() - 1)return -2;//位置不合法
    m_data[grouId][index].text = text;
    isChange = true;
    return 1;
}

void QCATLBarChart::SetScaleColor(unsigned int color)
{
    m_ScaleColor = color;
    isChange = true;
}

void QCATLBarChart::SetScaleMax(float max)
{
    m_MaxSc = max;
    isChange = true;
}

void QCATLBarChart::SetScaleMin(float min)
{
    m_MinSc = min;
    isChange = true;
}

void QCATLBarChart::SetScaleWidth(float width)
{
    m_ScaleWidth = width;
    isChange = true;
}

void QCATLBarChart::SetScaleHeight(float height)
{
    m_ScaleHeight = height;
    isChange = true;
}

void QCATLBarChart::SetBarChartWidth(float width)
{
    m_bcWidth = width;
    isChange = true;
}

void QCATLBarChart::SetGridWidth(float width)
{
    m_gridWidth = width;
    isChange = true;
}

void QCATLBarChart::SetLineWidth(float width)
{
    m_lineWidth = width;
    isChange = true;
}

void QCATLBarChart::ClearBarChart()
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    m_data.clear();
    isChange = true;
}

int QCATLBarChart::GetGroupCount()
{
    std::shared_lock<std::shared_mutex> lk(m_lock);

    return m_data.size();
}

int QCATLBarChart::GetBarChartCount(int gID)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (gID < 0 || gID > m_data.size() - 1)return -1;
    return m_data[gID].size();
}

void QCATLBarChart::SetYAxisStrWidth(float width)
{
    m_YAxis_SW = width;
    isChange = true;
}



int QCATLBarChart::SetBarChart(int grouId, int index, CBCVT v, int color, const wchar_t* text, int tcolor)
{
    std::shared_lock<std::shared_mutex> lk(m_lock);
    if (grouId > m_data.size() - 1 || grouId < 0)return -1;//位置不合法
    if (index > m_data[grouId].size()-1 || index < 0)return -2;//位置不合法
    m_data[grouId][index]= { color ,v ,text ,tcolor };
    isChange = true;
    return 1;
}



void QCATLBarChart::Test()
{
    for (int i = -1; ++i < 20;) {
        InsertGroup(-1);
    }
    for (int i = -1; ++i < 20;) {
        InsertBarChart(i, -1, 50000, 0xfff00000);
        InsertBarChart(i, -1, 40000, 0xff00f000);
        InsertBarChart(i, -1, 30000, 0xff0000f0);
    }
    //std::vector<CATLBC> buf;
    //CATLBC c = { 0xffdd0000,65535.0,L"12",0xff000000 };
    //CATLBC d = { 0xff00ff00,30000.0,L"32546546",0xff000000 };
    //buf.push_back(c);
    //buf.push_back(d);
    //for (int i = -1; ++i < 20;) {
    //    m_data.push_back(buf);
    //}
}

inline double QCATLBarChart::GetYAxisBegin()
{
    return m_rect.Height - m_ScaleHeight;
}

inline double QCATLBarChart::GetXAxisEnd()
{
    return m_rect.Width ;
}

inline double QCATLBarChart::GetXAxisBegin()
{
    return m_ScaleWidth;
}
inline double QCATLBarChart::GetXAxisLength()
{
    return (double)m_rect.Width - m_ScaleWidth;//控件宽度减去Y轴字体宽度
}

inline double QCATLBarChart::GetYAxisLength()
{
    return (double)(m_rect.Height- m_ScaleHeight);//控件高度减去X轴字符高度
}

float QCATLBarChart::TransforPointY(float y)
{
    if (!(m_MaxSc - m_MinSc))return -1.0;
    double unitX = (GetYAxisLength()) / (m_MaxSc - m_MinSc);
    if (0 == unitX)return -1.0;
    return  (GetYAxisLength() - y) / unitX + m_MinSc;
}

#endif
//QCATLBarChart的导出函数定义
#if 1
extern "C"
{
    __declspec(dllexport) void QCBCSetFontName(QCATLBarChart* q, const wchar_t* name)
    {
        return q->SetFontName(name);
    }
    __declspec(dllexport) void QCBCSetFontSize(QCATLBarChart* q, float size)
    {
        return q->SetFontSize(size);
    }
    __declspec(dllexport) int QCBCInsertBarChart(QCATLBarChart* q, int grouId, int index, CBCVT v, int color, const wchar_t* text, int tcolor)
    {
    
        return q->InsertBarChart(grouId, index, v, color, text, tcolor);
    }
    __declspec(dllexport) int QCBCSetBarChart(QCATLBarChart* q, int grouId, int index, CBCVT v, int color, const wchar_t* text, int tcolor)
    {
        return q->SetBarChart(grouId, index, v, color, text, tcolor);
    }
    __declspec(dllexport) int QCBCSetBarChartValue(QCATLBarChart* q, int grouId, int index, CBCVT v)
    {
        return q->SetBarChartValue(grouId, index, v);
    }
    __declspec(dllexport) int QCBCSetBarChartColor(QCATLBarChart* q, int grouId, int index, unsigned int color)
    {
        return q->SetBarChartColor(grouId, index, color);
    }
    __declspec(dllexport) int QCBCSetBarChartText(QCATLBarChart* q, int grouId, int index, const wchar_t* text)
    {
        return q->SetBarChartText(grouId, index, text);
    }
    __declspec(dllexport) int QCBCSetBarChartTextColor(QCATLBarChart* q, int grouId, int index, unsigned int color)
    {
        return q->SetBarChartTextColor(grouId, index, color);
    }
    __declspec(dllexport) int QCBCInsertGroup(QCATLBarChart* q, int grouId)
    {
        return q->InsertGroup(grouId);
    }
    __declspec(dllexport) int QCBCRemoveGroup(QCATLBarChart* q, int grouId)
    {
        return q->RemoveGroup(grouId);
    }
    __declspec(dllexport)  int QCBCRemoveBarChart(QCATLBarChart* q, int grouId, int index)
    {
        return q->RemoveBarChart(grouId, index);
    }
    __declspec(dllexport) void QCBCSetScaleColor(QCATLBarChart* q, int color)
    {
        return q->SetScaleColor(color);
    }
    __declspec(dllexport) void QCBCSetScaleMax(QCATLBarChart* q, float max)
    {
        return q->SetScaleMax(max);
    }
    __declspec(dllexport) void QCBCSetScaleMin(QCATLBarChart* q, float min)
    {
        return q->SetScaleMin(min);
    }
    __declspec(dllexport) void QCBCSetScaleWidth(QCATLBarChart* q, float width)
    {
        return q->SetScaleWidth(width);
    }
    __declspec(dllexport) void QCBCSetScaleHeight(QCATLBarChart* q, float height)
    {
        return q->SetScaleHeight(height);
    }
    __declspec(dllexport) void QCBCSetBarChartWidth(QCATLBarChart* q, float width)
    {
        return q->SetBarChartWidth(width);
    }
    __declspec(dllexport) void QCBCSetGridWidth(QCATLBarChart* q, float width)
    {
        return q->SetGridWidth(width);
    }
    __declspec(dllexport) void QCBCSetLineWidth(QCATLBarChart* q, float width)
    {
        return q->SetLineWidth(width);
    }
    __declspec(dllexport) void QCBCSetYAxisStrWidth(QCATLBarChart* q, float width)
    {
        return q->SetYAxisStrWidth(width);
    }
    __declspec(dllexport) void QCBCClearBarChart(QCATLBarChart* q)
    {
        return q->ClearBarChart();
    }
    __declspec(dllexport) int QCBCGetGroupCount(QCATLBarChart* q)
    {
        return q->GetGroupCount();
    }
    __declspec(dllexport) int QCBCGetBarChartCount(QCATLBarChart* q,int gID)
    {
        return q->GetBarChartCount(gID);
    }


}
#endif

//QCATLTemperatureChart的函数定义
#if 1
void DrawRoundRect(Gdiplus::Graphics& graphics, int x, int y, int width, int height, int radius, GColor color, int penWidth)
{
    // 创建圆角矩形路径
    Gdiplus::GraphicsPath path;
    path.AddArc(x, y, 2 * radius, 2 * radius, 180, 90); // 左上角圆弧
    path.AddArc(x + width - 2 * radius, y, 2 * radius, 2 * radius, 270, 90); // 右上角圆弧
    path.AddArc(x + width - 2 * radius, y + height - 2 * radius, 2 * radius, 2 * radius, 0, 90); // 右下角圆弧
    path.AddArc(x, y + height - 2 * radius, 2 * radius, 2 * radius, 90, 90); // 左下角圆弧
    path.CloseFigure(); // 封闭路径

    // 创建画笔并绘制路径
    GPen pen(color, penWidth);
    graphics.DrawPath(&pen, &path);
}

QCATLTemperatureChart::QCATLTemperatureChart(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    //Test();
}

QCATLTemperatureChart::~QCATLTemperatureChart()
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
}

int QCATLTemperatureChart::Draw(HDC sdc)
{
    bool flag = 0;

    //添加绘制代码
    if (isChange) {
        flag = 1;
        isChange = false;
        //1.sdc中对应的位置拷贝过来
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);

        //2.绘制温度图表
        {
            Gdiplus::Graphics graphics(m_bdc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
            Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
            {
                std::shared_lock<std::shared_mutex> lk(m_lock);
                float x = 0;
                float y = m_startPos;
                float lastW = 0; //上一组的宽度
                float lastH = 0; //上一组的高度
                float totalH = 0;//总高度
                for (auto& it : m_data) {
                    int row = std::ceil(it.ctc.size() / (it.m_lineCount*1.0));
                    int col = it.m_lineCount;
                    
                    if (row < 2)col = it.ctc.size();
                    //计算长度
                    float w = col * m_UnitWidth + (col - 1) * m_UnitIntervalWidth;
                    float h = (row + 1) * m_UnitWidth + row  * m_UnitIntervalWidth;
                    if (x || lastW) {
                        if (x+ lastW + w + m_AlignWidth * 2 < m_rect.Width) {
                            x += lastW;
                        }
                        else {
                            x = 0;
                            lastW = 0;
                            y += lastH;
                            totalH += lastH;
                            lastH = 0;
                        }
                    
                    }


                    //GRectF(x,y,w+m_AlignWidth*2, h +m_AlignWidth * 2);
                    DrawRoundRect(graphics,x,y, w + m_AlignWidth * 2, h + m_AlignWidth * 2, m_AlignWidth,0xff00ffff,1.0);

                    float ux = x + m_AlignWidth;
                    float uy = y + m_AlignWidth;
                    GRectF rect(0, 0, m_UnitWidth, m_UnitWidth);
                    for (int i = -1; ++i < it.ctc.size();) {
                        auto& u = it.ctc[i];
                        rect.X = ux;
                        rect.Y = uy;
                        gsb.SetColor(u.color);
                        graphics.FillRectangle(&gsb, rect);
                        if (!((i+1) % (it.m_lineCount ))) {
                            //换行
                            ux= x + m_AlignWidth;
                            uy += m_UnitWidth + m_UnitIntervalWidth;
                        
                        }else {
                            ux += m_UnitWidth + m_UnitIntervalWidth;
                        }
                        if (L"" != u.text) {
                            GStringFormat gsf(0, 0);
                            gsf.SetAlignment(sAlign);//水平居中
                            gsf.SetLineAlignment(slAlign);//垂直居中
                            gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                            GFont drawFont(m_fontName.c_str(), m_ufontSize);
                            gsb.SetColor(u.tcolor);
                            graphics.DrawString(u.text.c_str(), (INT)u.text.length(), &drawFont, rect, &gsf, &gsb);
                        }
                    }
                    //绘制字符
                    rect.X = x + m_AlignWidth;
                    rect.Y = y + m_AlignWidth+ h- m_UnitWidth;

                    rect.Width = w;
                    rect.Height = m_UnitWidth;

                    if (L"" != it.name) {
                        GStringFormat gsf(0, 0);
                        gsf.SetAlignment(sAlign);//水平居中
                        gsf.SetLineAlignment(slAlign);//垂直居中
                        gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                        GFont drawFont(m_fontName.c_str(), m_fontSize);
                        gsb.SetColor(0xff000000);
                        graphics.DrawString(it.name.c_str(), (INT)it.name.length(), &drawFont, rect, &gsf, &gsb);
                    }

                    lastW = w + m_AlignWidth * 2;
                    if (lastH < h + m_AlignWidth * 2) {
                        lastH = h + m_AlignWidth * 2;
                    }
                }
                
                totalH += lastH;
                
                m_totalHeight = totalH;
            }

           
        }



        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);

    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;
   
}

int QCATLTemperatureChart::MouseMove(int x, int y, WPARAM wParam)
{
    isChange = true;
    if (__isDown) {
        m_startPos = __lastYPt + y - m_rect.Y;
        float h = m_totalHeight;


        int maxR = m_rect.Height - h;
        if (m_startPos >= 0) {
            m_startPos = 0;
            __lastYPt = m_rect.Y - y;//拉不动了，重新计算按下位置以便于能迅速反方向拉

        }
        else if (m_startPos < maxR) {

            if (maxR <= 0) {
                
                m_startPos = maxR;
            }
            else {
                m_startPos = 0;
            }
            __lastYPt = m_rect.Y + m_startPos - y;//拉不动了，重新计算按下位置以便于能迅速反方向拉
        }

        isChange = true;
    }

    return 0;
}

int QCATLTemperatureChart::LButtonDown(int x, int y, WPARAM wParam)
{
    __isDown = true;
    int ret = 0;
    {
        // m_LDownPosY=y-m_rect.Y-RollPointVertical;
        __lastYPt =    m_rect.Y + m_startPos- y ;
        ret = 1;//要求捕获鼠标

    }
    isChange = true;
    return ret;
}

int QCATLTemperatureChart::LButtonUp(int x, int y, WPARAM wParam)
{
    __isDown = false;
    isChange = true;
    return 0;
}
int QCATLTemperatureChart::MouseWheel(int x, int y, WPARAM wParam)
{

    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    m_startPos += zDelta;
    int maxR = m_rect.Height - m_totalHeight;

    if (m_startPos > 0) {
        m_startPos = 0;
    }
    else if (m_startPos < maxR) {
        if (maxR <= 0) {
            //最后一个的底边不能小于控件底边
            m_startPos = maxR;
        }
        else {
            m_startPos = 0;
        }


    }
    isChange = true;
    return 0;
}
void QCATLTemperatureChart::Reconstruct(HDC sdc, const GRect& rect)
{

    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    QCtrlBase::Reconstruct(sdc, rect);

}
void QCATLTemperatureChart::SetFontName(const wchar_t* name)
{
    m_fontName = name;
    isChange = true;
}

void QCATLTemperatureChart::SetFontSize(float size)
{
    m_fontSize = size;
    isChange = true;
}
void QCATLTemperatureChart::SetUnitWidth(float w)
{
    m_UnitWidth = w;
}

void QCATLTemperatureChart::SetUnitIntervalWidth(float w)
{
    m_UnitIntervalWidth = w;
}
int QCATLTemperatureChart::InsertGroup(int gID)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (-1 < gID && gID > m_data.size())return -1;
    if (-1 == gID) gID = m_data.size();
    m_data.insert(m_data.begin()+gID,CTempChartGroup());

    return 1;
}
int QCATLTemperatureChart::RemoveGroup(int gID)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0< gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    m_data.erase(m_data.begin() + gID);

    return 1;
}
void QCATLTemperatureChart::SetAlignWidth(float w)
{
    m_AlignWidth = w;
}

void QCATLTemperatureChart::SetUnitFontSize(float size)
{
    m_ufontSize = size;
    isChange = true;
}
void QCATLTemperatureChart::Test()
{
    CTempChart ctc = {0xffff0000,L"100℃",0xff000000};
    CTempChart ctc2 = { 0xff00f000,L"75℃",0xff000000 };
    CTempChart ctc3 = { 0xff00f0f0,L"50℃",0xff000000 };
    CTempChart ctc4 = { 0xff0000ff,L"0℃",0xff000000 };
    CTempChart ctc5 = { 0xff00ffff,L"20℃",0xff000000 };
    CTempChart ctc6 = { 0xffff00ff,L"60℃",0xff000000 };
    CTempChartGroup ctcg;

    ctcg.m_lineCount = 5;
    ctcg.name = L"测试组1";
    ctcg.ctc.push_back(ctc);
    ctcg.ctc.push_back(ctc2);
    ctcg.ctc.push_back(ctc3);
    ctcg.ctc.push_back(ctc4);
    ctcg.ctc.push_back(ctc5);
    ctcg.ctc.push_back(ctc6);
    CTempChartGroup ctcg2 =  ctcg;
    ctcg2.ctc.push_back(ctc2);
    ctcg2.ctc.push_back(ctc3);
    ctcg2.ctc.push_back(ctc4);
    ctcg2.ctc.push_back(ctc5);
    ctcg2.ctc.push_back(ctc6);
    m_data.push_back(ctcg);
    m_data.push_back(ctcg2);
    m_data.push_back(ctcg);
    m_data.push_back(ctcg);
    ctcg.ctc.push_back(ctc6);
    ctcg.ctc.push_back(ctc6);
    ctcg.ctc.push_back(ctc6);
    ctcg.ctc.push_back(ctc6);
    ctcg.ctc.push_back(ctc6);
    m_data.push_back(ctcg);

    m_data.push_back(ctcg2);
    m_data.push_back(ctcg2);
    m_data.push_back(ctcg2);
    m_data.push_back(ctcg2);
    m_data.push_back(ctcg2);

}

int QCATLTemperatureChart::InsertUnit(int gID, int uID, int color, const wchar_t* text, int tcolor)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (-1 == uID) uID = unit.size();
    unit.insert(unit.begin() + uID, CTempChart(color, text, tcolor));


    return 1;
}

int QCATLTemperatureChart::SetUnit(int gID, int uID, int color, const wchar_t* text, int tcolor)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (0 > uID)return -4;
    unit[uID].color = color;
    unit[uID].text = text;
    unit[uID].tcolor = tcolor;
    return 1;
}

int QCATLTemperatureChart::RemoveUnit(int gID, int uID)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (0 > uID)return -4;
    unit.erase(unit.begin()+uID);
    return 0;
}

int QCATLTemperatureChart::SetUnitColor(int gID, int uID, int color)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (0 > uID)return -4;
    unit[uID].color = color;


    return 1;
}

int QCATLTemperatureChart::SetUnitText(int gID, int uID, const wchar_t* text)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (0 > uID)return -4;
    unit[uID].text = text;


    return 1;
}

int QCATLTemperatureChart::SetUnitTextColor(int gID, int uID, int color)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    auto& unit = m_data[gID].ctc;
    if (-1 < uID && uID > unit.size())return -3;
    if (0 > uID)return -4;
    unit[uID].tcolor = color;


    return 1;
}
int QCATLTemperatureChart::SetGroupLineCount(int gID, int count)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0) {
        for (auto& it : m_data) {
            it.m_lineCount = count;
        
        }
    
    }
    else {
        m_data[gID].m_lineCount = count;
    
    }
  


    return 1;
}

int QCATLTemperatureChart::SetGroupName(int gID, const wchar_t* name)
{
    std::unique_lock<std::shared_mutex> lk(m_lock);
    if (0 < gID && gID > m_data.size())return -1;
    if (gID < 0)return -2;
    m_data[gID].name = name;
    return 1;
}
#endif

//QCATLTemperatureChart的导出函数定义
#if 1
extern "C"
{
    __declspec(dllexport) void QCTCSetFontName(QCATLTemperatureChart* p, const wchar_t* name)
    {
    
        return p->SetFontName(name);
    }
    __declspec(dllexport) void QCTCSetFontSize(QCATLTemperatureChart* p, float size)
	{
        return p->SetFontSize(size);
	}
    __declspec(dllexport) void QCTCSetUnitFontSize(QCATLTemperatureChart* p, float size)
	{
        return p->SetUnitFontSize(size);
	}
    __declspec(dllexport) void QCTCSetUnitWidth(QCATLTemperatureChart* p, float w)
	{
        return p->SetUnitWidth(w);
	}
    __declspec(dllexport) void QCTCSetUnitIntervalWidth(QCATLTemperatureChart* p, float w)
	{
        return p->SetUnitIntervalWidth(w);
	}
    __declspec(dllexport) void QCTCSetAlignWidth(QCATLTemperatureChart* p, float w)
	{
        return p->SetAlignWidth(w);
	}
    __declspec(dllexport) int QCTCInsertGroup(QCATLTemperatureChart* p, int gID)
	{
        return p->InsertGroup(gID);
	}
    __declspec(dllexport) int QCTCRemoveGroup(QCATLTemperatureChart* p, int gID)
	{
        return p->RemoveGroup(gID);
	}
    __declspec(dllexport) int QCTCInsertUnit(QCATLTemperatureChart* p, int gID, int uID, int color, const wchar_t* text , int tcolor )
	{
        return p->InsertUnit(gID, uID, color, text, tcolor);
	}
    __declspec(dllexport) int QCTCRemoveUnit(QCATLTemperatureChart* p, int gID, int uID)
	{
        return p->RemoveUnit(gID, uID);
	}

    __declspec(dllexport) int QCTCSetUnit(QCATLTemperatureChart* p, int gID, int uID, int color, const wchar_t* text , int tcolor )
	{
        return p->SetUnit(gID, uID, color, text, tcolor);
	}

    __declspec(dllexport) int QCTCSetUnitColor(QCATLTemperatureChart* p, int gID, int uID,unsigned int color)
	{
        return p->SetUnitColor(gID, uID, color);
	}

    __declspec(dllexport) int QCTCSetUnitText(QCATLTemperatureChart* p, int gID, int uID, const wchar_t* text)
	{
        return p->SetUnitText(gID, uID, text);
	}

    __declspec(dllexport) int QCTCSetUnitTextColor(QCATLTemperatureChart* p, int gID, int uID, int color)
	{
        return p->SetUnitTextColor(gID, uID, color);
	}

    __declspec(dllexport) int QCTCSetGroupLineCount(QCATLTemperatureChart* p, int gID,  int count)
	{
        return p->SetGroupLineCount(gID, count);
	}

    __declspec(dllexport) int QCTCSetGroupName(QCATLTemperatureChart* p, int gID, const wchar_t* name)
	{
        return p->SetGroupName(gID, name);
	}


}

#endif








