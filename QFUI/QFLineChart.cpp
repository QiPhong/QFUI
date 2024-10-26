
#include "QFLineChart.h"
#include <format>
#include <algorithm>
#include "QFValuation.h"
inline void QLineChart::QCreateDC(HDC winddc)
{
    //创建曲线DC
    m_curveDC = CreateCompatibleDC(winddc);
    m_curveHB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_curveDC, m_curveHB);
}
inline void QLineChart::ReleaseDC()
{
    DeleteObject(m_curveHB);
    m_curveHB = NULL;
    DeleteDC(m_curveDC);
    m_curveDC = NULL;
}
QLineChart::QLineChart(HDC winddc, const GRect& rf):QCtrlBase(winddc, rf)
{
    QCreateDC(winddc);
    m_bkBrush = new GSolidBrush(GColor(0xaf, 0xaa, 0xaa, 0xaa));


    //测试代码
    //std::vector<GPointF> srcPF;

    //for (GREAL x = 0.0; x < 20; x += 3.14 / 8) {
    //    srcPF.push_back(GPointF(x, std::sin(x)));
    //}
    //std::vector<GPointF> gpf = PointTransfor(srcPF);
    //QCurve qc;
    //qc.data = gpf;
    //qc.srcData = srcPF;
    //qc.color = 0xff000000;
    //m_curve_data.push_back(qc);


    //std::vector<GPointF> srcPF2{ {0,0},{1,0.01},{3,0.01},{5,0.05},{7,0.06},{8,0.09},{9,0.15},{10,0.08},{11,0.21},{13,0.23},{15,0.29},{16,0.12},{17,0.20}
    //,{18,0.40},{19,0.50},{20,0.60} ,{21,0.80} ,{25,1.0} };
    //gpf = PointTransfor(srcPF2);
    //qc.data = gpf;
    //qc.srcData = srcPF2;
    //qc.color = 0xffff0000;
    //m_curve_data.push_back(qc);
    //srcPF.clear();
    //for (GREAL x = 0.0; x < 20; x += 3.14 / 8) {
    //    srcPF.push_back(GPointF(x, std::cos(x)));
    //}
    //gpf = PointTransfor(srcPF);
    //qc.data = gpf;
    //qc.srcData = srcPF;
    //qc.color = 0xff0000fe;
    //qc.isVisible = false;
    //m_curve_data.push_back(qc);
    m_Attribute.uct.isHHCurveLeg = 1;//隐藏不显示的曲线图例
}

QLineChart::~QLineChart()
{
    ReleaseDC();
}



