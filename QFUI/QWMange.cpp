#include "QWMange.h"
#include "QFButton.h"
//***********************************.CPP file************************************************

QWMange::QWMange(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
    __WNDAttribute.Attribute.isContainerCtrl = 1;//�����ؼ�
    QCreateDC(winddc);
    //imageAttr = new Gdiplus::ImageAttributes();
    
    //����һ����Ϣ������
    isRunThread = true;
    std::thread t(&QWMange::msgThreadFun, this);
    m_msg_thread = std::move(t);
    //init
    LoadMsgModule();
    //QPopDialog* gppd = nullptr;
    //gppd = new QPopDialog(winddc,GRect(800,10,300,300));
    //QWMange* p = (QWMange*)gppd;
    //p->m_Parent = this;
    //QButton* pbt = new QButton(winddc, GRect(10,10,100,50));
    //gppd->AttachCtrl(pbt);
    //m_popDialog.store(gppd);


#ifdef __TEST_CODE_QWM
    DrawBK();
    //����һ�ѱ�����ˢ
    m_bkBrush = new Gdiplus::LinearGradientBrush(GPointF(0.0, 0.0), GPointF(m_rect.Width, m_rect.Height), GColor(255, 0xCC, 0xFB, 0xFF), GColor(255, 0xEF, 0X96, 0Xc5));
#endif

 /*   m_bkBrush = new Gdiplus::LinearGradientBrush(GPointF(0.0, 0.0), GPointF(m_rect.Width, m_rect.Height), GColor(255, 0xCC, 0xFB, 0xFF), GColor(255, 0xEF, 0X96, 0Xc5));
    UpdateBK(m_bkBrush);*/

}

void QWMange::AttachCtrl(QWND* w)
{

    QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

    w->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
  
    //std::unique_lock<std::mutex> lock(m_clock);
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    ((QWMange*)w)->m_Parent = (QWND*)this;
    ((QWMange*)w)->m_HostHwnd = m_HostHwnd;

    //printf("AttachCtrl 0x%x\n", w);
    m_QWNDs.push_back(w);
}
void QWMange::RemoveCtrl(QWND* w)
{
    //std::unique_lock<std::mutex> lock(m_clock);
   
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    auto it = std::find(m_QWNDs.begin(), m_QWNDs.end(), w);
    if (it != m_QWNDs.end()) {
        m_QWNDs.erase(it);
       
    }

}
inline void QWMange::QCreateDC(HDC winddc)
{
    
    //��������DC
    m_bdc = CreateCompatibleDC(winddc);
    // printf("%x,%x",m_bdc,winddc);
    m_bhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bdc, m_bhB);
    //��������BKDC
    m_bkdc = CreateCompatibleDC(winddc);
    // printf("%x,%x",m_bdc,winddc);
    m_bkhB = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(m_bkdc, m_bkhB);

}
inline void QWMange::ReleaseDC()
{
    
    //�ͷŻ���dc
    DeleteObject(m_bhB);
    m_bhB = NULL;
    DeleteDC(m_bdc);
    m_bdc = NULL;

    //�ͷű���dc
    DeleteObject(m_bkhB);
    m_bkhB = NULL;
    DeleteDC(m_bkdc);
    m_bkdc = NULL;

}
QWMange::~QWMange()
{
 
    //����msg�����߳�
    isRunThread = false;//ֹͣ�̱߳�־
    m_cv_queue.notify_all();//֪ͨ�߳�
    if (m_bkBrush) {
        delete m_bkBrush;
        m_bkBrush = NULL;

    }
    if (m_pImage) {
        delete m_pImage;
        m_pImage = NULL;

    }

    std::unique_lock<std::mutex> lock(m_lock_wcsz);

    std::unique_lock<std::mutex> lock_msgThread_Exit(m_clock_msgThread_Exit);
    
    m_msg_thread.detach();//�ȴ��߳�
    UnLoadMsgModule();
    ReleaseDC();
    if (imageAttr) {

        delete imageAttr;
        imageAttr = NULL;
    }

    
 
}

