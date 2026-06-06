#pragma once
//Q 这行代码是什么意思？#pragma once是干什么的？有什么作用？
//A #pragma once是一个非标准但被广泛支持的预处理指令，用于防止头文件被多次包含。它告诉编译器在编译过程中只包含一次该头文件，避免了重复定义和编译错误的问题。相比传统的include guard（使用#ifndef、#define、#endif），#pragma once更简洁且不易出错，因此在现代C++开发中被广泛使用。
#include <cmath>
#include "Vector.h"

//约定：列向量，右乘变换矩阵：result = M * v,多个变换矩阵相乘时，先应用的变换在右边，例如：M = T * R * S，先缩放再旋转最后平移
//存储 m[行][列]，例如 m[0][1] 是第一行第二列的元素
struct Matrix4x4 {
	float m[4][4] = {};

	//单位矩阵
	static Matrix4x4 Identity() {
		Matrix4x4 r;
		r.m[0][0] = r.m[1][1] = r.m[2][2] = r.m[3][3] = 1;
		return r;
	}

	// 矩阵乘法 C = A * B
	Matrix4x4 operator*(const Matrix4x4& b) const {
		Matrix4x4 r;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float sum = 0;
				for (int k = 0; k < 4; k++)
				{
					sum += m[i][k] * b.m[k][j];
				}
				r.m[i][j] = sum;
			}
		}
		return r;
	}

	// 矩阵乘向量 v' = M * v或者变换施加到顶点上
	Vector4 operator*(const Vector4& v) const {
		return{
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
		};
	}

	// 平移矩阵 平移项塞进第四列
	static Matrix4x4 Translate(const Vector3& t) {
		Matrix4x4 r = Identity();
		r.m[0][3] = t.x;
		r.m[1][3] = t.y;
		r.m[2][3] = t.z;
		return r;
	}

	// 缩放矩阵 缩放项塞进对角线
	static Matrix4x4 Scale(const Vector3& s) {
		Matrix4x4 r = Identity();
		r.m[0][0] = s.x;
		r.m[1][1] = s.y;
		r.m[2][2] = s.z;
		return r;
	}

	// 旋转矩阵 绕x轴旋转
	static Matrix4x4 RotateX(float rad) {
		Matrix4x4 r = Identity();
		float c = std::cos(rad), s = std::sin(rad);
		r.m[1][1] = c;
		r.m[1][2] = -s;
		r.m[2][1] = s;
		r.m[2][2] = c;
		return r;
	}
	static Matrix4x4 RotateY(float rad) {
		Matrix4x4 r = Identity();
		float c = std::cos(rad), s = std::sin(rad);
		r.m[0][0] = c;
		r.m[0][2] = s;
		r.m[2][0] = -s;
		r.m[2][2] = c;
		return r;
	}
	static Matrix4x4 RotateZ(float rad) {
		Matrix4x4 r = Identity();
		float c = std::cos(rad), s = std::sin(rad);
		r.m[0][0] = c;
		r.m[0][1] = -s;
		r.m[1][0] = s;
		r.m[1][1] = c;
		return r;
	}
	//Q 为什么这些方法要用static修饰？有什么好处？为什么不直接定义为成员函数？
	//A 这些方法用static修饰是因为它们不依赖于任何特定的Matrix4x4实例，而是用于创建新的矩阵对象。定义为static方法可以让我们直接通过类名调用这些方法，例如：Matrix4x4::Translate(t)，而不需要先创建一个Matrix4x4对象。这使得代码更加简洁和易读，同时也明确了这些方法的用途是生成新的矩阵，而不是操作现有的矩阵。如果定义为成员函数，那么我们就需要先创建一个Matrix4x4对象，然后调用这些方法来修改它，这样会增加不必要的复杂性和混淆。
	//Q 那这些方法怎么调用呢？举个例子
	//A 这些方法可以通过类名直接调用，例如：
	// Vector3 t(1, 2, 3);
	// Matrix4x4 translationMatrix = Matrix4x4::Translate(t);
};