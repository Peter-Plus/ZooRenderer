#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Core/Framebuffer.h"
#include "Core/Window.h"
#include "Core/Raster.h"
#include "Core/Camera.h"
#include "Core/Clip.h"
#include "Core/Light.h"
#include <chrono>
#include <cmath>

int main() {
    const int   W = 800, H = 600;
    const float PI = 3.14159265f;

    Window      window(W, H, L"ZooRenderer - Lighting");
    Framebuffer fb(W, H);

    Camera cam;
    cam.position = { 3.0f, 2.5f, 3.5f };
    cam.yaw = -0.7f; cam.pitch = -0.5f;

    Matrix4x4 proj = Matrix4x4::Perspective(PI / 3, float(W) / H, 0.1f, 100.0f);

    Vector3 verts[8] = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
    };
    // 每个面：4 个角（CCW-outward）+ 外法线
    struct Face { int idx[4]; Vector3 normal; };
    Face faces[6] = {
        {{0,3,2,1}, { 0, 0,-1}},   // 前
        {{4,5,6,7}, { 0, 0, 1}},   // 后
        {{1,2,6,5}, { 1, 0, 0}},   // 右
        {{0,4,7,3}, {-1, 0, 0}},   // 左
        {{3,7,6,2}, { 0, 1, 0}},   // 上
        {{0,1,5,4}, { 0,-1, 0}},   // 下
    };

    Vector3 base = { 0.90f, 0.82f, 0.72f };   // 单一暖色基底
    Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
    float   ambient = 0.15f;
    CullMode cull = CullMode::Back;

    auto toScreen = [&](const Vector4& clip) -> Vector3 {
        Vector3 ndc = clip.PerspectiveDivide();
        return { (ndc.x * 0.5f + 0.5f) * W, (1.0f - (ndc.y * 0.5f + 0.5f)) * H, ndc.z };
        };
    // 一个三角形：近平面裁剪 → 投影 → 背面剔除 → 光栅化（颜色已是光照后的）
    auto drawClipped = [&](const ClipVertex& a, const ClipVertex& b, const ClipVertex& c) {
        ClipVertex in[3] = { a, b, c }, poly[4];
        int n = ClipTriangleNear(in, poly);
        if (n < 3) return;
        for (int k = 1; k < n - 1; ++k) {
            Vector3 s0 = toScreen(poly[0].clip);
            Vector3 s1 = toScreen(poly[k].clip);
            Vector3 s2 = toScreen(poly[k + 1].clip);
            float area = EdgeFunc({ s0.x,s0.y }, { s1.x,s1.y }, { s2.x,s2.y });
            if (ShouldCull(area, cull)) return;     // 整面同朝向，剔了就整面跳过
            DrawTriangle(fb, s0, s1, s2, poly[0].color, poly[k].color, poly[k + 1].color);
        }
        };

    Matrix4x4 model = Matrix4x4::Identity();
    auto prev = std::chrono::high_resolution_clock::now();
    float t = 0.0f;

    while (window.ProcessMessages()) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;
        t += dt;

        float mdx, mdy; window.ConsumeMouseDelta(mdx, mdy);
        cam.Look(mdx, mdy);
        cam.Update(window.Keys(), dt);
        if (window.IsKeyDown('1')) cull = CullMode::None;
        if (window.IsKeyDown('2')) cull = CullMode::Back;
        if (window.IsKeyDown('3')) cull = CullMode::Front;

        // 方向光绕 Y 慢慢转（主要来自上方）
        Vector3 L = Vector3(std::cos(t) * 0.6f, 0.8f, std::sin(t) * 0.6f).Normalized();

        Matrix4x4 mvp = proj * cam.ViewMatrix();

        fb.Clear(Framebuffer::MakeColor(15, 15, 22));
        fb.ClearDepth();

        for (const Face& f : faces) {
            ClipVertex cv[4];
            for (int j = 0; j < 4; ++j) {
                Vector3 wp = verts[f.idx[j]];                            // 世界位置(model=I)
                Vector3 wn = (model * Vector4(f.normal, 0.0f)).XYZ();    // 世界法线(w=0，只受旋转)
                Vector3 lit = ShadeDiffuse(base, wn, L, lightColor, ambient);
                cv[j] = { mvp * Vector4(wp, 1.0f), lit };
            }
            // 四边形 → 2 个三角形
            drawClipped(cv[0], cv[1], cv[2]);
            drawClipped(cv[0], cv[2], cv[3]);
        }

        window.Present(fb);
    }
    return 0;
}