#include <cmath>
int QLineChart::Draw(HDC sdc)
{
    bool flag = 0;

    //添加绘制代码
    if (isChange) {
        std::unique_lock<std::mutex> lk(m_drawLock);
        flag = 1;
        isChange = false;
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
        //3.绘制坐标轴
        {
            Gdiplus::Graphics graphics(m_dc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
            
            //GSolidBrush gs(GColor(0x8f, 0xff, 0xff, 0xff));


            GPen pen(GColor(0xff, 0, 0, 0), m_lineWidth);
            graphics.DrawLine(&pen, GPointF(m_YAxis_TW,0.0), GPointF(m_YAxis_TW, (float)m_rect.Height- m_XAxis_TW));
            graphics.DrawLine(&pen,GPointF(m_YAxis_TW, GetYAxisBegin()), GPointF(GetXAxisEnd(), GetYAxisBegin()));
            //绘制x轴刻度
            double TotalSize = m_XAxis_MaxV - m_XAxis_MinV;//
            float tLen = GetXAxisLength();//绘制的总长度
            float sL = m_XAxis_TW * 0.1;//小刻度的长度
            float mL = m_XAxis_TW * 0.3;//大刻度的长度
            float sy = m_rect.Height - m_XAxis_TW;//x轴开始的y坐标
            if (m_lineWidth*2 > m_XAxis_SW)m_XAxis_SW = m_lineWidth * 2;
            double TotaSeg = tLen / m_XAxis_SW;    //总段数
            double SegLen = TotalSize / TotaSeg;  //每一段自增的值

            int i = 0;
            for (float cp = GetXAxisBegin(); cp < GetXAxisEnd(); cp += m_XAxis_SW,++i) {
                GRectF tRect(m_YAxis_TW * 0.7, sy + mL, m_YAxis_TW * 0.3 + m_XAxis_SW * 2.5, m_XAxis_TW - mL);
                if (!(i % 5)) {
                    graphics.DrawLine(&pen, GPointF(cp, sy), GPointF(cp, sy+ mL));
                    if (0 != i) {
                        tRect = GRectF(m_YAxis_TW + m_XAxis_SW * 2.5 * (2 * i/5 - 1), sy + mL, m_XAxis_SW * 5, m_XAxis_TW - mL);

                    }
                    GStringFormat gsf(0, 0);
                    gsf.SetAlignment(sAlign);//水平居中
                    gsf.SetLineAlignment(slAlign);//垂直居中
                    gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                    GFont drawFont(m_fontName.c_str(), m_fontSize);
                    Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                    std::wstring str = std::format(L"{:.2f}", m_XAxis_MinV + SegLen * i);

                    graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, tRect, &gsf, &gsb);
                }
                else {
                    graphics.DrawLine(&pen, GPointF(cp, sy), GPointF(cp, sy + sL));
   
                }

            }
            //绘制y轴刻度
            TotalSize = m_YAxis_MaxV - m_YAxis_MinV;//
            tLen = GetYAxisLength();//绘制的总长度
            sL = m_YAxis_TW * 0.1;//小刻度的长度
            mL = m_YAxis_TW * 0.3;//大刻度的长度
            float sx = m_YAxis_TW;//y轴开始的x坐标
            if (m_lineWidth * 2 > m_YAxis_SW)m_YAxis_SW = m_lineWidth * 2;
            TotaSeg = tLen / m_YAxis_SW;    //总段数
            SegLen = TotalSize / TotaSeg;  //每一段自增的值
            i = 0;
   
            GFont drawFont(m_fontName.c_str(), m_fontSize);
            GREAL FontHeight = drawFont.GetHeight(&graphics);
            for (float cp = tLen; cp > m_YAxis_SW; cp -= m_YAxis_SW, ++i) {
                if (!(i % 5)) {
                    graphics.DrawLine(&pen, GPointF(sx, cp), GPointF(sx - mL, cp));
                
                    GRectF tRect(0,cp- FontHeight/2, m_YAxis_TW - mL, FontHeight);

          
                    GStringFormat gsf(0, 0);
                    gsf.SetAlignment(sAlign);//水平居中
                    gsf.SetLineAlignment(slAlign);//垂直居中
                    gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                    GFont drawFont(m_fontName.c_str(), m_fontSize);
                    Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                    std::wstring str = std::format(L"{:.2f}", m_YAxis_MinV + SegLen * i);

                    graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, tRect, &gsf, &gsb);
                }
                else {
                    graphics.DrawLine(&pen, GPointF(sx, cp), GPointF(sx - sL,cp));
                }
            }


        }
        //绘制曲线
        {

           StretchBlt(m_curveDC, 0, 0, m_rect.Width , m_rect.Height ,
               m_dc, 0, 0, m_rect.Width , m_rect.Height , SRCCOPY);
            Gdiplus::Graphics graphics(m_curveDC);
   
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            GPen pen(GColor(0xff, 0, 0, 0), m_lineWidth);
            GPen pen1(GColor(0xff, 0, 0, 0));
            pen1.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDotDot);
            GRectF drawRect(m_YAxis_SW,0,m_rect.Width- m_YAxis_SW,m_rect.Height-m_XAxis_TW);
            GFont drawFont(m_fontName.c_str(), m_fontSize);
            GStringFormat gsf(0, 0);
            gsf.SetAlignment(sAlign);//水平居中
            gsf.SetLineAlignment(slAlign);//垂直居中
            gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
            Gdiplus::SolidBrush gsb(GColor(0,0,0,0));
         
            std::unique_lock lock(m_mutex_CtrlCurve);
            for (auto& it : m_curve_data) {
                if (!it.isVisible)continue;
                pen.SetColor(it.color);
                graphics.DrawCurve(&pen, it.data.data(), it.data.size());
                double y = m_mousey - m_rect.GetTop();
                double x = m_mousex - m_rect.GetLeft();
                
                if (!m_Attribute.uct.isCloseTracking && drawRect.Contains(x , y) && 0 < it.data.size()
                    && x >= it.data[0].X 
                    && x  <= it.data[it.data.size() - 1].X) {
                    pen1.SetColor(it.color);

                    double px= TransforPointX(x);
                    //auto y_ = QF::predict(px, it.coefficients);
                    auto y_ = QF::GetCurveYValue(it.srcData,px);
                    y= PointYTransfor(y_);
                    graphics.DrawLine(&pen1,GPointF(0,y), GPointF(x, y));
                    graphics.DrawLine(&pen1, GPointF(x, y), GPointF(x, m_rect.Height-m_XAxis_TW));

                    
                    
                    std::wstring str = std::format(L"({:.2f}，{:.2f})", px,y_);
                    gsb.SetColor(it.color);
                    GPointF ptf(x,y);
                    graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, ptf, &gsf, &gsb);
                }
            
            }
       
            StretchBlt(m_dc, m_YAxis_TW, 0, m_rect.Width - m_YAxis_TW, m_rect.Height - m_XAxis_TW,
                m_curveDC, m_YAxis_TW, 0, m_rect.Width - m_YAxis_TW, m_rect.Height - m_XAxis_TW, SRCCOPY);
        }
        
        //绘制图例
        {
            
            Gdiplus::Graphics graphics(m_dc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
            GFont drawFont(m_legFontName.c_str(), m_legFontSize);
            GREAL FontHeight = drawFont.GetHeight(&graphics);
            GStringFormat gsf(0, 0);
            gsf.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//水平居中
            gsf.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);//垂直居中
            gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);

            GRectF tRect(m_rect.Width-m_LegendW,0, m_LegendW*0.2, FontHeight);
            GSolidBrush gb(GColor(0,0,0,0));
            GRectF tRect_text(m_rect.Width - m_LegendW*0.8, 0, m_LegendW * 0.8, FontHeight);
            GPen pen(GColor(0,0,0), m_lineWidth);
            std::unique_lock lock(m_mutex_CtrlCurve);
            for (int i = -1,j=-1; ++i < m_curve_data.size();) {
                const auto& it = m_curve_data[i];
                if (m_Attribute.uct.isHHCurveLeg && !it.isVisible)continue;
                ++j;
                tRect.Y = FontHeight * j;
                tRect_text.Y = FontHeight * j;
                gb.SetColor(it.color);
                graphics.FillRectangle(&gb, tRect);
                graphics.DrawString(it.name.c_str(),-1, &drawFont, tRect_text, &gsf,&gb);
                if (!it.isVisible) {
                    pen.SetColor(it.color);
                    graphics.DrawLine(&pen,GPointF(tRect.X,tRect.Y), GPointF(tRect.X+ m_LegendW, tRect.Y+ FontHeight));
                    graphics.DrawLine(&pen, GPointF(tRect.X + m_LegendW, tRect.Y), GPointF(tRect.X , tRect.Y + FontHeight));
                }
            
            }
        
        
        }

    



      

    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;
}

