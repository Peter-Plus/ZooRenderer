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

    Window      window(W, H, L"ZooRenderer - Camera");
    Framebuffer fb(W, H);

    Camera cam;
    cam.position = { 0, 1.5f, 5 };
    cam.pitch = -0.15f;   // 略微低头，把地面网格和立方体框进画面

    Matrix4x4 proj = Matrix4x4::Perspective(PI / 3, float(W) / H, 0.1f, 100.0f);

    // 立方体（世界原点，边长 2）
    Vector3 verts[8] = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
    };
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, {4,5},{5,6},{6,7},{7,4}, {0,4},{1,5},{2,6},{3,7},
    };

    // 把世界坐标点投影到屏幕；返回 false 表示该点在相机平面/背后（暂时丢弃，阶段7做正式裁剪）
    auto project = [&](const Matrix4x4& m, const Vector3& p, float& sx, float& sy) -> bool {
        Vector4 clip = m * Vector4(p, 1.0f);
        if (clip.w < 1e-4f) return false;
        Vector3 ndc = clip.PerspectiveDivide();
        sx = (ndc.x * 0.5f + 0.5f) * W;
        sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * H;
        return true;
        };
    auto drawWorldLine = [&](const Matrix4x4& m, const Vector3& a, const Vector3& b, uint32_t col) {
        float ax, ay, bx, by;
        if (project(m, a, ax, ay) && project(m, b, bx, by))
            DrawLine(fb, int(ax), int(ay), int(bx), int(by), col);
        };

    auto prev = std::chrono::high_resolution_clock::now();

    while (window.ProcessMessages()) {
        // 帧时间（秒）
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;

        // 输入 → 相机
        float mdx, mdy;
        window.ConsumeMouseDelta(mdx, mdy);
        cam.Look(mdx, mdy);
        cam.Update(window.Keys(), dt);

        // 没有 model 变换，所以这就是 view-projection
        Matrix4x4 vp = proj * cam.ViewMatrix();

        fb.Clear(Framebuffer::MakeColor(15, 15, 22));

        // 地面网格（y = -1.5 平面，-10..10），给相机移动提供参照
        uint32_t gridCol = Framebuffer::MakeColor(55, 55, 70);
        for (int i = -10; i <= 10; ++i) {
            drawWorldLine(vp, { float(i), -1.5f, -10 }, { float(i), -1.5f, 10 }, gridCol);
            drawWorldLine(vp, { -10, -1.5f, float(i) }, { 10, -1.5f, float(i) }, gridCol);
        }

        // 立方体
        uint32_t cubeCol = Framebuffer::MakeColor(120, 230, 160);
        for (int e = 0; e < 12; ++e)
            drawWorldLine(vp, verts[edges[e][0]], verts[edges[e][1]], cubeCol);

        window.Present(fb);
    }
    return 0;
}