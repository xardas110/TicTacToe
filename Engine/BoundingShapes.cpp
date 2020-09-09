#include "BoundingShapes.h"

void _vectorcall Bounding::Box::SetCenter(glm::vec3 pos)
{
	Center = _mm_load_ps(&pos.x);
	UpdateBounds();
}

void _vectorcall Bounding::Box::SetExtents(glm::vec3 scale)
{
	Extents = _mm_load_ps(&scale.x);
	UpdateBounds();
}

void Bounding::Box::UpdateBounds()
{
	Bounds[0] = _mm_sub_ps(Center, Extents);
	Bounds[1] = _mm_add_ps(Center, Extents);
}