#include "Audio.h"

#pragma comment(lib, "lib/fmodex_vc.lib")

CAudio::CAudio()
{
	m_pFmodSystem = NULL;
	m_pEventSound = NULL;
	m_pMusic = NULL;
	m_pMusicChannel = NULL;
	m_initialised = false;
}

CAudio::~CAudio()
{}

bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_pFmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_pFmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	m_initialised = true;

	return true;
	
}

// Load an event sound
bool CAudio::LoadEventSound(char *filename)
{
	if (!m_initialised)
		return false;

	result = m_pFmodSystem->createSound(filename, FMOD_HARDWARE, 0, &m_pEventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play an event sound
bool CAudio::PlayEventSound()
{
	if (!m_initialised)
		return false;

	result = m_pFmodSystem->playSound(FMOD_CHANNEL_FREE, m_pEventSound, false, NULL);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(char *filename)
{
	if (!m_initialised)
		return false;

	result = m_pFmodSystem->createStream(filename, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &m_pMusic);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	if (!m_initialised)
		return false;

	result = m_pFmodSystem->playSound(FMOD_CHANNEL_FREE, m_pMusic, false, &m_pMusicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;
	return true;
}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{						
	if (result != FMOD_OK) {
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
		// const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
	}
}

void CAudio::Update()
{
	if (!m_initialised)
		return;

	if (m_pFmodSystem) 
		m_pFmodSystem->update();
}