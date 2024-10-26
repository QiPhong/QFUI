#include "QFReportForm.h"
typedef Gdiplus::Brush GBrush;
typedef Gdiplus::Rect GRect;
typedef Gdiplus::Size GSize;
typedef Gdiplus::Point GPoint;
typedef Gdiplus::PointF GPointF;
typedef Gdiplus::Color GColor;
typedef Gdiplus::SolidBrush GSolidBrush;
typedef Gdiplus::Font GFont;
typedef Gdiplus::RectF GRectF;
typedef Gdiplus::StringFormat GStringFormat;
typedef Gdiplus::Pen GPen;



QReportFormTitleUnit::QReportFormTitleUnit()
{
    color1.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(0XFF, 0xB7, 0xC1, 0xF6)));
    color2.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(0XFF, 0X5C, 0X7C, 0XBF)));

    color3.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(0XFF, 0X40, 0X72, 0XB4)));
    color4.SetValue(Gdiplus::ARGB(Gdiplus::Color::MakeARGB(0XFF, 0X1C, 0X41, 0X8E)));
}
QReportFormTitleUnit::QReportFormTitleUnit(const QReportFormTitleUnit& rft)
{
    text = rft.text;
    color1.SetValue(rft.color1.GetValue());
    color2.SetValue(rft.color2.GetValue());
    color3.SetValue(rft.color3.GetValue());
    color4.SetValue(rft.color4.GetValue());
    width = rft.width;

}
void QReportFormTitleUnit::operator=(const QReportFormTitleUnit& rft)
{
    text = rft.text;
    color1.SetValue(rft.color1.GetValue());
    color2.SetValue(rft.color2.GetValue());
    color3.SetValue(rft.color3.GetValue());
    color4.SetValue(rft.color4.GetValue());
    width = rft.width;

}
bool QReportFormBrushContext::operator<(const QReportFormBrushContext& tc)const
{
    unsigned long long V = A;
    V <<= 24;
    V |= R; V <<= 16;
    V |= G; V <<= 8;
    V |= B;
    V <<= 8;
    V |= A2;
    V <<= 8;
    V |= R2;
    V <<= 8;
    V |= G2;
    V <<= 8;
    V |= B2;

    unsigned long long V2 = tc.A;
    V2 <<= 24;
    V2 |= tc.R; V2 <<= 16;
    V2 |= tc.G; V2 <<= 8;
    V2 |= tc.B;
    V2 <<= 8;
    V2 |= tc.A2;
    V2 <<= 8;
    V2 |= tc.R2;
    V2 <<= 8;
    V2 |= tc.G2;
    V2 <<= 8;
    V2 |= tc.B2;
    if (V < V2)
        return true;
    else if (V == V2) {
        if (F < tc.F)return true;
        else if (F == tc.F) {
            if (W < tc.W)return true;
            else return false;
        }
        else
            return false;
    }
    else
        return false;
}
void QRecorder::SetUnitText(int index, std::wstring t)
{
    if (index >= unit.size())return;
    unit[index].text = t;

}

void QRecorder::SetUnitBackgroundColor(int index, QReportFormBrushContext context, INT height)
{
    if (index >= unit.size())return;
    //处理丢弃画刷
    {
        auto it = m_BToolBox.find(unit[index].bContext);//一个引用在map里面，一个在it。一个在当前值，所以如果应用值小于4则释放
        if (it != m_BToolBox.end()) {
            if (4 > it->second.use_count()) {

                m_BToolBox.erase(it);//释放没人用的画刷
            }

        }
    }


    //处理新的画刷
    unit[index].bContext = context;
    //查找有没有人用过这个画刷
    {
        auto it = m_BToolBox.find(unit[index].bContext);//
        if (it != m_BToolBox.end()) {
            //有人用过就直接引用
            unit[index].pGBrush = it->second;
        }
        else {
            //如果没人用过就创造一个
            std::shared_ptr<GBrush> brush(CreaterGBrush(unit[index].bContext, height));
            unit[index].pGBrush = brush;
            //暂存工具箱里面
            m_BToolBox.insert(std::pair<QReportFormBrushContext, std::shared_ptr<GBrush>>(unit[index].bContext, brush));


        }
    }


}

