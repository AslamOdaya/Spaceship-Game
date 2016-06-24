#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"



class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();
	vector<glm::vec3> GetLeftObjectPoints();
	vector<glm::vec3> GetCentrelinePoints();
	vector<glm::vec3> GetRightObjectPoints();
	void CreateTrack(string filename);
	void RenderTrack();
	void RenderObjectPath();
	void CreateOjectPath();
	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = glm::vec3(0, 0, 0)); // Return a point on the centreline based on a certain distance along the control curve.
	bool SampleSides(float d, glm::vec3 &p, vector<glm::vec3> &pointVec, glm::vec3 &up = glm::vec3(0, 0, 0));
private:

	void SetControlPoints();
	
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);
	float m_currentDistance;
	float m_w;
	vector<float> m_distances;
	CTexture m_texture;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoLeftObjectCurve;
	GLuint m_vaoRightObjectCurve;
	GLuint m_vaoTrack;

	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points
	vector<glm::vec3> m_leftObjectPoints;  //Left object points
	vector<glm::vec3> m_rightObjectPoints; //right object points

	unsigned int m_vertexCount;				// Number of vertices in the track VBO
};
