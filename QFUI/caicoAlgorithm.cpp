#include "caicoAlgorithm.h"
#pragma comment (lib,"Msimg32.lib")
void draw_gradient_ring(cairo_t* cr, double centerX, double centerY, double radius, double thickness,unsigned int color1, unsigned int color2, bool* endFlag)
{
    // 定义起始颜色和结束颜色
    double start_r = ((color1>>16)&0xff)/255.0; 
    double start_g = ((color1 >> 8) & 0xff) / 255.0;
    double start_b = ((color1) & 0xff) / 255.0;

    double end_r = ((color2 >> 16) & 0xff) / 255.0;
    double end_g = ((color2 >> 8) & 0xff) / 255.0;
    double end_b = ((color2) & 0xff) / 255.0;

    // 计算渐变的颜色步长
    int num_steps = 360;

    for (double i = 0; i < num_steps; i += 0.1) {
        if (*endFlag)return;
        // 计算颜色的插值
        double t = static_cast<double>(i) / 360.0;
        double r = (1 - t) * start_r + t * end_r;
        double g = (1 - t) * start_g + t * end_g;
        double b = (1 - t) * start_b + t * end_b;

        // 计算圆环的每个段的位置
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

        // 创建路径并填充
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
// 将 Cairo 图像内容绘制到 HDC 上
void DrawCairoContentToHDC(HDC hdc,int dx,int dy,int dw,int dh, cairo_surface_t* surface,int sx,int sy)
{
    // 获取图像表面的宽高
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    // 获取图像数据指针
    unsigned char* data = cairo_image_surface_get_data(surface);

    // 获取图像格式（假设是 CAIRO_FORMAT_ARGB32）
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // 注意：负数表示自上而下的 DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;     // 每像素 32 位（ARGB）
    bmi.bmiHeader.biCompression = BI_RGB;
    if (dw < 0)dw = width;
    if (dh < 0)dh = height;

    // 创建兼容的 DIB
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, hBitmap);

    // 使用 GDI 函数将 Cairo 图像数据复制到 HDC
    SetDIBitsToDevice(
        memDC,
        0, 0,             // 目标设备左上角位置
        width, height,    // 目标区域大小
        0, 0,             // 源图像起始位置
        0, height,        // 起始扫描线
        data,             // 图像数据指针
        &bmi,             // 图像格式信息
        DIB_RGB_COLORS    // 颜色格式
    );

    // 设置透明度合成模式
    BLENDFUNCTION blendFunc;
    blendFunc.BlendOp = AC_SRC_OVER;
    blendFunc.BlendFlags = 0;
    blendFunc.SourceConstantAlpha = 255;
    blendFunc.AlphaFormat = AC_SRC_ALPHA;
    // 通过透明度合成函数将 DIB 绘制到目标 HDC 上
    AlphaBlend(hdc, dx, dy, dw, dh, memDC, sx, sy, width, height, blendFunc);
    //BitBlt(hdc, dx, dy, dw, dh, memDC,0,0, SRCCOPY);
    // 清理
    DeleteObject(hBitmap);
    DeleteDC(memDC);
}