GBrush* QRecorder::CreaterGBrush(const QReportFormBrushContext& rf, INT height)
{
    /*
    #define QREPORTFORMCONTEXT_SOLIDE                   0x0000//描述使用实心画刷
    #define QREPORTFORMCONTEXT_GRADUAL_V                0x0001//线性垂直
    #define QREPORTFORMCONTEXT_GRADUAL_H                0x0002//线性水平
    #define QREPORTFORMCONTEXT_GRADUAL_LTRB             0x0003//线性左上右下
    #define QREPORTFORMCONTEXT_GRADUAL_RTLB             0x0004//线性右上左下
    */
    GBrush* brush = NULL;
    switch (rf.F) {
    case QREPORTFORMCONTEXT_GRADUAL_V: {

        brush = new Gdiplus::LinearGradientBrush(GPointF(rf.W / 2.0, 0), GPointF(rf.W / 2.0, height)
            , GColor(rf.A, rf.R, rf.G, rf.B), GColor(rf.A2, rf.R2, rf.G2, rf.B2));
        break;
    }
    case QREPORTFORMCONTEXT_GRADUAL_H: {
        brush = new Gdiplus::LinearGradientBrush(GPointF(0, height / 2.0), GPointF(rf.W, height / 2.0)
            , GColor(rf.A, rf.R, rf.G, rf.B), GColor(rf.A2, rf.R2, rf.G2, rf.B2));
        break;
    }
    case QREPORTFORMCONTEXT_GRADUAL_LTRB: {
        brush = new Gdiplus::LinearGradientBrush(GPointF(0, 0), GPointF(rf.W, height)
            , GColor(rf.A, rf.R, rf.G, rf.B), GColor(rf.A2, rf.R2, rf.G2, rf.B2));
        break;
    }
    case QREPORTFORMCONTEXT_GRADUAL_RTLB: {
        brush = new Gdiplus::LinearGradientBrush(GPointF(rf.W, 0), GPointF(0, height)
            , GColor(rf.A, rf.R, rf.G, rf.B), GColor(rf.A2, rf.R2, rf.G2, rf.B2));
        break;
    }
    default: {
        //默认创建一个实心画刷
        brush = new Gdiplus::SolidBrush(GColor(rf.A, rf.R, rf.G, rf.B));
        break;
    }

    }
    return brush;

}

inline int QRecorder::InsertUint(int index)
{
    if (0 > index || index > unit.size())return 0;

    unit.insert(unit.begin() + index, QRecorderUnit());

    return 1;
}


inline int QReportForm::InsertRecorder(const std::wstring& w, int index)
{
    if (-1 == index)index = m_data.size();
    //检查插入位置是否合法
    if (0 > index || index > m_data.size())return 0;

    //初始化
    QRecorder Qr(m_Title.unit.size());
    if (0 < Qr.unit.size()) {
        Qr.unit[0].text = w;
    }
    for (int i = -1; ++i < m_Title.unit.size();) {
        Qr.unit[i].bContext.W = m_Title.unit[i].width;

    }


    //插入
    m_data.insert(m_data.begin() + index, Qr);
    isChange = true;
    return 1;
}
inline int QReportForm::RemoveRecorder(int index)
{
    if (-1 == index)index = QDWORD(m_data.size()) - 1;

    //检查位置是否合法
    if (0 > index || index >= m_data.size())return 0;
    std::unique_lock<std::shared_mutex> lk(m_adlock);
    //记录位置
    // int point = 
    

    m_data.erase(m_data.begin() + index);
    int maxR = m_rect.Height - m_Title.height - height * m_data.size();
    if (RollPointVertical > 0) {
        RollPointVertical = 0;
    }
    else if (RollPointVertical < maxR) {
        if (maxR <= 0) {
            //最后一个的底边不能小于控件底边
            RollPointVertical = maxR;
        }
        else {
            RollPointVertical = 0;
        }


    }

    if (selectItemIndex >= m_data.size()) {
        selectItemIndex = -1;


    }
    isChange = true;

}
inline int QReportForm::SelectItemIndex()
{
    if (0 > selectItemIndex || selectItemIndex >= m_data.size()) {
        selectItemIndex = -1;
        return -1;
    }
    return selectItemIndex;
}
inline int QReportForm::SetRecorderText(int iRd, int iUnit, std::wstring context)
{
    if (0 > iRd || iRd > QDWORD(m_data.size())-1)return 0;
    if (0 > iUnit || iUnit > m_data[iRd].unit.size())return 2;
    m_data[iRd].unit[iUnit].text = context;




    isChange = true;
    return 1;

}
inline int QReportForm::GetRecorderCount()const
{
    return m_data.size();
}
inline int QReportForm::GetTitleCount()const
{
    return m_Title.unit.size();

}
inline int QReportForm::SetRecorderBK(int iRd, int iUnit, const QReportFormBrushContext& context)
{
    if (0 > iRd || iRd > m_data.size())return 0;
    QReportFormBrushContext qcontext = context;
    if (-1 == iUnit) {
        //整行设置这个画刷
        for (int i = -1; ++i < m_data[iRd].unit.size();) {
            qcontext.W = m_Title.unit[i].width;
            m_data[iRd].SetUnitBackgroundColor(i, qcontext, height);

        }

    }
    else if (m_data[iRd].unit.size() > iUnit && -1 < iUnit) {
        //只设置一个指定的单元
        qcontext.W = m_Title.unit[iUnit].width;
        m_data[iRd].SetUnitBackgroundColor(iUnit, qcontext, height);
    }
    else return 2;

    isChange = true;
    return 1;
}







