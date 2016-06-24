/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 4.0a 30/01/2016
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"
#include <iostream>
using namespace std;


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Cube.h"
#include "Pyramid.h"


// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pShaderProgram = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_pFighterMesh = NULL;
	m_pCube = NULL;
	m_pPyramid = NULL;
	m_pHealthPack = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	m_rotateObject = 0.0;
	m_spaceShipPosition = glm::vec3(0, 0, 0);
	m_spaceShipOrientation = glm::mat4(0);
	m_health = 100;
	m_points = 0;
	m_currentLap = 0;
	m_isGameOver = false;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_pFighterMesh;
	delete m_pCube;
	delete m_pPyramid;
	delete m_pHealthPack;
	delete m_pShaderProgram;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pShaderProgram = new CShaderProgram;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pHealthPack = new COpenAssetImportMesh;
	m_pFighterMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_pCube = new CCube;
	m_pPyramid = new PPyramid;
	m_pModelMatrix = new glm::mat4(1);
	m_pViewMatrix = new glm::mat4(1);
	m_pProjectionMatrix = new glm::mat4(1);
	
	
	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);


	// You can follow this pattern to load additional shaders




	// Create the skybox
	// Skybox downloaded from   http://www.custommapmakers.org/skyboxes.php
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "mars_texture.jpg", 2000.0f, 2000.0f, 100.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pFighterMesh->Load("resources\\models\\Fighter\\fighter1.obj"); // Downloaded from http://www.psionicgames.com/?page_id=24 on March 15th 2016
	m_pHealthPack->Load("resources\\models\\Healthpack\\healthpack.3ds");// Downloaded from http://www.turbosquid.com/3d-models/free-health-pack-3d-model/514293 on 23th March 2016
	
																		   
	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "green_texture.jpg", 25, 25);  // Texture downloaded from http://www.pageresource.com/wallpapers/3974/texture-green-wall-hd-wallpaper.html on 19 March 2016
	//create cube
	m_pCube->Create("resources\\textures\\red_texture.jpg"); //texture downloaded from http://www.pageresource.com/wallpapers/4190/muffet-red-texture-textures-geprek-hd-wallpaper.htmlon 15 March 2016
	//create pyramid
	m_pPyramid->Create("resources\\textures\\yellow_texture.jpg"); //texture downloaded from http://muffet1.deviantart.com/art/Smoky-Glow-161302234 on 19 March 2016
	
																   
    //glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	//m_pAudio->Initialise();
	//m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	//m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateOjectPath();
	m_pCatmullRom->CreateTrack("resources\\textures\\space_floor.jpg"); ////texture downloaded from http://thumbs.dreamstime.com/t/texture-silver-metal-platform-floor-background-close-up-54526246.jpg 17 March 2016

																		//store the location of each object.
	vector<glm::vec3> spherePoints = m_pCatmullRom->GetRightObjectPoints();
	vector<glm::vec3> cubePoints = m_pCatmullRom->GetLeftObjectPoints();
	vector<glm::vec3> centrePoints = m_pCatmullRom->GetCentrelinePoints();

	//store locations for spheres
	for (int i = 1; i < spherePoints.size(); i++) {
		if (i % 50 == 0) {

			m_spherePointLocation.push_back(spherePoints.at(i));
			m_activeSphere.push_back(true);
		}
	}

	//store locations for cube
	for (int i = 1; i < cubePoints.size(); i++) {
		if (i % 50 == 0) {
			m_cubePointLocation.push_back(cubePoints.at(i));
			m_activeCube.push_back(true);
		}
	}

	//store locations for pyramid
	for (int i = 1; i < centrePoints.size(); i++) {
		if (i % 50 == 0) {
			m_pyramidPointLocation.push_back(centrePoints.at(i));
			m_activePyramid.push_back(true);
		}
	}

	//store locations for health pack.
	for (int i = 0; i < centrePoints.size(); i++) {
		if (i % 250 == 0) {
			m_healthpackPointLocation.push_back(centrePoints.at(i + 36));
			m_activeHealthPack.push_back(true);
		}
	
	}


		
	m_objectNames.push_back("Pyramid");
	m_objectNames.push_back("Cube");
	m_objectNames.push_back("Sphere");
	srand(time(0));
	m_currentObjects = m_objectNames[(rand() % 3)];
	
}

