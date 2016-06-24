#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	m_w = 70.0f;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	 //Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(100, 20, 0));
	m_controlPoints.push_back(glm::vec3(250, 20, 300));
	m_controlPoints.push_back(glm::vec3(100, 20,600));
	m_controlPoints.push_back(glm::vec3(-150, 20, 400));
	m_controlPoints.push_back(glm::vec3(-300, 20, 520));
	m_controlPoints.push_back(glm::vec3(-600, 20, 300));
    m_controlPoints.push_back(glm::vec3(-600, 20, -500));
	m_controlPoints.push_back(glm::vec3(-400, 20, -550));
	m_controlPoints.push_back(glm::vec3(-300, 20, -200));
	m_controlPoints.push_back(glm::vec3(-100, 20, -300));
	m_controlPoints.push_back(glm::vec3(-100, 20, -700));
	m_controlPoints.push_back(glm::vec3(400, 20, -600));
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}



// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

		int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}

bool CCatmullRom::SampleSides(float d, glm::vec3 &p, vector<glm::vec3> &pointVec,glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)pointVec.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(pointVec[iPrev], pointVec[iCur], pointVec[iNext], pointVec[iNextNext], t);
	if (m_controlUpVectors.size() == pointVec.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}




// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	
	// Call Set Control Points
	SetControlPoints();
	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	glm::vec2 textCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 points = glm::vec3(m_centrelinePoints.at(i));
		vbo.AddData(&points, sizeof(glm::vec3));
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}

void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
    glm::vec3 l = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 r = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pNext = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		p = m_centrelinePoints.at(i);
       int pIndex = (i + 1) % m_centrelinePoints.size();

		pNext = m_centrelinePoints.at(pIndex);

		glm::vec3 T = glm::normalize(pNext-p);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		l = p - (m_w / 2)*N;
		r = p + (m_w / 2)*N;

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}
	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 textCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	for (int i = 0; i < m_leftOffsetPoints.size(); i++) {
		glm::vec3 lPoints = glm::vec3(m_leftOffsetPoints.at(i));
		vbo.AddData(&lPoints, sizeof(glm::vec3));
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));


	CVertexBufferObject sVbo;
	sVbo.Create();
	sVbo.Bind();
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	for (int i = 0; i < m_rightOffsetPoints.size(); i++) {
		glm::vec3 rPoints = glm::vec3(m_rightOffsetPoints.at(i));
		sVbo.AddData(&rPoints, sizeof(glm::vec3));
		sVbo.AddData(&textCoord, sizeof(glm::vec2));
		sVbo.AddData(&normal, sizeof(glm::vec3));

	}

	sVbo.UploadDataToGPU(GL_STATIC_DRAW);
	
	// Set the vertex attribute locations
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

	// Note it is possible to only use one VAO / VBO with all the points instead.

}
//vertices used to spawn objects between centreline and the two offset curves.
void CCatmullRom::CreateOjectPath() {
	glm::vec3 l = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pNext = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		p = m_centrelinePoints.at(i);
		int pIndex = (i + 1) % m_centrelinePoints.size();

		pNext = m_centrelinePoints.at(pIndex);

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		l = p - (m_w / 3.5f)*N;
		r = p + (m_w / 3.5f)*N;

		m_leftObjectPoints.push_back(l);
		m_rightObjectPoints.push_back(r);
	}
	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 textCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftObjectCurve);
	for (int i = 0; i < m_leftObjectPoints.size(); i++) {
		glm::vec3 lPoints = glm::vec3(m_leftObjectPoints.at(i));
		vbo.AddData(&lPoints, sizeof(glm::vec3));
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

	CVertexBufferObject sVbo;
	sVbo.Create();
	sVbo.Bind();
	glGenVertexArrays(1, &m_vaoRightObjectCurve);
	glBindVertexArray(m_vaoRightObjectCurve);
	for (int i = 0; i < m_rightObjectPoints.size(); i++) {
		glm::vec3 rPoints = glm::vec3(m_rightObjectPoints.at(i));
		sVbo.AddData(&rPoints, sizeof(glm::vec3));
		sVbo.AddData(&textCoord, sizeof(glm::vec2));
		sVbo.AddData(&normal, sizeof(glm::vec3));

	}
	sVbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}


void CCatmullRom::CreateTrack(string filename)
{
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	CVertexBufferObject vbo;
	vbo.Bind();
	vbo.Create();
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	
	glm::vec2 textCoord(0.0f, 0.0f);
	
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	
	int i = 0;
	//add the offsets of the path
	for (i ; i < m_centrelinePoints.size(); i++) {
		glm::vec3 lPoints = glm::vec3(m_leftOffsetPoints.at(i));
		vbo.AddData(&lPoints, sizeof(glm::vec3));
		textCoord = glm::vec2(0,i);
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
		m_vertexCount += 1;
		glm::vec3 rPoints = glm::vec3(m_rightOffsetPoints.at(i));
		vbo.AddData(&rPoints, sizeof(glm::vec3));
		textCoord = glm::vec2(1, i);
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
		m_vertexCount += 1;
	}

	//add the first two points to close the path.
	vbo.AddData(&m_leftOffsetPoints.at(0), sizeof(glm::vec3));
	textCoord = glm::vec2(0, i);
	vbo.AddData(&textCoord, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));
	m_vertexCount += 1;
	vbo.AddData(&m_rightOffsetPoints.at(0), sizeof(glm::vec3));
	textCoord = glm::vec2(1, i);
	vbo.AddData(&textCoord, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));
	m_vertexCount += 1;
	

	

	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

    

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it

	glBindVertexArray(m_vaoCentreline);

   // glDrawArrays(GL_POINTS,0,m_centrelinePoints.size());
	//glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());

}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	//glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());
	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	//glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
}

void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
 //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_texture.Bind();
	glBindVertexArray(m_vaoTrack);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_leftOffsetPoints.size()+m_rightOffsetPoints.size()+2);
 //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
}

void CCatmullRom::RenderObjectPath()
{
	// Bind the VAO m_vaoLeftObjectCurve and render it
	glBindVertexArray(m_vaoLeftObjectCurve);
	//glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());
//	glDrawArrays(GL_POINTS, 0, m_leftObjectPoints.size());
	// Bind the VAO m_vaoRightOffObjectCurve and render it
	glBindVertexArray(m_vaoRightObjectCurve);
	//glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
	//glDrawArrays(GL_POINTS, 0, m_rightObjectPoints.size());



}

int CCatmullRom::CurrentLap(float d)
{
	return (int)(d / m_distances.back());
}

//Some get methods
vector<glm::vec3> CCatmullRom::GetLeftObjectPoints() 
{
	return m_leftObjectPoints;
}

vector<glm::vec3> CCatmullRom::GetCentrelinePoints()
{
	return m_centrelinePoints;
}

vector<glm::vec3> CCatmullRom::GetRightObjectPoints()
{
	return m_rightObjectPoints;
}