int QReportForm::SetTitleHeight(int h)
{
    if (1 > h)return m_Title.height;
    int height = m_Title.height;
    m_Title.height = h;

    return height;

}

const wchar_t* QReportForm::GetTitleText(int index)
{
    if (index < 0 || index >= QDWORD(m_Title.unit.size()))return nullptr;



    return m_Title.unit[index].text.c_str();
}

int QReportForm::GetTitleWidth(int index)
{
    if (index < 0 || index >= QDWORD(m_Title.unit.size()))return -1;
    return m_Title.unit[index].width;
}

int QReportForm::RemovTitle(int index)
{
    std::unique_lock<std::shared_mutex> lk(m_adlock);
    if (-1 == index)index = QDWORD(m_Title.unit.size()) - 1;
    if (index < 0 || index > QDWORD(m_Title.unit.size()) - 1)return -1;
    m_Title.unit.erase(m_Title.unit.begin()+index);
    for (auto& it : m_data) {
        it.unit.erase(it.unit.begin() + index);
    
    }


    return 1;
}

int QReportForm::SetTitle(const std::wstring& w, INT wdth, int index)
{
    std::unique_lock<std::shared_mutex> lk(m_adlock);
    if (index < 0 || index > QDWORD(m_Title.unit.size()) - 1)return -1;
    m_Title.unit[index].text = w;
    m_Title.unit[index].width = wdth;
    return 1;
}

const wchar_t* QReportForm::GetCellText(int iRd, int iUnit)
{
    std::unique_lock<std::shared_mutex> lk(m_adlock);
    if (0 > iRd || iRd > QDWORD(m_data.size())-1)return nullptr;
    if (0 > iUnit || iUnit > QDWORD(m_Title.unit.size()) - 1)return nullptr;

    return m_data[iRd].unit[iUnit].text.c_str();
}

void QReportForm::InsertTitle(const std::wstring& w, INT wdth, int index)
{
    if (1 > wdth)return;
    if (-1 == index)index = m_Title.unit.size();
    //检查插入位置是否合法
    if (0 > index || index > m_Title.unit.size())return;

    QReportFormTitleUnit rft;
    rft.width = wdth;
    rft.text = w;

    std::unique_lock<std::shared_mutex> lk(m_adlock);
    //插入
    QRecorderUnit rd;
    m_Title.unit.insert(m_Title.unit.begin() + index, rft);

    for (auto& it : m_data) {//每个记录都插入一列
        if (m_Title.unit.size() > it.unit.size()) {
            while (index > it.unit.size()) {
                it.unit.push_back(rd);
            }
            if (index <= it.unit.size()) {
                it.unit.insert(it.unit.begin() + index, rd);

            }
            // it.InsertUint(index);

        }
    }




}

