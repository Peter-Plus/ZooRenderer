#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Core/Framebuffer.h"
#include "Core/Window.h"
#include "Core/Raster.h"
#include <cmath>

int main() {
    const int   W = 800, H = 600;
    const float PI = 3.14159265f;

    Window      window(W, H, L"ZooRenderer - Wireframe Cube");
    Framebuffer fb(W, H);

    // 立方体的 8 个顶点（局部空间，边长 2，中心在原点）
    Vector3 verts[8] = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},   // 前面 z = -1
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},   // 后面 z = +1
    };
    // 12 条棱（每条连接两个顶点的下标）
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},   // 前面
        {4,5},{5,6},{6,7},{7,4},   // 后面
        {0,4},{1,5},{2,6},{3,7},   // 前后相连
    };

    // 相机放在 z=4，看向原点
    Matrix4x4 view = Matrix4x4::LookAt({ 0,0,4 }, { 0,0,0 }, { 0,1,0 });
    // 投影：垂直视野 60°，近 0.1，远 100
    Matrix4x4 proj = Matrix4x4::Perspective(PI / 3, float(W) / H, 0.1f, 100.0f);

    float angle = 0.0f;
    while (window.ProcessMessages()) {
        fb.Clear(Framebuffer::MakeColor(15, 15, 22));

        // Model：让立方体绕 Y、X 轴慢慢转，制造立体感
        Matrix4x4 model = Matrix4x4::RotateY(angle) * Matrix4x4::RotateX(angle * 0.6f);
        angle += 0.02f;

        Matrix4x4 mvp = proj * view * model;     // 注意顺序：右边的 model 先作用

        // 把 8 个顶点都变换到屏幕坐标
        Vector2 screen[8];
        for (int i = 0; i < 8; ++i) {
            Vector4 clip = mvp * Vector4(verts[i], 1.0f);   // 局部 → 裁剪空间
            Vector3 ndc = clip.PerspectiveDivide();        // ÷w → NDC[-1,1]
            screen[i].x = (ndc.x * 0.5f + 0.5f) * W;        // NDC → 屏幕像素
            screen[i].y = (1.0f - (ndc.y * 0.5f + 0.5f)) * H; // y 翻转
        }

        // 连出 12 条棱
        uint32_t col = Framebuffer::MakeColor(120, 230, 160);
        for (int e = 0; e < 12; ++e) {
            Vector2 a = screen[edges[e][0]];
            Vector2 b = screen[edges[e][1]];
            DrawLine(fb, int(a.x), int(a.y), int(b.x), int(b.y), col);
        }

        window.Present(fb);
    }
    return 0;
}