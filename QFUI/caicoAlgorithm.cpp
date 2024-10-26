#include "caicoAlgorithm.h"
#pragma comment (lib,"Msimg32.lib")
void draw_gradient_ring(cairo_t* cr, double centerX, double centerY, double radius, double thickness,unsigned int color1, unsigned int color2, bool* endFlag)
{
    // ������ʼ��ɫ�ͽ�����ɫ
    double start_r = ((color1>>16)&0xff)/255.0; 
    double start_g = ((color1 >> 8) & 0xff) / 255.0;
    double start_b = ((color1) & 0xff) / 255.0;

    double end_r = ((color2 >> 16) & 0xff) / 255.0;
    double end_g = ((color2 >> 8) & 0xff) / 255.0;
    double end_b = ((color2) & 0xff) / 255.0;

    // ���㽥�����ɫ����
    int num_steps = 360;

    for (double i = 0; i < num_steps; i += 0.1) {
        if (*endFlag)return;
        // ������ɫ�Ĳ�ֵ
        double t = static_cast<double>(i) / 360.0;
        double r = (1 - t) * start_r + t * end_r;
        double g = (1 - t) * start_g + t * end_g;
        double b = (1 - t) * start_b + t * end_b;

        // ����Բ����ÿ���ε�λ��
        double angle1 = i * (M_PI / 180.0f);
        double angle2 = (i + 0.1) * (M_PI / 180.0f);

        double x1 = centerX + radius * cos(angle1);
        double y1 = centerY + radius * sin(angle1);

        double x2 = centerX + radius * cos(angle2);
        double y2 = centerY + radius * sin(angle2);

        double x3 = centerX + (radius - thickness) * cos(angle2);
        double y3 = centerY + (radius - thickness) * sin(angle2);

        double x4 = centerX + (radius - thickness) * cos(angle1);
        double y4 = centerY + (radius - thickness) * sin(angle1);

        // ����·�������
        cairo_set_source_rgb(cr, r, g, b);
        cairo_move_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_line_to(cr, x3, y3);
        cairo_line_to(cr, x4, y4);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
}
void draw_circle(cairo_t* cr, double centerX, double centerY, double radius )
{
    
    cairo_arc(cr, centerX, centerY, radius, 0, 2 * M_PI);
    cairo_close_path(cr);
}

void draw_circleT(cairo_t* cr, double centerX, double centerY, double radius, double thickness)
{

    cairo_arc(cr, centerX, centerY, radius, 0, 2*0.4 * M_PI);
    
    cairo_close_path(cr);
}
// �� Cairo ͼ�����ݻ��Ƶ� HDC ��
void DrawCairoContentToHDC(HDC hdc,int dx,int dy,int dw,int dh, cairo_surface_t* surface,int sx,int sy)
{
    // ��ȡͼ�����Ŀ��
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    // ��ȡͼ������ָ��
    unsigned char* data = cairo_image_surface_get_data(surface);

    // ��ȡͼ���ʽ�������� CAIRO_FORMAT_ARGB32��
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // ע�⣺������ʾ���϶��µ� DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;     // ÿ���� 32 λ��ARGB��
    bmi.bmiHeader.biCompression = BI_RGB;
    if (dw < 0)dw = width;
    if (dh < 0)dh = height;

    // �������ݵ� DIB
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, hBitmap);

    // ʹ�� GDI ������ Cairo ͼ�����ݸ��Ƶ� HDC
    SetDIBitsToDevice(
        memDC,
        0, 0,             // Ŀ���豸���Ͻ�λ��
        width, height,    // Ŀ�������С
        0, 0,             // Դͼ����ʼλ��
        0, height,        // ��ʼɨ����
        data,             // ͼ������ָ��
        &bmi,             // ͼ���ʽ��Ϣ
        DIB_RGB_COLORS    // ��ɫ��ʽ
    );

    // ����͸���Ⱥϳ�ģʽ
    BLENDFUNCTION blendFunc;
    blendFunc.BlendOp = AC_SRC_OVER;
    blendFunc.BlendFlags = 0;
    blendFunc.SourceConstantAlpha = 255;
    blendFunc.AlphaFormat = AC_SRC_ALPHA;
    // ͨ��͸���Ⱥϳɺ����� DIB ���Ƶ�Ŀ�� HDC ��
    AlphaBlend(hdc, dx, dy, dw, dh, memDC, sx, sy, width, height, blendFunc);
    //BitBlt(hdc, dx, dy, dw, dh, memDC,0,0, SRCCOPY);
    // ����
    DeleteObject(hBitmap);
    DeleteDC(memDC);
}