QReportForm::QReportForm(HDC winddc, const GRect& rf) :QCtrlBase(winddc, rf)
{
    m_bkBrush = new GSolidBrush(GColor(0x50, 0xE0, 0xE0, 0xE0));
    SItemColor.SetValue(Gdiplus::Color::MakeARGB(0x70, 0x4E, 0X6E, 0XF2));

    // m_tBrush = new GSolidBrush(GColor(255,0,0,0));
}
QReportForm::~QReportForm()
{
    if (!m_bkBrush) {
        delete m_bkBrush;

    }


}
int QReportForm::Draw(HDC sdc)
{
    bool flag = 0;
    //添加绘制代码
    
    if (isChange) {
        bool flag = 1;
        isChange = false;
        //1.sdc中对应的位置拷贝过来
        BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);

        //2.更新控件内容
        Gdiplus::Graphics graphics(m_bdc);

        graphics.FillRectangle(m_bkBrush, 0, 0, m_rect.Width, m_rect.Height);
        graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
        std::shared_lock<std::shared_mutex> lk(m_adlock);

        {//绘制表格内容
            int h = height;
            int y = RollPointVertical + m_Title.height;
            for (auto vit : m_data) {//绘制对象集合

                int left = RollPointHorizontal;
                if (y < 0) {
                    y += h;
                    continue;
                }

                for (int i = -1; ++i < m_Title.unit.size();) {//获取水平方向的位置

                    int w = m_Title.unit[i].width;
                    GRectF tRect(left, y, w, h);
                    //查看有没有画刷，有就绘制特殊背景
                    if (vit.unit[i].pGBrush) {
                        //绘制独特背景
                        graphics.FillRectangle(vit.unit[i].pGBrush.get(), tRect);

                    }
                    //绘制表格文字

                    GStringFormat gsf(0, 0);
                    gsf.SetAlignment(sAlign);//水平居中
                    gsf.SetLineAlignment(slAlign);//垂直居中
                    gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                    
                    GFont drawFont(m_fontName.c_str(), m_fontSize);
                    Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                    std::wstring& str = vit.unit[i].text;

                    graphics.DrawString(str.c_str(), (INT)str.length(), &drawFont, tRect, &gsf, &gsb);
                    left += w;
                    if (left > m_rect.Width)break;

                }
                y += h;
                if (m_rect.Height < y)break;
            }






        }

        { //绘制选中高亮
            //当前选中项的位置
            if (-1 < selectItemIndex) {
                int pos = selectItemIndex * height + RollPointVertical;
                if (0 <= pos + height && m_rect.Height >= pos) {//只要有部分显示在屏幕的就绘制
                    GRectF rtf(0.0, pos + m_Title.height, m_rect.Width, height);
                    Gdiplus::SolidBrush brush(SItemColor);
                    graphics.FillRectangle(&brush, rtf);

                }
            }

        }



        {//绘制表头
            //拿到表头的画刷
            int h = m_Title.height;
            int left = RollPointHorizontal;
            for (int i = -1; ++i < m_Title.unit.size();) {
                auto& it = m_Title.unit[i];
                // float compensate = 0.0;
                // GRectF rectf1(left,0,it.width,h/2.0+compensate);

                // GRectF rectf2(left,h/2.0-compensate,it.width,h/2.0+compensate);
                // Gdiplus::LinearGradientBrush bh1(GPointF(rectf1.Width/2.0,0),GPointF(rectf1.Width/2.0,rectf1.Height)
                // ,it.color1,it.color2);
                // graphics.FillRectangle(&bh1, rectf1);
                // Gdiplus::LinearGradientBrush bh2(GPointF(rectf2.X+rectf2.Width/2.0,rectf2.Y),GPointF(rectf2.X+rectf2.Width/2.0,rectf2.Y+rectf2.Height)
                // ,it.color3,it.color4);

                // graphics.FillRectangle(&bh2, rectf2);


                GRectF rectf1(left, 0, it.width, h);
                Gdiplus::LinearGradientBrush bh1(GPointF(rectf1.Width / 2.0, 0), GPointF(rectf1.Width / 2.0, rectf1.Height)
                    , it.color1, it.color2);
                GColor ca[4];
                ca[0] = it.color1;
                ca[1] = it.color2;
                ca[2] = it.color3;
                ca[3] = it.color4;
                Gdiplus::REAL pos[] = { 0.0f,0.45f,0.55f,1.0f };
                bh1.SetInterpolationColors(ca, pos, 4);
                graphics.FillRectangle(&bh1, rectf1);
                //如果是最后一个则绘制到结尾
                if (i + 1 == m_Title.unit.size()) {
                    GRectF rectf1(left + it.width, 0, m_rect.Width - left - it.width, h);
                    graphics.FillRectangle(&bh1, rectf1);
                }





                //绘制表头文字
                GRectF tRect(left, 0, it.width, h);
                GStringFormat gsf(0, 0);
                gsf.SetAlignment(sAlign);//水平居中
                gsf.SetLineAlignment(slAlign);//垂直居中
                gsf.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap);
                GFont drawFont(m_fontName.c_str(), m_fontSize);
                Gdiplus::SolidBrush gsb(GColor(0xff, 0, 0, 0));
                graphics.DrawString(it.text.c_str(), (INT)it.text.length(), &drawFont, tRect, &gsf, &gsb);

                left += it.width;
                // //绘制纵向表格线
                // GPen pen(GColor(0xff,0,0,0));
                // graphics.DrawLine(&pen,GPointF(left,0),GPointF(left,m_rect.Height));
            }




        }

        {
            //绘制表格线
            //1.纵向线
            int h = m_Title.height;
            int left = RollPointHorizontal;
            GPen pen(GColor(0xff, 0, 0, 0));
            for (auto it : m_Title.unit) {
                left += it.width;
                if (0 <= left) {

                    graphics.DrawLine(&pen, GPointF(left, 0), GPointF(left, m_rect.Height));
                }

            }
            //2.水平线
            int startPos = RollPointVertical + m_Title.height;//开始位置
            int top = startPos;

            while (top < m_rect.Height) {
                if (top >= m_Title.height) {
                    graphics.DrawLine(&pen, GPointF(0, top), GPointF(m_rect.Width, top));

                }



                top += height;
            }



        }







        BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);

    }


    //****************************************
    QCtrlBase::Draw(sdc);
    return flag;

}