// Render method runs repeatedly in a loop
void Game::Render()
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("CubeMapTex", 1);


	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render();
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();




	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	



	

	//Render spaceship
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0, 5, 0));
	modelViewMatrixStack.Translate(m_spaceShipPosition.x, m_spaceShipPosition.y, m_spaceShipPosition.z);
	modelViewMatrixStack *= m_spaceShipOrientation;
	modelViewMatrixStack.Scale(0.3);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pFighterMesh->Render();
	modelViewMatrixStack.Pop();




	//render centreline and the two offsets.
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture",true); // turn on texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderCentreline();
	m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderTrack();
	m_pCatmullRom->RenderObjectPath();
	modelViewMatrixStack.Pop();

	//Render Health pack
	
	for (int i = 0; i < m_healthpackPointLocation.size(); i++) {
		if (m_activeHealthPack[i] != false) {
			modelViewMatrixStack.Push();
			//put it at a random place.
			modelViewMatrixStack.Translate(m_healthpackPointLocation.at(i).x, m_healthpackPointLocation.at(i).y + 5.5f, m_healthpackPointLocation.at(i).z);
			modelViewMatrixStack.Scale(0.5f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pHealthPack->Render();
			modelViewMatrixStack.Pop();
		}

	}


	
	//Render the sphere
	for (int i = 0; i < m_spherePointLocation.size(); i++){
		//if there is no collision.
			if (m_activeSphere[i] != false) {
				modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(m_spherePointLocation.at(i).x, m_spherePointLocation.at(i).y + 3.5f, m_spherePointLocation.at(i).z);
				modelViewMatrixStack.Rotate(glm::vec3(1, 1, 0), m_rotateObject);
				modelViewMatrixStack.Scale(2.0f);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				pMainProgram->SetUniform("bUseTexture", true);
				m_pSphere->Render();
				modelViewMatrixStack.Pop();
			}
		}

	


	//render cube;
	for (int i = 0; i < m_cubePointLocation.size(); i++) {
		//if collision is not detected.
		if (m_activeCube[i] != false) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(m_cubePointLocation.at(i).x, m_cubePointLocation.at(i).y + 3.5f, m_cubePointLocation.at(i).z);
			modelViewMatrixStack.Rotate(glm::vec3(1, 1, 0), m_rotateObject);
			modelViewMatrixStack.Scale(2.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			pMainProgram->SetUniform("bUseTexture", true);
			m_pCube->Render();
			modelViewMatrixStack.Pop();
		}
	

	}

	//render the pyramid.
	for (int i = 0; i < m_pyramidPointLocation.size(); i++) {
		if(m_activePyramid[i] != false){
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(m_pyramidPointLocation.at(i).x, m_pyramidPointLocation.at(i).y + 3.5f, m_pyramidPointLocation.at(i).z);
			modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_rotateObject);
			modelViewMatrixStack.Scale(2.0f);
			pMainProgram->SetUniform("bUseTexture", true); // turn on texturing
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix",
				m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pPyramid->Render();
			modelViewMatrixStack.Pop();
			}
		
	}
	
	

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();
	DisplayHUD();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}