void QWMange::UpdateBK(const GBrush* brush)
{
    if (m_bkdc) {
        Gdiplus::Graphics graphics(m_bkdc);

        std::unique_lock<std::recursive_mutex> lk(m_BKdraw_lock);
        
        //�ȸ���һ��ԭ���İ�ɫ
        Gdiplus::SolidBrush sb(GColor(0xff, 0xff, 0xff));
        graphics.FillRectangle(&sb, 0, 0, m_rect.Width, m_rect.Height);
       
        //���Ʊ���ͼƬ
        if (m_pImage) {
            graphics.DrawImage(m_pImage, GRect(0, 0, m_rect.Width, m_rect.Height), 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight(), Gdiplus::UnitPixel, imageAttr);
            if (imageAttr) {

                delete imageAttr;
                imageAttr = NULL;
            }
        }


        //��ˢ�ǿջ���
        if (!brush)brush = m_bkBrush;
        if (brush)
            graphics.FillRectangle(brush, 0, 0, m_rect.Width, m_rect.Height);

    }


}

int QWMange::Draw(HDC sdc)
{

   
    //��ӻ��ƴ���
    //isChange = false;
    //1����bkdc��������bdc
    std::shared_lock<std::shared_mutex> LK(m_drawLK);
    //{
    //    Gdiplus::Graphics graphics(m_bkdc);
    //    Gdiplus::SolidBrush sb(GColor(0xff, 0xff, 0xff));
    //    graphics.FillRectangle(&sb, 0, 0, m_rect.Width, m_rect.Height);
    //}
    if (m_BKdraw_lock.try_lock()) {
        long long dta=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - __createrTime).count();
        if (dta < 3000) {
            UpdateBK(NULL);
        }

        
        m_BKdraw_lock.unlock();
    }

    BitBlt(m_bdc, 0, 0, m_rect.Width, m_rect.Height, m_bkdc, 0, 0, SRCCOPY);

    //2.�ѿؼ�m_QWNDs�Ļ��Ƶ�����DC m_bdc��
    {
        //std::unique_lock<std::mutex> lock(m_clock);
        // for(auto it = m_QWNDs.rbegin();it != m_QWNDs.rend();++it){
        //     if((*it)->Draw(m_bdc)){
        //         //�пؼ�ˢ��


        //     }
        // }
        //
        // GRect rect;
        Gdiplus::Graphics graphics(m_bdc);

        Gdiplus::Region region;
        {
            std::shared_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
            for (auto& it : m_QWNDs) {
                if (!m_sizeChaneg) {

                    if (it->Draw(m_bdc)) {

                        //�пؼ�ˢ��
                        // GRect::Union(rect,rect,it->GetRect());
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
            //std::shared_lock<std::shared_mutex> ilk(m_invertPopDLK);
            if (m_popDialog) {

                QPopDialogBase* ppd = m_popDialog.load();
                if (ppd) {
                    GRect rect = ppd->GetRect();
                    GPoint pt[4] = { GPoint(rect.X,rect.Y),GPoint(rect.X + rect.Width,rect.Y)
                    ,GPoint(rect.X + rect.Width,rect.Y + rect.Height),GPoint(rect.X,rect.Y + rect.Height) };
                    for (int i = -1; ++i < 4;) {
                        if (region.IsVisible(pt[i], &graphics)) {
                            ppd->Refresh();
                        }
                    }
                    ppd->Draw(m_bdc);
                }
            }


        }
    }

    //3.�ѻ���DC���Ƶ�m_dc��
   
    BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, m_bdc, 0, 0, SRCCOPY);
    //Gdiplus::Graphics graphics1(m_dc);
    //GSolidBrush gs(GColor(0xff,0xff, 0xff, 0xff));
    //graphics1.FillRectangle(&gs,0,0,100,100);
    // printf("�ѻ���DC���Ƶ�m_dc��");
    
    //****************************************
    QWND::Draw(sdc);

    return 1;
}
void QWMange::Reconstruct(HDC sdc, const GRect& rect)
{
    m_rect.Width = rect.Width;
    m_rect.Height = rect.Height;
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    //QCreateDC(sdc);
    Reconstruct(sdc);

}
void QWMange::Reconstruct(HDC sdc)
{
    std::unique_lock<std::shared_mutex> LK(m_drawLK);
    QWND::Reconstruct(sdc);
    //����ع��������Ĵ���
    ReconstructDC(m_bdc, m_dc, m_bhB, m_rect);
    ReconstructDC(m_bkdc, m_dc, m_bkhB, m_rect);
    isChange = true;


}
void QWMange::SetLocation(const GPoint& point)
{
    QWND::SetLocation(point);
    //������ñ��ؼ�λ�õĴ���




}
void QWMange::SetSize(const GSize& size, HDC dc)
{
    QWND::SetSize(size, dc);
    //������ñ��ؼ���С�Ĵ���




}


int QWMange::MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   
    QFMSG msg(HWND(1), uMsg, wParam, lParam);
    //���ù��
    // switch(uMsg){
    //     case WM_SETCURSOR:{
    //         auto hCursor = LoadCursor(NULL, IDC_IBEAM);
    //         ::SetCursor(hCursor);
    //         return 1;
    //     }


    // }
    // if(m_hCursor)
    //     ::SetCursor(m_hCursor);      
    pushQueue(msg);

    {
        switch (uMsg) {
            //�����Ϣ������Ҫ�����޸�
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_MOUSEMOVE:
            // MouseMessage(uMsg,wParam,lParam);
            // if(m_hCursor){
            //     ::SetCursor(m_hCursor); 
            //     return 2;//�й����������
            // }  
            return 2;//�й����������  
            break;
            //������Ϣ����
            // case WM_KEYDOWN:
            // case WM_IME_CHAR:{

            //     break;
            // }
            // case WM_SIZE:{//���ڴ�С�����ı䴦��
            // if(m_HostHwnd){
            //     // RECT rect;
            //     // GetWindowRect(m_HostHwnd,&rect);
            //     // m_rect.X=0;
            //     // m_rect.Y=0;
            //     // m_rect.Width = rect.right-rect.left;
            //     // m_rect.Height = rect.bottom-rect.top;
            //     // Reconstruct(m_dc,m_rect);
            //     // #ifdef __TEST_CODE_QWM
            //     // DrawBK();
            //     // #endif
            // }
            // break;
        }





    }

    return 0;
}