int QLineChart::MouseMove(int x, int y, WPARAM wParam)
{
    m_mousex = x;
    m_mousey = y;
    isChange = true;
    return 0;
}

int QLineChart::LButtonDown(int x, int y, WPARAM wParam)
{
    return 0;
}

int QLineChart::MouseMoveOut()
{
    m_mousex = -1;
    m_mousey = -1;
    isChange = true;
    return 0;
}

void QLineChart::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    std::unique_lock<std::mutex> lk(m_drawLock);
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;

    //QCtrlBase::Reconstruct(sdc, rect);
    //ReleaseDC();

    //QCreateDC(sdc);
    Reconstruct(sdc);
    
}

void QLineChart::Reconstruct(HDC sdc)
{
    QCtrlBase::Reconstruct(sdc);
 /*   ReleaseDC();
    QCreateDC(sdc);*/
    ReconstructDC(m_curveDC, m_dc, m_curveHB, m_rect);
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data= PointTransfor(it.srcData);
    }
    isChange = true;
   
}



void QLineChart::SetFontName(const wchar_t* name)
{
    m_fontName = name;

    isChange = true;
}

void QLineChart::SetFontSize(INT size)
{
    m_fontSize = size;
    isChange = true;
}

void QLineChart::SetTextAlgn(const Gdiplus::StringAlignment& align)
{
    sAlign = align;

    isChange = true;
}

