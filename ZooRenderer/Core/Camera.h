#pragma
#include <cmath>
#include "../Math/Vector.h"
#include "../Math/Matrix.h"

//第一人称相机：yaw/pitch决定朝向，WASD移动、QE升降、右键拖动转视角
class Camera {
public:
	Vector3 position{ 0,0,4 };//相机位置，初始在z=4
	float yaw = 0.0f;//水平旋转角，绕y轴，初始朝向-z
	float pitch = 0.0f;//垂直旋转角，绕x轴，初始水平
	float moveSpeed = 4.0f;//移动速度，单位每秒
	float mouseSensitivity = 0.0025f;//鼠标灵敏度，单位像素转弧度

	Vector3 Forward() const {
		//Q 这个方法是干嘛的，为什么要加上const
		//A 这个方法是用来计算相机的前向方向向量的。它根据当前的yaw和pitch角度，使用三角函数计算出一个单位向量，表示相机朝向的方向。加上const是为了表明这个方法不会修改Camera对象的状态，可以在const对象上调用这个方法。
		return Vector3(
			std::sin(yaw) * std::cos(pitch),
			std::sin(pitch),
			-std::cos(yaw) * std::cos(pitch)
		).Normalized();
	}

	Vector3 Right() const {
		return Vector3(std::cos(yaw), 0, std::sin(yaw)).Normalized();
		//Q 这里是不是没必要调用Normalized()了，因为这个向量本来就是单位向量
		//A 是的，这个向量本来就是单位向量，所以调用Normalized()是多余的。不过调用它也不会有性能问题，因为它会检测到向量已经是单位向量并直接返回。
	}

	Matrix4x4 ViewMatrix() const {
		return Matrix4x4::LookAt(position, position + Forward(), { 0,1,0 });
		//Q 为啥这里up参数直接写死为{0,1,0}了？如果相机pitch旋转到90度了，这个up就不对了吧？会不会有问题？为什么不根据当前的yaw/pitch计算出一个正确的up向量呢？
		//A 这里up参数直接写死为{0,1,0}是因为我们假设相机的上方向始终与世界坐标系的y轴对齐，这样可以简化计算并避免一些特殊情况。虽然当pitch旋转到90度时，up向量可能不再垂直于forward向量，但在大多数情况下，这种简单的处理方式是足够的，并且不会引起明显的问题。如果需要更复杂的相机控制，可以考虑使用四元数来表示旋转，这样就可以避免万向锁问题，并且能够正确地计算出up向量。
	}

	//keys数组包含了当前帧所有按键的状态，dt是距离上一帧的时间，单位秒
	void Update(const bool* keys, float dt) {
		Vector3 f = Forward();
		Vector3 r = Right();
		Vector3 worldUp{ 0,1,0 };
		float v = moveSpeed * dt;
		if (keys['W']) position = position + f * v;
		if (keys['S']) position = position - f * v;
		if (keys['A']) position = position - r * v;
		if (keys['D']) position = position + r * v;
		if (keys['Q']) position = position - worldUp * v;
		if (keys['E']) position = position + worldUp * v;
	}

	//dx、dy是鼠标相对于上一帧的移动距离，单位像素
	void Look(float dx, float dy) {
		yaw += dx * mouseSensitivity;
		pitch -= dy * mouseSensitivity;//鼠标y向下是正，所以这里要减去
		const float limit = 1.55f;//接近90度时会有万向锁问题，限制一下
		if (pitch > limit) pitch = limit;
		if (pitch < -limit) pitch = -limit;
	}
};