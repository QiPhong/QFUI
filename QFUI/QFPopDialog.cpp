#include "QFPopDialog.h"
HCURSOR QPopDialog::m_hCursorBuff = nullptr;
QPopDialog::QPopDialog(HDC winddc, const GRect& rf) :QPopDialogBase(winddc, rf)
{
    if (!m_hCursorBuff) {
        m_hCursorBuff = LoadCursor(NULL, IDC_ARROW);
    }
}

QPopDialog::~QPopDialog()
{
}

int QPopDialog::Draw(HDC sdc)
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
        {
            ID2D1SolidColorBrush* psl = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xb0b0b0, 0.7), &psl);
            D2D1_RECT_F rect(0, m_titleHeight, m_rect.Width, m_rect.Height);

            pRenderTarget->FillRectangle(rect, psl);
            psl->Release();
        }
        pRenderTarget->EndDraw();
        //�����ӿؼ�
        {
            std::shared_lock<std::shared_mutex>lk(m_shared_lock_ctrl);
            bool isLayout = false;
            GRect rect;

            for (auto& it : m_QWNDs) {
                if (it->Draw(m_dc)) {


                }

                if (rect.IsEmptyArea()) {
                    rect = it->GetRect();
                }
                else {
                    GRect rt = it->GetRect();
                    if (rect.IntersectsWith(rt)) {
                        isLayout = true;

                    }
                    else {

                        rect.Union(rect, rt, GRect(rect));

                    }

                }



            }
        
        
        
        
        }




        pRenderTarget->BeginDraw();
        if(m_titleHeight > 0)
        {
            D2D1_GRADIENT_STOP gradientStops[2];
            gradientStops[0].position = 0.0f;      // ��ʼλ��
            gradientStops[0].color = D2D1::ColorF((m_titleColor1&0xffffff),((m_titleColor1>>24)&0xff)/255.0);

            gradientStops[1].position = 1.0f;      // ����λ��
            gradientStops[1].color = D2D1::ColorF((m_titleColor2 & 0xffffff), ((m_titleColor2 >>24) & 0xff) / 255.0);
            ID2D1GradientStopCollection* gradientStopCollection = nullptr;
            pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                2,  // ����ֹͣ������
                D2D1_GAMMA_2_2,  // Gamma У��
                D2D1_EXTEND_MODE_CLAMP,
                &gradientStopCollection
            );

            ID2D1LinearGradientBrush* linearGradientBrush = nullptr;
            if (0 == m_titleColorDirect) {
                pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),  // �������
                        D2D1::Point2F(m_rect.Width, 200)  // �����յ�
                    ),
                    gradientStopCollection,
                    &linearGradientBrush
                );
            }
            else if (1 == m_titleColorDirect) {
                pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),  // �������
                        D2D1::Point2F(0, m_titleHeight)  // �����յ�
                    ),
                    gradientStopCollection,
                    &linearGradientBrush
                );
            
            }




            D2D1_RECT_F rect(0,0,m_rect.Width, m_titleHeight);
            //D2D1_RECT_F rect(0, 0, m_rect.Width, m_rect.Height);
            pRenderTarget->FillRectangle(rect, linearGradientBrush);

            // �ͷ���Դ
            gradientStopCollection->Release();
            linearGradientBrush->Release();

        }

    


        pRenderTarget->EndDraw();
    }
    QPopDialogBase::Draw(sdc);
	return ret;
}

void QPopDialog::AttachCtrl(QWND* w)
{
    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
    ((QPopDialog*)w)->m_Parent = (QWND*)this;
    ((QPopDialog*)w)->m_HostHwnd = m_HostHwnd;
    QSCWnd::AttachCtrl(w);
}

