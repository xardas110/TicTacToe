#include "Raycast.h"

RayCast::RayCast(_In_ const glm::vec3 orig, _In_ const glm::vec3 dir)
	:ORIG(_mm_load_ps(&orig.x)), DIR(_mm_load_ps(&dir.x))
{
	INVDIR = _mm_div_ps({1.f, 1.f, 1.f, 1.f}, DIR);
	ORIG.m128_f32[3] = DIR.m128_f32[0] < 0.f;
	DIR.m128_f32[3] = DIR.m128_f32[0] < 0.f;
	INVDIR.m128_f32[3] = DIR.m128_f32[0] < 0.f;
}

bool RayCast::Intersect(_In_ const Bounding::Box& b)
{
	float  txMin, txMax, tyMin, tyMax, tzMin, tzMax;
	txMin = (b.Bounds[(int)(this->ORIG.m128_f32[3])].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];
	txMax = (b.Bounds[1 - (int)this->ORIG.m128_f32[3]].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];

	tyMin = (b.Bounds[(int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];
	tyMax = (b.Bounds[1 - (int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];

	if ((txMin > tyMax) || (tyMin > txMax))
		return false;
	if (tyMin > txMin)
		txMin = tyMin;
	if (tyMax < txMax)
		txMax = tyMax;

	tzMin = (b.Bounds[(int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];
	tzMax = (b.Bounds[1 - (int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];

	if ((txMin > tzMax) || (tzMin > txMax))
		return false;
	if (tzMin > txMin)
		txMin = tzMin;
	if (tzMax < txMax)
		txMax = tzMax;

	return true;
}