void QWMange::MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam)
{
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
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
        int ret = qwnd->LButtonDown(pt.x, pt.y, wParam);
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
        int ret = qwnd->RButtonDown(pt.x, pt.y, wParam);
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



}


void QWMange::MouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    if (WM_MOUSEWHEEL == uMsg) {
        //RECT rect = {0};
        //GetClientRect(m_HostHwnd,&rect);
        //pt.x -= rect.left;
        //pt.y-= rect.top;
        ScreenToClient(m_HostHwnd, &pt);
    
    }
    bool isAtCtrl = false;
    HCURSOR hCursor = m_hCursor;
    //�������Ի������Ϣ
    bool isInPPD = false;
    {
        //std::shared_lock<std::shared_mutex> ilk(m_invertPopDLK);
        if (m_popDialog) {
            
            QPopDialogBase* p=m_popDialog.load();
            if (p->Contains(pt.x, pt.y)) {
                isInPPD = true;
                MoseMsg(p, pt, uMsg, wParam);
                if (mouseCapture) {
                    mouseCapture->MouseMoveOut();
                    mouseCapture = nullptr;
                }
                if  (mouseFocusCtrl) {
                    mouseFocusCtrl->MouseMoveOut();
                    mouseFocusCtrl = nullptr;
                }
            
            }
            else {
                QWND* pwnd = nullptr;
                std::shared_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
                for (auto pit = m_QWNDs.rbegin(); pit != m_QWNDs.rend(); ++pit) {
                    auto& it = *pit;
                    if (it->Contains(pt.x, pt.y)) {

                        pwnd = (QWND*)it->TransferMessage(QM_POINT_IN_CTRL, pt.x-m_rect.X, pt.y-m_rect.Y,0);
                    }
                
                }
                p->MouseMoveOut();
                //printf("%p ,%p\n", pwnd, p->m_attchObj);
                if (pwnd != p->m_attchObj && (WM_RBUTTONDOWN == uMsg || WM_LBUTTONDOWN == uMsg)) {
                    p->Inverted(0);
                    p->TransferMessage(QM_POPDIALOG_MOUSE_OUT, uMsg, 0, 0);
                }
            
            }
        
        
        }
    
    
    }


    if(!isInPPD)
    {
        //std::unique_lock<std::mutex> lock(m_clock);

        {
    /*        if(uMsg == WM_MOUSEWHEEL && !mouseFocusCtrl)
                printf("WM_MOUSEWHEEL: mouseFocusCtrl=%x,msg=%x\n", mouseFocusCtrl, uMsg);*/
            //printf("WM_MOUSEWHEEL: mouseFocusCtrl=%x,msg=%x,(x=%d,y=%d)\n", mouseFocusCtrl, uMsg, pt.x, pt.y);
            std::shared_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
            for (auto pit = m_QWNDs.rbegin(); pit != m_QWNDs.rend(); ++pit) {
                auto& it = *pit;
                if (mouseCapture && it == mouseCapture) {
                    MoseMsg(mouseCapture, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ

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
                            MoseMsg(mouseFocusCtrl, pt, uMsg, wParam);//�ȸ��������Ŀؼ�����һ����ǰ�����Ϣ
                        }


                        if (it->Contains(pt.x, pt.y)) {
                            isAtCtrl = true;
                            hCursor = it->GetCursor();
                            break;
                        }
                        else mouseFocusCtrl = NULL;
                    }
                    else {
                        /*POINT pt1;
                        pt1.x = mouseFocusCtrl->GetRect().X - 1;
                        pt1.y = mouseFocusCtrl->GetRect().Y - 1;
                        MoseMsg(mouseFocusCtrl, pt1, uMsg, wParam);*/



                        if (it->Contains(pt.x, pt.y) && it->m_isVisual) {
                            printf("it != mouseFocusCtrl: mouseFocusCtrl=%x,it=%x,msg=%x\n", mouseFocusCtrl, it, uMsg);
                            mouseFocusCtrl->MouseMoveOut();
                            MoseMsg(it, pt, uMsg, wParam);
                            mouseFocusCtrl = it;
                            isAtCtrl = true;
                            hCursor = it->GetCursor();
                            break;
                        }
                    }
                }
                else if (it->Contains(pt.x, pt.y) && it->m_isVisual && it->m_isEnable) {
                    if (mouseCapture) {
                        mouseCapture->MouseMoveOut();
                    }
                    printf("mouseFocusCtrl = it;: mouseFocusCtrl=%x,it=%x;mouseCapture=%x\n", mouseFocusCtrl, it, mouseCapture);
                    mouseFocusCtrl = it;
                    
                    MoseMsg(it, pt, uMsg, wParam);
                    isAtCtrl = true;
                    hCursor = it->GetCursor();
                    break;

                }

            }
        }
        //���ö�Ӧ�ؼ������
        SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
        ::SetCursor(hCursor);
        if (!isAtCtrl) {
            //����˲��ǿؼ���ԭ����ؼ�ʧȥ����
            //֪ͨԭ����ؼ�ʧȥ����

            if ((WM_RBUTTONDOWN == uMsg || WM_LBUTTONDOWN == uMsg) ) {
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


}

inline void QWMange::KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void QWMange::pushQueue(const QFMSG& msg)
{
    {
        std::unique_lock<std::mutex> lock(m_clock_queue,std::defer_lock);
        if (m_msg_queue.size() < 5)lock.lock();
        if (m_msg_queue.size() >= __QWMANGE_MSG_MAX) {
            //������г������ƣ����ǰ��ĳ����ټ���
            m_msg_queue.pop();
        }
        m_msg_queue.push(msg);
    }
    m_cv_queue.notify_all();

}
QFMSG QWMange::popQueue()
{
    std::unique_lock<std::mutex> lock(m_clock_queue, std::defer_lock);
    if (m_msg_queue.size() < 5)lock.lock();
    m_cv_queue.wait(lock, [&]() {return !isRunThread || 0 < m_msg_queue.size(); });
    QFMSG msg;
    if (!m_msg_queue.empty()) {
        
        msg = m_msg_queue.front();
        m_msg_queue.pop();
    }
    return msg;
    

}
void QWMange::LoadMsgModule()
{
    auto* p =  new QMsgGroupLK<std::vector<QWND*>, std::shared_mutex>(&m_shared_lock_ctrl);
    p->AddCtrlArr(&m_QWNDs);
    m_msgModule.push_back(p);
}
void QWMange::UnLoadMsgModule()
{
    for (auto& it : m_msgModule) {
        delete it;
    }
    m_msgModule.clear();
}
void QWMange::msgThreadFun()
{
    m_clock_msgThread_Exit.lock();
    while (isRunThread) {
        //ȡһ����Ϣ
        QFMSG msg = popQueue();
        if (NULL != msg.hwnd) {
            __MsgHandle(msg.msg, msg.wParam, msg.lParam);

        }

    }
    m_clock_msgThread_Exit.unlock();

}
int QWMange::__MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        //�����Ϣ������Ҫ�����޸�
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
        MouseMessage(uMsg, wParam, lParam);
        break;
        //������Ϣ����
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
    case WM_IME_CHAR:
        KeyBoardMessage(uMsg, wParam, lParam);
        break;
    case WM_SIZE: {//���ڴ�С�����ı䴦��
        //if (m_HostHwnd) {
        //    RECT rect;
        //    GetWindowRect(m_HostHwnd, &rect);
        //    m_rect.X = 0;
        //    m_rect.Y = 0;
        //    m_rect.Width = rect.right - rect.left;
        //    m_rect.Height = rect.bottom - rect.top;
        //}
        RECT rect;
        GetWindowRect(m_HostHwnd, &rect);
        if (__listener.__sizeChangeListen) {
            (*__listener.__sizeChangeListen)(this, rect.right - rect.left, rect.bottom - rect.top);
        }

        m_OldChangeTime = std::chrono::high_resolution_clock::now();
        if (!m_isRunWCThread) {
            std::thread t(&QWMange::WindChangeSizeEvent, this);
            t.detach();
        }
        break;
    }




    }

    return 0;
}

