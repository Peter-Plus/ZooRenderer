#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Core/Framebuffer.h"
#include "Core/Window.h"
#include "Core/Raster.h"
#include "Core/Camera.h"
#include <chrono>

int main() {
    const int   W = 800, H = 600;
    const float PI = 3.14159265f;

    Window      window(W, H, L"ZooRenderer - Solid Cube (Z-Buffer)");
    Framebuffer fb(W, H);

    Camera cam;
    cam.position = { 3.0f, 2.5f, 3.5f };
    cam.yaw = -0.7f; cam.pitch = -0.5f;   // 从一个角斜看立方体，一开始就能看到三个面

    Matrix4x4 proj = Matrix4x4::Perspective(PI / 3, float(W) / H, 0.1f, 100.0f);

    Vector3 verts[8] = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
    };
    struct Tri { int a, b, c; Vector3 color; };
    Tri tris[12] = {
        {0,1,2,{0.90f,0.25f,0.25f}}, {0,2,3,{0.90f,0.25f,0.25f}},  // 前 红
        {5,4,7,{0.25f,0.80f,0.35f}}, {5,7,6,{0.25f,0.80f,0.35f}},  // 后 绿
        {4,0,3,{0.30f,0.50f,0.90f}}, {4,3,7,{0.30f,0.50f,0.90f}},  // 左 蓝
        {1,5,6,{0.95f,0.85f,0.25f}}, {1,6,2,{0.95f,0.85f,0.25f}},  // 右 黄
        {3,2,6,{0.25f,0.80f,0.85f}}, {3,6,7,{0.25f,0.80f,0.85f}},  // 上 青
        {4,5,1,{0.85f,0.35f,0.80f}}, {4,1,0,{0.85f,0.35f,0.80f}},  // 下 品红
    };

    auto prev = std::chrono::high_resolution_clock::now();

    while (window.ProcessMessages()) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;

        float mdx, mdy; window.ConsumeMouseDelta(mdx, mdy);
        cam.Look(mdx, mdy);
        cam.Update(window.Keys(), dt);

        Matrix4x4 mvp = proj * cam.ViewMatrix();   // model = 单位矩阵

        // 顶点 → 屏幕坐标 + NDC 深度
        Vector3 sp[8];
        bool    valid[8];
        for (int i = 0; i < 8; ++i) {
            Vector4 clip = mvp * Vector4(verts[i], 1.0f);
            valid[i] = clip.w > 1e-4f;             // 在相机前方才有效
            if (!valid[i]) continue;
            Vector3 ndc = clip.PerspectiveDivide();
            sp[i] = { (ndc.x * 0.5f + 0.5f) * W,
                      (1.0f - (ndc.y * 0.5f + 0.5f)) * H,
                      ndc.z };
        }

        fb.Clear(Framebuffer::MakeColor(15, 15, 22));
        fb.ClearDepth();                            // 每帧重置深度缓冲！

        for (const Tri& t : tris) {
            if (!valid[t.a] || !valid[t.b] || !valid[t.c]) continue;  // 跨相机平面的暂时跳过（阶段8做裁剪）
            DrawTriangle(fb, sp[t.a], sp[t.b], sp[t.c], t.color, t.color, t.color);
        }

        window.Present(fb);
    }
    return 0;
}