QDWORD QPopDialog::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    switch (msgID)
    {
    case QM_GET_SUB_CTRL: {
        std::shared_lock<std::shared_mutex> lk(m_shared_lock_ctrl);
        if (LParam) {
            QWND** pwnd = (QWND**)LParam;
            if (RParam > -1 && RParam < QDWORD(m_QWNDs.size())) {
                *pwnd = m_QWNDs[RParam];
            }

        }

        return m_QWNDs.size();
    }
    case QM_UPDATE_CTRL: {
        isChange = true;
        if (LParam & 0x01) {
            if (m_Parent)m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
        if (LParam & 0x02) {
            std::shared_lock<std::shared_mutex> lk(m_shared_lock_ctrl);
       
            for (auto& it : m_QWNDs) {
                it->Redraw();
                it->TransferMessage(msgID, LParam, RParam, AddParam);
            }

            
        }


    }break;
    case  QM_POPDIALOG_MOUSE_OUT: {
        //��겻�ڿؼ���ʱ������ʱ����ϢֵLParam
        //һ�����Ҽ����¾���ʧ�������
        if (WM_RBUTTONDOWN == LParam || WM_LBUTTONDOWN == LParam) {
            return m_Parent->TransferMessage(QM_SET_POPDIALOG, 0, 0, 0);
        }

    }break;
    }

    QCtrlBaseD2DIPD::TransferMessage(msgID, LParam, RParam, AddParam);
}

void QPopDialog::Reconstruct(HDC sdc, const GRect& rect)
{
    QCtrlBaseD2DIPD::Reconstruct(sdc, rect);


    isChange = true;
}

int QPopDialog::MouseMove(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_MOUSEMOVE, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::LButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������
        
    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_LBUTTONDOWN, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}


int QPopDialog::LButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_LBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::LButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_LBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::RButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_RBUTTONDOWN, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::RButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_RBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::RButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_RBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::MButtonDown(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_MBUTTONDOWN, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::MButtonUp(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_MBUTTONUP, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::MButtonDBLClick(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_MBUTTONDBLCLK, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::MouseWheel(int x, int y, WPARAM wParam)
{
    int px = x - m_rect.X;
    int py = y - m_rect.Y;

    int ret = 0;
    if (py < m_titleHeight) {
        //������

    }
    else {
        int pcx = px;
        int pcy = py - m_titleHeight;
        ret = MouseMessage(WM_MOUSEHWHEEL, wParam, pcx, pcy);
    }
    isChange = true;

    return ret;
}

int QPopDialog::MouseMoveOut()
{

    return 0;
}

int QPopDialog::KeyChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_CHAR, wParam, lParam);
    return 0;
}

int QPopDialog::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_IME_CHAR, wParam, lParam);
    return 0;
}

int QPopDialog::KeyDown(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYDOWN, wParam, lParam);
    return 0;
}

int QPopDialog::KeyUp(WPARAM wParam, LPARAM lParam)
{
    KeyBoardMessage(WM_KEYUP, wParam, lParam);
    return 0;
}

void QPopDialog::killFocus()
{
}

int QPopDialog::Inverted(QPOINTER ptr)
{

    return 0;
}

inline int QPopDialog::MouseMessage(UINT uMsg, WPARAM wParam, int x, int y)
{

    int ret = 0;

    POINT pt;
    pt.x = x;
    pt.y = y;

    {
        bool isAtCtrl = false;
        HCURSOR hCursor = m_hCursorBuff;
        std::shared_lock<std::shared_mutex> lk(m_shared_lock_ctrl);

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

    }
    return ret;
}

inline int QPopDialog::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
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

inline void QPopDialog::KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

QPopList::QPopList(HDC winddc, const GRect& rf) :QPopDialogBase(winddc, rf)
{
}

QPopList::~QPopList()
{
}

