#include "Pyramid.h"

PPyramid::PPyramid() 
{}

PPyramid::~PPyramid() 
{
	Release();
}

void PPyramid::Create(string name) 
{
	m_texture.Load(name);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	//glm::vec2 TextCoord = glm::vec2( 0.0f, 0.0f);

	//glm::vec3 pyramidNormals = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 pyramidSides[16]
	{
		////front face
		glm::vec3(-1.0, 0.0, 1.0),
		glm::vec3(1.0, 0.0, 1.0),
		glm::vec3(0.0, 2.0, 0.0),

		//glm::vec3(-1.0,-1.0, 1.0),
		//glm::vec3(1.0, -1.0, 1.0),
		//glm::vec3(0.0,1.0,0.0),

		////right side face
		glm::vec3(1.0, 0.0, 1.0),
		glm::vec3(1.0, 0.0, -1.0),
		glm::vec3(0.0, 2.0, 0.0),

		//glm::vec3(1.0, -1.0, 1.0),
		//glm::vec3(1.0,-1.0,-1.0),
		//glm::vec3(0.0,1.0,0.0),

		////back face
		glm::vec3(-1.0, 0.0, -1.0),
		glm::vec3(1.0, 0.0, -1.0),
		glm::vec3(0.0, 2.0, 0.0),
		//glm::vec3(1.0, -1.0, -1.0),
		//glm::vec3(-1.0, -1.0, -1.0),
		//glm::vec3(0.0, 1.0, 0.0),
		////left side face
		glm::vec3(-1.0, 0.0, 1.0),
		glm::vec3(-1.0, 0.0, -1.0),
		glm::vec3(0.0, 2.0, 0.0),
		//glm::vec3(-1.0, -1.0, -1.0),
		//glm::vec3(-1.0,-1.0, 1.0),
		//glm::vec3(0.0, 1.0, 0.0),

		//bottom
		glm::vec3(-1.0, 0.0, 1.0),
		glm::vec3(1.0, 0.0, 1.0),
		glm::vec3(-1.0, 0.0, -1.0),
		glm::vec3(1.0, 0.0, -1.0),
		//glm::vec3(-1,-1,1),
		//glm::vec3(1,-1,1),
		//glm::vec3(-1,-1,-1),
		//glm::vec3(1,-1,-1),

	};


	glm::vec2 TextCoord[3]{
		glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
	};


	glm::vec3 pyramidNormals[5]
	{ 
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),


		////front
		//glm::vec3(0.0f, 1.0f, 1.0f),
		////right
		//glm::vec3(1.0f, 1.0f, 0.0f),
		////back
		//glm::vec3(0.0f, 1.0f,-1.0f),
		////left
		//glm::vec3(-1.0f, 1.0f, 0.0f),
		////base
		//glm::vec3(0.0f, -1.0f, 0.0f),
	};

	//add the points of pyramids
	for (int i = 0; i < 16; i++) {
		m_vbo.AddData(&pyramidSides[i], sizeof(glm::vec3));
		m_vbo.AddData(&TextCoord[i%3] , sizeof(glm::vec2));
		m_vbo.AddData(&pyramidNormals[i%3] , sizeof(glm::vec3));
	}
	glm::vec2 pyramidBaseCoord[4]{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)

	
	};
	
	 


	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void PPyramid::Render() {
	glBindVertexArray(m_vao);
	// Call glDrawArrays to render each side
	m_texture.Bind();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//draw the sides of pyramid
	for (int i = 0; i < 16; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, i*3	, 16);
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PPyramid::Release() {
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();

}