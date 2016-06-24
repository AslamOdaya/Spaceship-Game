#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class PPyramid;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();
	

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CShaderProgram *m_pShaderProgram;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh *m_pFighterMesh;
	COpenAssetImportMesh *m_pHealthPack;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;
	CCube *m_pCube;
	PPyramid *m_pPyramid;
	glm::mat4 *m_pModelMatrix;
	glm::mat4 *m_pViewMatrix;
	glm::mat4 *m_pProjectionMatrix;


	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	bool m_hasRespawned;
	bool m_isGameOver;
	float m_currentDistance;
	float m_rotateObject;
	float m_strafeX;
	float m_strafeZ;
	int m_health;
	int m_points;
	int m_currentLap;

	string m_currentObjects;

	vector<glm::vec3> m_spherePointLocation;
	vector<glm::vec3> m_cubePointLocation;
	vector<glm::vec3> m_pyramidPointLocation;
	vector<glm::vec3> m_healthpackPointLocation;
	vector<string> m_objectNames;
	

	//list of bool vectors that store collisions
	vector<bool> m_activeSphere;
	vector<bool> m_activeCube;
	vector<bool> m_activePyramid;
	vector<bool> m_activeHealthPack;
	glm::vec3 m_spaceShipPosition;
	glm::mat4 m_spaceShipOrientation;



public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
	void DisplayHUD();
	void RespawnObjects();
	void ShakeCam();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;


};