int QPopList::Draw(HDC sdc)
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
        {
            ID2D1SolidColorBrush* psl = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xb0b0b0, 0.7), &psl);
            D2D1_RECT_F rect(0, 0, m_rect.Width, m_rect.Height);
            // ����Բ�Ǿ��εĲ���
            D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                rect, // ����λ�úʹ�С
               2, 2                              // Բ�ǰ뾶��x, y��
            );
            pRenderTarget->FillRoundedRectangle(roundedRect, psl);
            //pRenderTarget->FillRectangle(rect, psl);
            psl->Release();
        }
        
        //����list��������
        if(m_LK_list && m_list)
        {
            std::shared_mutex* pLK = m_LK_list.load();
            std::shared_lock<std::shared_mutex> lk(*pLK);
            float y = *m_offsetY;
            float x = *m_offsetX;
            int i = -1;
            std::list<PopListObj>* ls = m_list.load();
            ID2D1SolidColorBrush* bksb = nullptr;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0.2),&bksb);
            for (const auto& it: *ls) {
                ++i;
                //�ж������ʾ�Ƿ��ڴ�����
                if ((i * m_itemHeight+y > m_rect.Height) || 0 >((i + 1) * m_itemHeight+y )) {//�ϱߴ��ڴ����±߿�ģ��±�С�ڴ����ϱ߿��
                    //������ʾ�������Ŀ����
                
                
                }
                else {
                    //���ֻ���ȫ������ʾ����Ĵ���
                    D2D1_RECT_F rect(0, i * m_itemHeight+y, m_rect.Width, (i + 1) * m_itemHeight+y);
                    // ���彥��ֹͣ�� (��ɫ��λ��)
                    D2D1_GRADIENT_STOP gradientStops[] = {
                        { 0.0f, D2D1::ColorF(it.color1 & 0xffffff,((it.color1>>24)&0xff)/255.0) },  // ��ʼ�Ļ�ɫ
                        { 1.0f, D2D1::ColorF(it.color2 & 0xffffff,((it.color2 >> 24) & 0xff) / 255.0) }      // �����ĺ�ɫ
                    };

                    // ��������ֹͣ����
                    ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
                    pRenderTarget->CreateGradientStopCollection(
                        gradientStops,
                        ARRAYSIZE(gradientStops),
                        &pGradientStopCollection);
            
                    ID2D1LinearGradientBrush* pLinearGradientBrush = nullptr;

                    D2D1_POINT_2F pt1(0, 0), pt2(rc.right, 0);
                    if (1 == it.colorD) {
                        pt2.x = 0;
                        pt2.y = rect.bottom;
                    }

                    // �������Խ��仭ˢ
                    pRenderTarget->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                            pt1,              // ���
                            pt2 // �յ�
                        ),
                        pGradientStopCollection,
                        &pLinearGradientBrush);


                    // ����Բ�Ǿ��εĲ���
                    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
                        rect, // ����λ�úʹ�С
                        1, 1                              // Բ�ǰ뾶��x, y��
                    );
                    //����item����
                    pRenderTarget->FillRoundedRectangle(roundedRect, pLinearGradientBrush);
                    if (i != m_index) {
                        pRenderTarget->FillRoundedRectangle(roundedRect, bksb);
                    }
                    

                    pLinearGradientBrush->Release();
                    pGradientStopCollection->Release();

                    //�����ַ��������
                    gradientStops[0].color = D2D1::ColorF(it.fcolor1 & 0xffffff, ((it.fcolor1 >> 24) & 0xff) / 255.0);
                    gradientStops[1].color = D2D1::ColorF(it.fcolor2 & 0xffffff, ((it.fcolor2 >> 24) & 0xff) / 255.0);
                    pRenderTarget->CreateGradientStopCollection(
                        gradientStops,
                        ARRAYSIZE(gradientStops),
                        &pGradientStopCollection);


                    if (1 == it.fcolorD) {
                        pt2.x = 0;
                        pt2.y = rect.bottom;
                    }
                    else {
                        pt2.x = rect.right;
                        pt2.y = 0;
                    }
                    pRenderTarget->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                            pt1,              // ���
                            pt2 // �յ�
                        ),
                        pGradientStopCollection,
                        &pLinearGradientBrush);

                    //�����ַ�
                    pRenderTarget->DrawText(
                        it.text.c_str(),
                        it.text.length(),
                        pTextFormat,             // The text format
                        rect,              // The layout rectangle
                        pLinearGradientBrush                   // The brush

                    );




                    pGradientStopCollection->Release();
                    pLinearGradientBrush->Release();
                  
                }

             
                

            
            }
            bksb->Release();
        }



     
        




        pRenderTarget->EndDraw();
    }
    QPopDialogBase::Draw(sdc);
    return ret;
}

