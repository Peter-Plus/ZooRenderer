#include <iostream>
#include "Math/Vector.h"
#include "Math/Matrix.h"

void Print(const char* tag, const Vector4& v) {
    std::cout << tag << " = (" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
}

int main() {
    const float PI = 3.14159265f;

    Vector4 point(0, 0, 0, 1);   // 一个点
    Vector4 dir(1, 1, 1, 0);   // 一个方向

    Matrix4x4 T = Matrix4x4::Translate({ 1, 2, 3 });
    Print("T * point", T * point);                          // (1, 2, 3, 1)
    Print("T * dir  ", T * dir);                            // (1, 1, 1, 0) 方向不被平移

    Matrix4x4 S = Matrix4x4::Scale({ 2, 3, 4 });
    Print("S * (1,1,1)", S * Vector4(1, 1, 1, 1));          // (2, 3, 4, 1)

    Matrix4x4 Rz = Matrix4x4::RotateZ(PI / 2);
    Print("Rz90 * (1,0,0)", Rz * Vector4(1, 0, 0, 1));      // (0, 1, 0, 1)

    // 组合：先旋转再平移（右边的 Rz 先生效）
    Matrix4x4 M = Matrix4x4::Translate({ 5, 0, 0 }) * Matrix4x4::RotateZ(PI / 2);
    Print("(T*Rz)*(1,0,0)", M * Vector4(1, 0, 0, 1));       // (5, 1, 0, 1)
    return 0;
}