void QLineChart::SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign)
{
    sAlign = align;
    slAlign = lalign;

    isChange = true;
}

void QLineChart::SetTextAllgn(const Gdiplus::StringAlignment& lalign)
{
    slAlign = lalign;

    isChange = true;
}

void QLineChart::SetLegFontName(const wchar_t* name)
{
    m_legFontName = name;
    isChange = true;
}

void QLineChart::SetLegFontSize(INT size)
{
    m_legFontSize = size;
    isChange = true;
}

void QLineChart::SetXAxisStrHeight(double h)
{
    if (m_rect.Height < h)return;
    m_XAxis_TW = h;
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data = PointTransfor(it.srcData);

    }
    isChange = true;
}

void QLineChart::SetYAxisStrWidth(double w)
{
    if (m_rect.Width - m_YAxis_TW - w < 0)return;
    m_YAxis_TW = w;
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data = PointTransfor(it.srcData);

    }
    isChange = true;
}

void QLineChart::SetXAxisScaleWidth(double w)
{
    m_XAxis_SW = w;

}

void QLineChart::SetYAxisScaleWidth(double w)
{
    
    m_YAxis_SW = w;

}

void QLineChart::SetLineWidth(double w)
{
    m_lineWidth = w;

}

void QLineChart::SetXAxisRange(double m, double M)
{
    if (m >= M)return;
    m_XAxis_MinV = m;
    m_XAxis_MaxV = M;
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data = PointTransfor(it.srcData);
    
    }
    isChange = true;
}

void QLineChart::SetYAxisRange(double m, double M)
{
    if (m >= M)return;
    m_YAxis_MinV = m;
    m_YAxis_MaxV = M;
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data = PointTransfor(it.srcData);

    }
    isChange = true;
}

void QLineChart::SetLegWidth(double w)
{
    if (m_rect.Width - m_YAxis_TW - w < 0)return;
    m_LegendW = w;
    std::unique_lock lock(m_mutex_CtrlCurve);
    for (auto& it : m_curve_data) {
        it.data = PointTransfor(it.srcData);

    }
    isChange = true;
}

void QLineChart::SetAttribute(long long v)
{
    m_Attribute.value = v;
    isChange = true;
}

int QLineChart::AddCurve(GPointF* pt, int len, unsigned long color, const wchar_t* name)
{
    QCurve qc;
    //qc.srcData.resize(len);
    //qc.srcData.reserve(0);
    qc.color = color;
    for (int i = -1; ++i < len;) {
        InsertPoint(qc.srcData, pt[i]);
    }
    qc.data= PointTransfor(qc.srcData);
    if (name)qc.name = name;
    std::unique_lock lock(m_mutex_CtrlCurve);
    m_curve_data.push_back(qc);
    isChange = true;
    return m_curve_data.size() - 1;
}

int QLineChart::FindCurve(const wchar_t* name, int start)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (start<0 || start>m_curve_data.size() - 1)return -1;
    std::vector<QCurve>::const_iterator it =std::find_if(m_curve_data.cbegin()+start, m_curve_data.cend(), [&name](const QCurve& qc) {return qc.name == name; });
    if (it == m_curve_data.cend())return -1;
    
    return it- m_curve_data.cbegin();

}

void QLineChart::SetCurveColor(int index, unsigned long color)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (index<0 || index > m_curve_data.size() - 1)return;
    m_curve_data[index].color = color;
    isChange = true;
}

void QLineChart::SetCurveName(int index,const wchar_t* name)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (index<0 || index > m_curve_data.size() - 1)return;
    m_curve_data[index].name = name;
    isChange = true;
}

void QLineChart::SetCurveVisible(int index, bool isVisible)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (index<0 || index > m_curve_data.size() - 1)return;
    m_curve_data[index].isVisible = isVisible;
    isChange = true;
}

void QLineChart::CurveInsertPoint(int index, const GPointF& pt)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (index<0 || index > m_curve_data.size() - 1)return;
    InsertPoint(m_curve_data[index].srcData, pt);
    InsertPoint(m_curve_data[index].data, GPointF(PointXTransfor(pt.X), PointYTransfor(pt.Y)));
    isChange = true;
}

void QLineChart::RemoveCurve(int index)
{
    std::unique_lock lock(m_mutex_CtrlCurve);
    if (index<0 || index > m_curve_data.size() - 1)return;
    m_curve_data.erase(m_curve_data.begin()+ index);
    isChange = true;
}

