#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod/fmod.hpp"
#include "./include/fmod/fmod_errors.h"


class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadEventSound(char *filename);
	bool PlayEventSound();
	bool LoadMusicStream(char *filename);
	bool PlayMusicStream();
	void Update();

private:
		

	void FmodErrorCheck(FMOD_RESULT result);


	FMOD_RESULT result;
	FMOD::System* m_pFmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound *m_pEventSound;

	
	FMOD::Sound *m_pMusic;
	FMOD::Channel* m_pMusicChannel;

	boolean m_initialised;

};