//属性设置


void QReportForm::SetFontName(const wchar_t* name)
{
    m_fontName = name;

    isChange = true;
}
void QReportForm::SetFontSize(INT size)
{
    m_fontSize = size;

    isChange = true;
}
void QReportForm::SetTextAlgn(const Gdiplus::StringAlignment& align)
{
    sAlign = align;

    isChange = true;
}
void QReportForm::SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign)
{
    sAlign = align;
    slAlign = lalign;

    isChange = true;
}
void QReportForm::SetTextAllgn(const Gdiplus::StringAlignment& lalign)
{
    slAlign = lalign;

    isChange = true;
}

void QReportForm::SetBKBrush(GBrush* pgb)
{
    if (m_bkBrush) {
        delete m_bkBrush;
    }
    m_bkBrush = pgb->Clone();
    isChange = true;
}
const GBrush* QReportForm::GetBKBrush()const
{
    // GBrush* ret =m_bkBrush;
    // m_bkBrush = pgb->Clone();
    return m_bkBrush;
}
int QReportForm::LButtonDown(int x, int y, WPARAM wParam)
{
    int ret = 0;
    if ((y - m_rect.Y) > m_Title.height) {
        m_LbtIsDown = true;
        // m_LDownPosY=y-m_rect.Y-RollPointVertical;
        m_LDownPosY = m_rect.Y + RollPointVertical - y;
        ret = 1;//要求捕获鼠标

    }
    {

        //计算被选中的记录
        int Y = y - m_rect.Y - m_Title.height;//偏移下Y的坐标
        if (-m_Title.height <= Y && Y < 0) {
            //点击表头处理
            selectItemIndex = -1;
        }
        else if (Y >= 0) {
            int pos = (Y - RollPointVertical) / height;
            selectItemIndex = pos;
        }


    }
    return ret;
}
int QReportForm::LButtonDBLClick(int x, int y, WPARAM wParam)
{

    return 0;
}
int QReportForm::LButtonUp(int x, int y, WPARAM wParam)
{
    m_LbtIsDown = false;
    // m_LDownPosY=y-m_rect.Y-RollPointVertical;
    // RollPointVertical=m_LDownPosY+m_rect.Y-y;
    // m_LDownPosY=m_rect.Y+RollPointVertical-y;

    isChange = true;
    return 0;
}