void QLineChart::SetBackground(GBrush* brush)
{
    if (m_bkBrush) {
        delete m_bkBrush;
        m_bkBrush = NULL;
    
    }
    m_bkBrush = brush->Clone();
    m_isUpdateBK = true;
    isChange = true;
}



std::vector<GPointF> QLineChart::PointTransfor(const std::vector<GPointF>& data)
{
    std::vector<GPointF> ret;
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return ret;
    double unitX = (GetXAxisLength()) / (m_XAxis_MaxV - m_XAxis_MinV);
    double unitY = (GetYAxisLength()) / (m_YAxis_MaxV - m_YAxis_MinV);
    std::for_each(data.cbegin(), data.cend(), [&](const GPointF& p) {
        ret.push_back(GPointF(GetXAxisBegin() + (p.X - m_XAxis_MinV) * unitX, GetYAxisBegin() - (p.Y - m_YAxis_MinV) * unitY));
        });


    return ret;
}

std::vector<GPointF> QLineChart::PointTransfor(const GPointF* data, int len)
{
    std::vector<GPointF> ret;
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return ret;
    double unitX = (GetXAxisLength()) / (m_XAxis_MaxV - m_XAxis_MinV);
    double unitY = (GetYAxisLength()) / (m_YAxis_MaxV - m_YAxis_MinV);
    
    for (int i = -1; ++i < len;) {
        ret.push_back(GPointF(GetXAxisBegin() +(data[i].X - m_XAxis_MinV) * unitX, GetYAxisBegin()-(data[i].Y - m_YAxis_MinV) * unitY));
    }
    return ret;
}

double QLineChart::PointXTransfor(double x)
{
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return -1.0;
    double unitX = (GetXAxisLength()) / (m_XAxis_MaxV - m_XAxis_MinV);
    return GetXAxisBegin() + (x - m_XAxis_MinV) * unitX;
}

double QLineChart::PointYTransfor(double y)
{
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return -1.0;
    double unitY = (GetYAxisLength()) / (m_YAxis_MaxV - m_YAxis_MinV);
    return GetYAxisBegin() - (y - m_YAxis_MinV) * unitY;
}

double QLineChart::TransforPointX(double x)
{
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return -1.0;
    double unitX = (GetXAxisLength()) / (m_XAxis_MaxV - m_XAxis_MinV);
    if (0 == unitX)return -1.0;

    return (x- GetXAxisBegin())/ unitX+ m_XAxis_MinV;
}

double QLineChart::TransforPointY(double y)
{
    if (!(m_XAxis_MaxV - m_XAxis_MinV) || !(m_YAxis_MaxV - m_YAxis_MinV))return -1.0;
    double unitY = (GetYAxisLength()) / (m_YAxis_MaxV - m_YAxis_MinV);
    if (0 == unitY)return -1.0;


    return m_YAxis_MinV-(y- GetYAxisBegin())/ unitY;
}

inline double QLineChart::GetXAxisLength()
{
    return (double)m_rect.Width - m_YAxis_TW-m_LegendW;//控件宽度减去Y轴字体宽度和图例宽度
}

inline double QLineChart::GetXAxisBegin()
{
    return m_YAxis_TW;
}

inline double QLineChart::GetXAxisEnd()
{
    return m_rect.Width-m_LegendW;
}

inline double QLineChart::GetYAxisLength()
{
    return (double)m_rect.Height - m_XAxis_TW;//控件高度减去X轴字符高度
}

inline double QLineChart::GetYAxisBegin()
{
    return m_rect.Height-m_XAxis_TW;
}

inline double QLineChart::GetYAxisEnd()
{
    return 0.0;
}

void QLineChart::InsertPoint(std::vector<GPointF>& vpt, const GPointF& ppt)
{
    auto it = std::lower_bound(vpt.begin(), vpt.end(), ppt, [](const GPointF& pt1, const GPointF& pt2) {return pt1.X < pt2.X; });

    if (it != vpt.end() && it->X == ppt.X) {
        it->Y = ppt.Y; // Update the y value if x is the same
    }
    else {
        vpt.insert(it, ppt); // Insert new point
    }
   
}


extern "C"
__declspec(dllexport) void QFLCSetBackground(QLineChart * qw, GBrush* brush)
{
    return qw->SetBackground(brush);
}