QDWORD QPopList::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{

    switch (msgID)
    {
    case  QM_POPDIALOG_MOUSE_OUT: {
        //��겻�ڿؼ���ʱ������ʱ����ϢֵLParam
        //һ�����Ҽ����¾���ʧ�������
        if (WM_RBUTTONDOWN == LParam || WM_LBUTTONDOWN == LParam) {
      /*      bool* open = m_isOpen.load();
            if (open)*open = false;*/
            if (m_Parent)m_Parent->TransferMessage(QM_SET_POPDIALOG, 0, 0, 0);
            //m_attchObj = nullptr;
  
        }

    }break;
    }

    return QCtrlBaseD2DIPD::TransferMessage(msgID, LParam, RParam, AddParam);
}

void QPopList::Reconstruct(HDC sdc, const GRect& rect)
{
    QCtrlBaseD2DIPD::Reconstruct(sdc, rect);


    isChange = true;
}

int QPopList::MouseMove(int x, int y, WPARAM wParam)
{
    std::shared_mutex* pLK = m_LK_list.load();
    std::shared_lock<std::shared_mutex> lk(*pLK);
    int cy = y - m_rect.Y;
    float fy = *m_offsetY.load();
    float dtah = cy - fy;
    m_index=std::ceil(dtah/ m_itemHeight)-1;
    //printf("%d\n", m_index);
    return 0;
}

int QPopList::MouseMoveOut()
{
    m_index = -1;
    return 0;
}

int QPopList::LButtonDown(int x, int y, WPARAM wParam)
{
    std::shared_mutex* pLK = m_LK_list.load();
    std::shared_lock<std::shared_mutex> lk(*pLK);
    int cy = y - m_rect.Y;
    float fy = *m_offsetY.load();
    float dtah = cy - fy;
    m_index = std::ceil(dtah / m_itemHeight) - 1;
    std::list<PopListObj>* ls = m_list.load();
    if (m_index < ls->size()) {
        //��Ŀ��ѡ��
        if (m_selectItem)m_selectItem((QPOINTER)this, m_index);
        bool* open = m_isOpen.load();
        if (open)*open = false;
        m_attchObj = nullptr;
        m_Parent->TransferMessage(QM_SET_POPDIALOG, 0, 0, 0);
    }

    return 0;
}

int QPopList::MouseWheel(int x, int y, WPARAM wParam)
{
    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (zDelta > m_itemHeight) {
        zDelta = m_itemHeight;
    }
    std::list<PopListObj>* ls = m_list.load();
    int maxR = m_rect.Height  - m_itemHeight * ls->size();
    float* fy = m_offsetY.load();
    float by = *fy;
    by += zDelta;
    if (by > 0) {
        by = 0;
    }
    else if (by < maxR) {
        if (maxR <= 0) {
            //���һ���ĵױ߲���С�ڿؼ��ױ�
            by = maxR;
        }
        else {
            by = 0;
        }


    }
    *fy = by;

    isChange = true;


    return 0;
}

int QPopList::Inverted(QPOINTER ptr)
{
   
    bool* open = m_isOpen.load();
    if (open)*open = false;

    m_attchObj = nullptr;
    return 1;
}

int QPopDialogBase::Inverted(QPOINTER ptr)
{
    return 1;
}

QPopDialogBase::QPopDialogBase(HDC winddc, const GRect& rf):QCtrlBaseD2DIPD(winddc, rf)
{
}
QWND* QPopDialogBase::m_attchObj = nullptr;