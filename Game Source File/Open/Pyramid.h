#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class PPyramid
{
public:
	PPyramid();
	~PPyramid();
	void Create(string name);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

};