void QWMange::WindChangeSizeEvent()
{
    std::unique_lock<std::mutex> lock(m_lock_wcsz, std::try_to_lock);
    if (lock.owns_lock()) {//ȷ��ֻ��һ���߳�ִ��
        m_isRunWCThread = true;
        uint64_t  dta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_OldChangeTime).count();
        while (dta < m_wcCyc) {
  
            dta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_OldChangeTime).count();
        }
        m_sizeChaneg = true;
        //char c[128];
        //sprintf_s(c,"%d",(int)m_HostHwnd);
        //MessageBoxA(0,c,"",0);
        if (m_HostHwnd) {
            RECT rect;
            GetClientRect(m_HostHwnd, &rect);
            
            m_rect.X = 0;
            m_rect.Y = 0;
            m_rect.Width = rect.right - rect.left;
            m_rect.Height = rect.bottom - rect.top;



            //HDC dc = GetDC(m_HostHwnd);
            Reconstruct(NULL, m_rect);
            //::ReleaseDC(0, dc);


        /*    for (int i = -1; ++i < 5;)
                UpdateBK(m_bkBrush);*/
            __createrTime = std::chrono::high_resolution_clock::now();
            QDWORD lp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

            std::shared_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
            for (auto& it : m_QWNDs) {
                it->TransferMessage(QM_PARENTCTRL_SIZE_CHANGE, lp, 0, 0);//֪ͨ�ı�ؼ��Ĵ�С
            }
            isChange = true;
        }
        RECT rect;
        GetWindowRect(m_HostHwnd, &rect);
        if (__listener.__sizeChangedListen) {
            (*__listener.__sizeChangedListen)(this, rect.right - rect.left, rect.bottom - rect.top);
        }
        m_isRunWCThread = false;
 
    }
}

