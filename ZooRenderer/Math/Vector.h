#pragma once
#include <cmath>

// Vector2 UV坐标，屏幕坐标
struct Vector2 {
	float x = 0, y = 0;
	Vector2() = default;// 构造函数
	Vector2(float x, float y) : x(x), y(y) {}// 构造函数

	Vector2 operator+(const Vector2& v) const {
		return { x + v.x, y + v.y };
	}
	//Q 这里两个const分别代表什么含义，有什么作用？
	//A 第一个const表示这个函数不会修改成员变量，第二个const表示这个函数不会修改传入的参数v。这两个const都是为了保证代码的安全性和可读性，告诉调用者这个函数不会改变对象的状态或参数的值。
	//Q operator+(const Vector2& v)里的const是什么含义，有什么作用？
	//A 这里的const表示这个函数不会修改传入的参数v。它告诉调用者这个函数不会改变参数v的值，这样可以提高代码的安全性和可读性。
	//Q 为什么v用引用传递而不是值传递？有什么好处？
	//A 使用引用传递而不是值传递可以避免不必要的对象复制，提高性能。对于较大的对象，值传递会涉及到对象的复制，这可能会导致性能下降。而引用传递只传递对象的地址，不需要复制整个对象，因此更高效。
	Vector2 operator-(const Vector2& v) const {
		return { x - v.x, y - v.y };
	}
	Vector2 operator*(float s) const {
		return{ x * s,y * s };
	}
	float Dot(const Vector2& v) const {
		return x * v.x + y * v.y;
	}// 点积 = |A| * |B| * cos(θ) = Ax * Bx + Ay * By
	float Length() const {
		return std::sqrt(Dot(*this));
	}

};


struct Vector3 {
	float x = 0, y = 0, z = 0;
	Vector3() = default;
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 operator+(const Vector3& v) const {
		return { x + v.x, y + v.y, z + v.z };
	}
	Vector3 operator-(const Vector3& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}
	Vector3 operator-() const {
		return { -x, -y, -z };
	}
	Vector3 operator*(float s) const {
		return { x * s, y * s, z * s };
	}
	Vector3 operator/(float s) const {
		return { x / s, y / s, z / s };
	}

	// 点积 —— 求夹角、投影，光照里的N·L
	float Dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	// 叉积 —— 结果向量垂直于ab，求法线，求垂直向量、判断三角形正反面
	Vector3 Cross(const Vector3& v) const {
		return {
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		};
	}
	/*a × b
		= (a2b3 - a3b2)i
		+ (a3b1 - a1b3)j
		+ (a1b2 - a2b1)k*/
	float Length() const {
		return std::sqrt(Dot(*this));
	}

	//归一化
	Vector3 Normalized() const {
		float len = Length();
		return len > 1e-6f ? *this / len : Vector3();
		//Q 这里为什么要判断len > 1e-6f？有什么作用？
		//A 这里判断len > 1e-6f是为了避免除以零或非常小的数，导致数值不稳定或溢出。
		// 如果len非常小，说明向量的长度接近于零，无法进行有效的归一化操作，因此返回一个默认的零向量。

	}
};

inline Vector3 operator*(float s, const Vector3& v) {
	return v * s;
}// 允许float在左，Vector3在右的乘法，例如：2.0f * v
//Q这是个什么东西？inline是干嘛的，这个函数是干嘛的
//A 这是一个全局函数，允许float在左，Vector3在右的乘法，例如：2.0f * v。
// inline是一个建议编译器将函数体直接插入到每个调用点的关键字，这样可以避免函数调用的开销，提高性能。
// 这个函数的作用是实现了float和Vector3之间的乘法运算，使得代码更加灵活和易读。
//Q 这个函数为什么要定义为inline？有什么好处？
//A 定义为inline可以建议编译器将函数体直接插入到每个调用点，避免函数调用的开销，提高性能。对于这样一个简单的函数，inline可以使得代码更加高效，同时也可以提高代码的可读性，因为它允许我们使用更自然的语法来进行乘法运算。
//Q v3不是定义了operator*(float s)吗？为什么还需要这个函数？
//A v3定义了operator*(float s)，允许Vector3在左，float在右的乘法，例如：v * 2.0f。但是，如果我们想要支持float在左，Vector3在右的乘法，例如：2.0f * v，我们就需要这个全局函数来实现这个操作。因为成员函数只能定义一种顺序的操作，而全局函数可以定义另一种顺序的操作，从而提供更灵活的语法支持。
inline Vector3 Hadamard(const Vector3& a, const Vector3& b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}// 哈达玛积，分量乘积，a和b对应分量相乘，常用于颜色计算
//Q 这不就是点积吗？为什么叫哈达玛积？有什么区别？
//A 哈达玛积和点积是不同的运算。点积是将两个向量的对应分量相乘后求和，得到一个标量值。而哈达玛积是将两个向量的对应分量相乘，得到一个新的向量。哈达玛积常用于颜色计算，因为它可以直接对颜色的每个分量进行操作，而点积则不适合这种用途。
//Q 哈达玛积是干什么用的，为什么不能定义为V3的方法这个函数为什么要定义为inline？
//A 哈达玛积常用于颜色计算，因为它可以直接对颜色的每个分量进行操作。这个函数定义为inline是为了提高性能，避免函数调用的开销。由于哈达玛积是一个简单的操作，inline可以使得代码更加高效，同时也可以提高代码的可读性，因为它允许我们使用更自然的语法来进行分量乘积运算。这个函数不能定义为Vector3的方法，因为它需要同时操作两个Vector3对象，而成员函数只能操作一个对象，因此需要定义为全局函数。
//Q 那直接定义一个全局函数不就行了，为什么要定义为inline？有什么好处？
//A 定义为inline可以建议编译器将函数体直接插入到每个调用点，避免函数调用的开销，提高性能。对于这样一个简单的函数，inline可以使得代码更加高效，同时也可以提高代码的可读性，因为它允许我们使用更自然的语法来进行分量乘积运算。虽然直接定义一个全局函数也可以实现相同的功能，但定义为inline可以进一步优化性能，特别是在频繁调用这个函数的情况下。
inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	return a * (1 - t) + b * t;
}// 线性插值，a和b之间t比例的点，常用于动画、渐变等

// Vector4 齐次坐标
struct Vector4 {
	float x = 0, y = 0, z = 0, w = 0;
	Vector4() = default;
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

	Vector4 operator+(const Vector4& v) const {
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}
	Vector4 operator-(const Vector4& v) const {
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}
	Vector4 operator*(float s) const {
		return { x * s, y * s, z * s, w * s };
	}
	Vector3 XYZ() const {
		return { x, y, z };
	}
	//透视除法，将齐次坐标转换为三维坐标，得到NDC
	//Q NDC是什么？为什么要进行透视除法？有什么作用？
	//A NDC是归一化设备坐标（Normalized Device Coordinates）的缩写，是一种将三维坐标转换为二维屏幕坐标的坐标系统。透视除法是将齐次坐标中的x、y、z分量除以w分量的过程，这样可以将齐次坐标转换为三维坐标。透视除法的作用是实现透视投影，使得远处的物体看起来更小，近处的物体看起来更大，从而增强了场景的深度感和真实感。
	Vector3 PerspectiveDivide() const {
		return w != 0 ? XYZ() / w : XYZ();
	}
};