// Update method runs repeatedly with the Render method
void Game::Update()
{
	m_currentDistance += m_dt * 0.1f;
	m_pCamera->Update(m_dt);

	//m_pAudio->Update();

	glm::vec3 p;
	glm::vec3 pNext;
	glm::vec3 pLeft;

	bool isSphereCollected = false;

	//Get the left points, right and centreline points and store it in local vector variable
	vector<glm::vec3> leftPoints = m_pCatmullRom->GetLeftObjectPoints();
	vector<glm::vec3> rightPoints = m_pCatmullRom->GetRightObjectPoints();
	vector<glm::vec3> centrelinePoints = m_pCatmullRom->GetCentrelinePoints();
	////default position
	
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1, pNext);

	if (!m_isGameOver) {
		//move spaceship left when left key is pressed
		if (GetKeyState(VK_LEFT) & 0x80) {
			m_pCatmullRom->SampleSides(m_currentDistance, p, leftPoints);
			m_pCatmullRom->SampleSides(m_currentDistance + 1, pNext, leftPoints);
		}
		//move spaceship right when right key is pressed.
		else if (GetKeyState(VK_RIGHT) & 0x80) {
			m_pCatmullRom->SampleSides(m_currentDistance, p, rightPoints);
			m_pCatmullRom->SampleSides(m_currentDistance + 1, pNext, rightPoints);
		}

		else {
			//default position in the middle.
			m_pCatmullRom->Sample(m_currentDistance, p);
			m_pCatmullRom->Sample(m_currentDistance + 1, pNext);
		}


	}


	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));


	glm::vec3 pathX;
	glm::vec3 pathY;
	glm::vec3 pathZ;

	//default third person view
	pathX = glm::vec3((p - 30.0f*T + (20.0f*B)));
	pathY = glm::vec3(p + 40.0f*T);
	pathZ = glm::vec3(0, 1, 0);

	if (!m_isGameOver) {
		//change to first person when button is pressed.
		if (GetKeyState('C') & 1) {
			//set the camera along the path -- FIRST PERSON
			pathX = glm::vec3(p + glm::vec3(0, 10, 0));
			pathY = glm::vec3(p + 30.0f*T);
			pathZ = glm::vec3(0, 1, 0);
		}
	}


	//OTHER CAMERA VIEWS
	
	//change to top down view
	if (m_isGameOver) {
	
			//Top view
			pathX = glm::vec3((p - 1.0f*T + (200.0f*B)));
			pathY = glm::vec3(p + 2.0f*T);
			pathZ = glm::vec3(0, 1, 0);
			////change to right side  view
			//	//Side View
			//pathX = glm::vec3((p + 10.0f*T + 15.0f*B + 180.0f*N));
			//pathY = glm::vec3(p + 10.0f*T + 15.0f*B + 70.0f*N);
			//pathZ = glm::vec3(0, 1, 0);


			//////change to left side view
			//	//Side View
			//pathX = glm::vec3((p + 10.0f*T + 15.0f*B - 180.0f*N));
			//pathY = glm::vec3(p + 10.0f*T + 15.0f*B + 70.0f*N);
			//pathZ = glm::vec3(0, 1, 0);
	}
	//comment to enable free view mode.
	m_pCamera->Set(pathX, pathY, pathZ); // COMMENT TO ENABLE FREE VIEW CAMER A 


	//set the spaceship orientation and make it travel along the path
	m_spaceShipOrientation = glm::mat4(glm::mat3(T, B, N));

	//set the location of the spaceship
	m_spaceShipPosition = p;


	//rotate primtive objects
	m_rotateObject += 0.2f * m_dt;



	//keep track of lap
	m_currentLap = m_pCatmullRom->CurrentLap(m_currentDistance);
	int speed = 0.12f;



	//last point of each line on the path
	int centreIndex = m_pCatmullRom->GetCentrelinePoints().size() - 1;
	int leftIndex = m_pCatmullRom->GetLeftObjectPoints().size() - 1;
	int rightIndex = m_pCatmullRom->GetRightObjectPoints().size() - 1;

	glm::vec3 centreLastPoint = m_pCatmullRom->GetCentrelinePoints()[centreIndex];
	glm::vec3 leftLastPoint = m_pCatmullRom->GetLeftObjectPoints()[leftIndex];
	glm::vec3 rightLastPoint = m_pCatmullRom->GetRightObjectPoints()[rightIndex];

	//respawn objects at new lap and make spaceship go faster
	if (glm::length( centreLastPoint - p) < 5.0f || (glm::length(leftLastPoint - p)   < 5.0f || (glm::length(leftLastPoint - p) < 5.0f)))
	{
		RespawnObjects();
	
	}
	
	for (int i = 1; i < 100; i++) {
		if (m_currentLap == i) {
			m_currentDistance += m_dt * 0.06f;
	}
	}
		




	


		//check collision detection for sphere 
	for (int i = 0; i < m_spherePointLocation.size(); i++) {
		if (glm::length(m_spherePointLocation.at(i) - p) < 5.0f) {
			if (m_activeSphere[i] && m_currentObjects == "Sphere") {
				m_activeSphere[i] = false;  //do not render object
				m_points++; //increase points
				//select random object name from vector
				m_currentObjects = m_objectNames[(rand() % 3)];
			}
			//take away health if wrong object is collected.
			if (m_activeSphere[i] && m_currentObjects != "Sphere") 
			{
				m_health--;
			}
		}

	}


	
		//check collision for cube
		for (int i = 0; i < m_cubePointLocation.size(); i++) {
			if (glm::length(m_cubePointLocation.at(i) - p) < 5.0f) {
				//check if object has been rendered and matches name with shape
				if(m_activeCube[i] && m_currentObjects == "Cube") {
					m_activeCube[i] = false;  //do not render object
					m_points++; //increase points
					//select random object name from vector
					m_currentObjects = m_objectNames[(rand() % 3)];
				}
				//take away health if wrong object is collected.
				if (m_activeCube[i] && m_currentObjects != "Cube")
				{
					m_health--;
				}
			}
		}

		//check collision for pyramid

		//check if object has been rendered and matches name with shape

		for (int i = 0; i < m_pyramidPointLocation.size(); i++) {
			if (glm::length(m_pyramidPointLocation.at(i) - p) < 5.0f) {
				if (m_activePyramid[i] && m_currentObjects == "Pyramid") {
					m_activePyramid[i] = false; //do not render object
					m_points++; 
					//select random object name from vector
					m_currentObjects = m_objectNames[(rand() % 3)];
				}
				//take away health if wrong object is collected.
				if (m_activePyramid[i] && m_currentObjects != "Pyramid")
				{
					m_health--;
				}
			}
		}

		
		//collision detection and interactions with healthpack
		for (int i = 0; i < m_healthpackPointLocation.size(); i++) {
			if (glm::length(m_healthpackPointLocation.at(i) - p) < 5.0f) {
				if (m_activeHealthPack[i] && m_health < 100) {
					m_activeHealthPack[i] = false;
					if (m_health > 100) {
						m_health = 100;
					}
					else {
						m_health+=10;
					}
				}
			}
		}


		if (m_health <= 0) {
			for (int i = 0; i < m_healthpackPointLocation.size(); i++) {
				m_activeHealthPack[i] = false;
			}
			m_health = 0;
			m_currentLap = 0;
		}
	
		
		
		
		
}