int QReportForm::RButtonDown(int x, int y, WPARAM wParam)
{

    return 0;
}
int QReportForm::RButtonUp(int x, int y, WPARAM wParam)
{

    return 0;
}
int QReportForm::RButtonDBLClick(int x, int y, WPARAM wParam)
{

    return 0;
}

int QReportForm::MButtonDown(int x, int y, WPARAM wParam)
{

    return 0;
}

int QReportForm::MButtonUp(int x, int y, WPARAM wParam)
{

    return 0;
}

int QReportForm::MButtonDBLClick(int x, int y, WPARAM wParam)
{

    return 0;
}


int QReportForm::MouseMove(int x, int y, WPARAM wParam)
{
    //char string[1024] = { 0 };
    //sprintf_s(string, 1024, "x=%d,y=%d",x, y);
    //MessageBoxA(0, string, "", 0);
    if (m_LbtIsDown && m_isDrag) {
        RollPointVertical = m_LDownPosY + y - m_rect.Y;
 /*       char string[1024] = { 0 };
        sprintf_s(string, 1024, "x=%d,y=%d,m_LDownPosY=%d,RollPointVertical=%d", x, y, m_LDownPosY ,RollPointVertical);
        MessageBoxA(0, string, "", 0);*/
        int maxR = m_rect.Height - m_Title.height - height * m_data.size();
        if (RollPointVertical >= 0) {
            RollPointVertical = 0;
            m_LDownPosY = m_rect.Y  - y;//拉不动了，重新计算按下位置以便于能迅速反方向拉

        }
        else if (RollPointVertical < maxR) {

            if (maxR <= 0) {
                //最后一个的底边不能小于控件底边
                RollPointVertical = maxR;
            }
            else {
                RollPointVertical = 0;
            }
            m_LDownPosY = m_rect.Y + RollPointVertical - y;//拉不动了，重新计算按下位置以便于能迅速反方向拉
        }

        isChange = true;
    }


    return 0;
}

int QReportForm::MouseWheel(int x, int y, WPARAM wParam)
{

    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    RollPointVertical += zDelta;
    int maxR = m_rect.Height - m_Title.height - height * m_data.size();

    if (RollPointVertical > 0) {
        RollPointVertical = 0;
    }
    else if (RollPointVertical < maxR) {
        if (maxR <= 0) {
            //最后一个的底边不能小于控件底边
            RollPointVertical = maxR;
        }
        else {
            RollPointVertical = 0;
        }


    }
    isChange = true;
    return 0;
}








//导出函数

extern "C" __declspec(dllexport) void QWRFIsDrag(QReportForm * qrf, bool isDrag)//是否可以鼠标按住上下拉动
{
    qrf->m_isDrag = isDrag;
}

extern "C" __declspec(dllexport) void QWRFInsertRecorder(QReportForm * qrf, const wchar_t* w, int index)//给列表插入一条记录
{
    qrf->InsertRecorder(std::wstring(w), index);
}

extern "C" __declspec(dllexport) void QWRFRemoveRecorder(QReportForm * qrf, int index)//给列表移除一条记录
{
    qrf->RemoveRecorder(index);
}

extern "C" __declspec(dllexport) int QWRFSelectItemIndex(QReportForm * qrf)//列表当前选择的记录索引
{
    return qrf->SelectItemIndex();
}



extern "C" __declspec(dllexport) int QWRFSetRecorderText(QReportForm * qrf, int iRd, int iUnit, const wchar_t* context)//设置项目文本
{
    return qrf->SetRecorderText(iRd, iUnit, std::wstring(context));
}

