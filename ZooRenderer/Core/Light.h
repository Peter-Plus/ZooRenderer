#pragma once
#include "../Math/Vector.h"

inline Vector3 ShadeDiffuse(const Vector3& base, const Vector3& N,
	const Vector3& L, const Vector3 lightColor, float ambient) {
	float ndl = N.Normalized().Dot(L.Normalized());
	if (ndl < 0.0f) ndl = 0.0f;
	float intensity = ambient + (1.0f - ambient) * ndl;
	return Hadamard(base, lightColor) * intensity;
}