void Game::RespawnObjects() {

	for (int i = 0; i < m_spherePointLocation.size(); i++) {
		m_activeSphere[i] = true;
	}

	for (int i = 0; i < m_cubePointLocation.size(); i++) {
		m_activeCube[i] = true;
	}

	for (int i = 0; i < m_pyramidPointLocation.size(); i++) {
		m_activePyramid[i] = true;
	}
}




//Displays 2D HUD 
void Game::DisplayHUD(){

	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	char * message;
	//convert string to char *
	message = new char[m_currentObjects.size() + 1];
	memcpy(message,m_currentObjects.c_str(), m_currentObjects.size()+1);

	//render the texts
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	m_pFtFont->Render(10, 5, 15, "Use C to change camera views");
	m_pFtFont->Render(600, height - 20, 15, "Pick up: " );
	m_pFtFont->Render(650, height - 20, 15, message);
	m_pFtFont->Render(600, height - 40, 15, "Health: %d", m_health);
	m_pFtFont->Render(600, height - 60, 15, "Points: %d", m_points);
	m_pFtFont->Render(600, height - 80, 15, "Lap: %d", m_currentLap);
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = 0x09;
	ip.ki.dwFlags = 0;
	if (m_health <= 0) {
		m_pFtFont->Render(260, height - 200, 40, "GAME OVER!");
		m_pFtFont->Render(260, height - 300, 30, "TOTAL POINTS: %d", m_points);
		m_pFtFont->Render(260, height - 400, 30, "LAPS COMPLETED: %d", m_currentLap);
		m_isGameOver = true;

	}
	//release resources
	delete[] message;
}


void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	


	}

	

}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;

		//case VK_UP:
		//	m_strafe -= 0.5f;
		//	break;
		//case VK_DOWN:
		//	m_strafe += 0.5f;
		//	break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