QLSizeChangeListener* QWMange::SetSizeChangeListen(QLSizeChangeListener* listen)
{
    QLSizeChangeListener* ret = __listener.__sizeChangeListen;

    __listener.__sizeChangeListen = listen;

    return ret;
   
}


QLSizeChangeListener* QWMange::SetSizeChangedListen(QLSizeChangeListener* listen)
{
    QLSizeChangeListener* ret = __listener.__sizeChangedListen;

    __listener.__sizeChangedListen = listen;

    return ret;
    

}

int QWMange::SetBKBrush(GBrush* brush)
{
    if (m_bkBrush) { 
        delete m_bkBrush;
        m_bkBrush = NULL;
    }
    int ret = 2;
    if (brush) {
        m_bkBrush = brush->Clone();
        ret = 1;
    }
    UpdateBK(m_bkBrush);
    return ret;
   
}

int QWMange::SetBKImage(const wchar_t* path)
{
    if (m_pImage) {
        delete m_pImage;
        m_pImage = NULL;
        
    }

    m_pImage = new GImage(path);
    UpdateBK(m_bkBrush);
    return 1;
}

void QWMange::SetTransparency(unsigned char apha)
{
    if (!imageAttr) {
        imageAttr = new Gdiplus::ImageAttributes();
    }

    Gdiplus::ColorMatrix colorMatrix = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, (float)apha / 0xff, 0.0f,  // ����͸����ͨ��Ϊ0.5
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    imageAttr->SetColorMatrix(&colorMatrix);
    UpdateBK(m_bkBrush);
   
}