extern "C" __declspec(dllexport) int QWRFGetRecorderCount(QReportForm * qrf)//取记录数
{
    return qrf->GetRecorderCount();
}

extern "C" __declspec(dllexport) int QWRFGetTitleCount(QReportForm * qrf)//取字段数
{
    return qrf->GetTitleCount();
}

__declspec(dllexport) int QWRFSetRecorderBK(QReportForm* qrf, int iRd, int iUnit, unsigned char A, unsigned char R, unsigned char G, unsigned char B)//设置项目背景颜色
{
    QReportFormBrushContext context;
    context.A = A;
    context.R = R;
    context.G = G;
    context.B = B;

    return qrf->SetRecorderBK(iRd, iUnit, context);
}

__declspec(dllexport) int QWRFSetRecorderBK(QReportForm* qrf, int iRd, int iUnit, unsigned char A, unsigned char R, unsigned char G, unsigned char B,
    unsigned char A2, unsigned char R2, unsigned char G2, unsigned char B2, int w, int f)//设置项目背景颜色
{
    QReportFormBrushContext context;
    context.A = A;
    context.R = R;
    context.G = G;
    context.B = B;
    context.A2 = A2;
    context.R2 = R2;
    context.G2 = G2;
    context.B2 = B2;
    context.W = w;
    context.F = f;
    return qrf->SetRecorderBK(iRd, iUnit, context);
}


extern "C" __declspec(dllexport) void QWRFSetFontName(QReportForm * qrf, const wchar_t* name)//文本的字体名称
{

    qrf->SetFontName(name);
}
extern "C" __declspec(dllexport) void QWRFSetFontSize(QReportForm * qrf, INT size)//文本的字体大小
{

    qrf->SetFontSize(size);
}
__declspec(dllexport) void QWRFSetTextAlgn(QReportForm* qrf, int align)//文本的字体对齐
{

    qrf->SetTextAlgn(Gdiplus::StringAlignment(align));
}
__declspec(dllexport) void QWRFSetTextAlgn(QReportForm* qrf, int align, int lalign)//文本的字体对齐
{

    qrf->SetTextAlgn(Gdiplus::StringAlignment(align), Gdiplus::StringAlignment(lalign));
}

__declspec(dllexport) void QWRFSetTextVAlgn(QReportForm* qrf, int lalign)//文本的字体对齐
{

    qrf->SetTextAlgn(Gdiplus::StringAlignment(lalign));
}

__declspec(dllexport) void QWRFSetBKBrush(QReportForm* qrf, GBrush* pgb)//设置背景画刷
{

    qrf->SetBKBrush(pgb);
}

__declspec(dllexport) GBrush* QWRFGetBKBrush(QReportForm* qrf)//取背景画刷
{

    return const_cast<GBrush*>(qrf->GetBKBrush());
}

__declspec(dllexport) void QWRFInsertTitle(QReportForm* qrf, const wchar_t* w, int wdth, int index)//插入字段
{

    qrf->InsertTitle(w, wdth, index);
}

__declspec(dllexport) int QWRFSetTitleHeight(QReportForm* qrf, int h)//设置字段高度
{

    return qrf->SetTitleHeight(h);
}

extern "C" {
    
    __declspec(dllexport) const wchar_t* QWRFGetTitleText(QReportForm* qrf, int index)
    {
    
        return qrf->GetTitleText(index);
    }
    __declspec(dllexport)int QWRFGetTitleWidth(QReportForm* qrf, int index)
    {
    
        return qrf->GetTitleWidth(index);
    }
    __declspec(dllexport)int QWRFRemovTitle(QReportForm* qrf, int index)
    {

        return qrf->RemovTitle(index);
    }
    __declspec(dllexport)int QWRFSetTitle(QReportForm* qrf, const wchar_t* w, INT wdth, int index)
    {

        return qrf->SetTitle(w, wdth, index);
    }

    __declspec(dllexport) const wchar_t* QWRFGetCellText(QReportForm* qrf, int iRd, int iUnit)
    {

        return qrf->GetCellText(iRd, iUnit);
    }

}


#include "pch.h"