extern "C"
__declspec(dllexport) void QFLCRemoveCurve(QLineChart * qw, int index)
{
    return qw->RemoveCurve(index);
}

extern "C"
__declspec(dllexport) void QFLCSetCurveVisible(QLineChart * qw,int index, bool isVisible)
{
    return qw->SetCurveVisible(index,isVisible);
}

extern "C"
__declspec(dllexport) void QFLCSetXAxisStrHeight(QLineChart * qw, double w)
{
    return qw->SetXAxisStrHeight(w);
}


extern "C"
__declspec(dllexport) void QFLCSetYAxisStrWidth(QLineChart * qw, double w)
{
    return qw->SetYAxisStrWidth(w);
}

extern "C"
__declspec(dllexport) void QFLCSetXAxisScaleWidth(QLineChart * qw, double w)
{
    return qw->SetXAxisScaleWidth(w);
}

extern "C"
__declspec(dllexport) void QFLCSetYAxisScaleWidth(QLineChart * qw, double w)
{
    return qw->SetYAxisScaleWidth(w);
}

extern "C"
__declspec(dllexport) void QFLCSetLineWidth(QLineChart * qw, double w)
{
    return qw->SetLineWidth(w);
}


extern "C"
__declspec(dllexport) void QFLCSetXAxisRange(QLineChart * qw, double m, double M)
{
    return qw->SetXAxisRange(m, M);
}

extern "C"
__declspec(dllexport) void QFLCSetYAxisRange(QLineChart * qw, double m,double M)
{
    return qw->SetYAxisRange(m,M);
}


extern "C"
__declspec(dllexport) void QFLCSetLegWidth(QLineChart * qw, double w)
{
    return qw->SetLegWidth(w);
}


extern "C"
__declspec(dllexport) void QFLCSetAttribute(QLineChart * qw, long long v)
{
    return qw->SetAttribute(v);
}


extern "C"
__declspec(dllexport) int QFLCFindCurve(QLineChart * qw, const wchar_t* name, int start = 0)
{
    return qw->FindCurve(name, start);
}

extern "C"
__declspec(dllexport) void QFLCSetCurveColor(QLineChart * qw, int index, unsigned long color)
{
    return qw->SetCurveColor(index, color);
}

extern "C"
__declspec(dllexport) void QFLCSetCurveName(QLineChart * qw, int index, const wchar_t* name)
{
    return qw->SetCurveName(index, name);
}

extern "C"
__declspec(dllexport) void QFLCCurveInsertPoint(QLineChart * qw,int index, const GPointF& pt)
{
    return qw->CurveInsertPoint(index,pt);
}

extern "C"
__declspec(dllexport) int QFLCAddCurve(QLineChart * qw, GPointF * pt, int len, unsigned long color , const wchar_t* name )
{
    return qw->AddCurve(pt, len, color,name);
}

extern "C"
__declspec(dllexport) void QFLCSetFontName(QLineChart * qw, const wchar_t* t)
{
    return qw->SetFontName(t);
}

extern "C"
__declspec(dllexport) void QFLCSetFontSize(QLineChart * qw, INT size)
{
    return qw->SetFontSize(size);
}
extern "C"
__declspec(dllexport) void QFLCSetTextAlgn(QLineChart * qw, int index)
{
    if (index < 0 || 2 < index)
        return qw->SetTextAlgn(Gdiplus::StringAlignment(index));
}
extern "C"
__declspec(dllexport) void QFLCSetTextAllgn(QLineChart * qw, int index)
{
    if (index < 0 || 2 < index)
        return qw->SetTextAllgn(Gdiplus::StringAlignment(index));
}

extern "C"
__declspec(dllexport) void QFLCSetTextAlgnii(QLineChart * qw, int index, int allgn)
{
    if (index < 0 || 2 < index)
        return qw->SetTextAlgn(Gdiplus::StringAlignment(index), Gdiplus::StringAlignment(allgn));
}

extern "C"
__declspec(dllexport) void QFLCSetLegFontName(QLineChart * qw, const wchar_t* t)
{
    return qw->SetLegFontName(t);
}

extern "C"
__declspec(dllexport) void QFLCSetLegFontSize(QLineChart * qw, INT size)
{
    return qw->SetLegFontSize(size);
}