void QWMange::ClearBKImage()
{
    if (m_pImage) {
        delete m_pImage;
        m_pImage = NULL;
    }
    UpdateBK(m_bkBrush);
}

QDWORD QWMange::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
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
    case QM_TO_CLIENT_XY: 
        return QMSGDEF_RET;
    case QM_GET_CLIENT_SIZE: {
        QDWORD* plp = (QDWORD*)LParam;

        *plp = (QDWORD(m_rect.Width) << 32) | (QDWORD(m_rect.Height) & 0xffffffff);

        return QMSGDEF_RET;
    }
    case QM_SET_POPDIALOG: {

        std::unique_lock<std::shared_mutex> ilk(m_invertPopDLK);
        QPopDialogBase* popDialog = (QPopDialogBase*)LParam;
        QPopDialogBase* p=m_popDialog.load();
        if (p ) {

            p->Inverted(QPOINTER(popDialog));
        }
        m_popDialog.store(popDialog);
  
    }return QMSGDEF_RET;
    case QM_GET_POPDIALOG: {

        QWND* popDialog = (QWND*)m_popDialog.load();
        return QDWORD(popDialog);

    }
    case QM_UPDATE_CTRL: {
        isChange = true;
    }break;
    default:
        QDWORD ret = 0;
        for (auto& it : m_msgModule) {
            if (it->MsgProcess(msgID, LParam, RParam, AddParam, &ret))return ret;

        }
        break;
    }
   
    QWND::TransferMessage(msgID, LParam, RParam, AddParam);
}

int QWMange::GetSubCtrlCount()
{

    return m_QWNDs.size();
}

QWND* QWMange::GetSubCtrl(int index)
{
    std::unique_lock<std::shared_mutex> lock(m_shared_lock_ctrl);
    if (index<0 || index> m_QWNDs.size() - 1)return nullptr;

    return m_QWNDs[index];
}


extern "C"
__declspec(dllexport) void QWMAttachCtrl(QWMange * qwm, QWND * w)
{

    qwm->AttachCtrl(w);
}

extern "C" __declspec(dllexport) void QWMRemoveCtrl(QWMange * qwm, QWND * w)
{
    qwm->RemoveCtrl(w);
}



extern "C" __declspec(dllexport) GBrush * QWMGetBKBrush(QWMange * qwm)
{
    return qwm->GetBKBrush();
}

extern "C" __declspec(dllexport) QLSizeChangeListener * QWMSetSizeChangeListen(QWMange * qwm, QLSizeChangeListener * listen)
{
    return qwm->SetSizeChangeListen(listen);
}
extern "C" __declspec(dllexport) QLSizeChangeListener * QWMSetSizeChangedListen(QWMange * qwm, QLSizeChangeListener * listen)
{
    return qwm->SetSizeChangedListen(listen);
}

extern "C" __declspec(dllexport) QWND * CreateQWMange(HDC winddc, GRect  rf)
{
    
    return new QWMange(winddc, rf);
}
extern "C" __declspec(dllexport) void DeleteQWMange(QWND * wnd)
{

    delete wnd;
}

extern "C" __declspec(dllexport) int QWMSetBKBrush(QWMange * qwm,GBrush* brush)
{

    return qwm->SetBKBrush(brush);
}

extern "C" __declspec(dllexport) int QWMSetBKImage(QWMange * qwm, const wchar_t* path)
{
    return qwm->SetBKImage(path);
}
extern "C" __declspec(dllexport) void QWMSetTransparency(QWMange * qwm, unsigned char apha)
{
    qwm->SetTransparency(apha);
}
extern "C" __declspec(dllexport) void QWMClearBKImage(QWMange * qwm)
{
    qwm->ClearBKImage();
}



//**************************���Դ���д�±�***************************************************
#ifdef __TEST_CODE_QWM
void QWMange::DrawBK()
{
    if (m_bkdc) {
        //����һ�����仭ˢ
        Gdiplus::LinearGradientBrush LB(GPointF(0.0, 0.0), GPointF(m_rect.Width, m_rect.Height), GColor(255, 0xCC, 0xFB, 0xFF), GColor(255, 0xEF, 0X96, 0Xc5));
        UpdateBK(&LB);

    }



}


#endif







#include "pch.h"