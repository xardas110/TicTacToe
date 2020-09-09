#pragma once
#include <memory>
class Mesh
{
private:
	unsigned int ID;
	unsigned int indexCount;
	Mesh(const Mesh& copy) = delete;
	Mesh() = delete;
	Mesh(unsigned int id, unsigned int iCount);
	
public:
	const unsigned int				GetNumTriangles() const;
	void Draw						(GLenum mode = GL_TRIANGLES);
	static std::unique_ptr<Mesh>	CreateQuad(const unsigned int s = 1.f);
	//*First argument specifies how many vertex edges the circle has, for ultra low poly I just choose to default this at 6 vertex edges*//
	static std::unique_ptr<Mesh>	CreateCircle(const unsigned int vertCount = 6U, const float radius = 0.5f);
	static std::unique_ptr<Mesh>	CreateGrid(const unsigned int nrColumns, const unsigned int nrRows, const float xGridSize = 1, const float yGridSize = 1);
};
