#pragma once
#include "irrLib.h"
#include "TouchEvent.h"
#include "App.h"

class IrrlichtApp : public irr::IEventReceiver
{
private:
	
	// user application
	CApp app;
			
	// irrlicht member values
	irr::IrrlichtDevice* m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;
	irr::core::stringc m_cAbsolutePath;
	bool m_Key[irr::KEY_KEY_CODES_COUNT];

	// irrklang
	irrklang::ISoundEngine* m_pSoundEngine;
	irrklang::ISound* m_pBGMSound;

	// initializing method
	void initDevice(void* windowID);
	
	// rednering method
	void render();
	
	// updating method
	void update(irr::f32 fDelta);
	
	// event receiver - overriding method
	virtual bool OnEvent(const irr::SEvent& event);

	// accel meter
	irr::f32 m_fAccelMeter;
	
	void AccelControl(irr::u32 direction, irr::f32 fDelta);
	
public:
	~IrrlichtApp();
	
	// Irrlicht initializing method
	// param - windowID : IPhone UIWindow or UIView address value (pointer)
	//void initIrrlicht(void* windowID);
	void initIrrlicht();
	
	// Irrlicht loop method - updating & rendering 
	void gameLoop();
	
	// Irrlicht Resize
	void resize(irr::s32 x, irr::s32 y);
	
	// Changing workingDirectory method for IPhone file system
	void setWorkingDirectory(const char* path);
	
	// Single touch event processing method that is called from touch event method in Irrlicht application
	void singleTouchEvent(STouchEvent& touchEvent);

	// Multi touch events processing method that is called from touch event method in Irrlicht application
	void multiTouchEvent(irr::core::array<STouchEvent>& touchEventArrays);
	
	// accelerometer event processing method
	void accelerometerEvent(irr::f32 x, irr::f32 y, irr::f32 z);
	
	// sound event processing method that is called from sound event method in Irrlicht application
	// fileName : test.wav - you must exclude a path (directory name : ex) resource/sound/)
	// number of loops : zero - play one time, nagative number - play infinitely
	// volume :  range - 0.0f ~ 1.0f
	// isBgm : is it a background music?
	void playSound(const char* fileName, irr::f32 volume = 1, bool isBgm = false);

	void playVibration() {}
	
	void preLoadSound(const char* fileName);

	// set a volume of the background music
	void setBgmVolume(float volume);
	
	// stop the background music
	void stopBgm();	
};
