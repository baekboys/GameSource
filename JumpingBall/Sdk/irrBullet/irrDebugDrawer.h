#ifndef _DEBUGDRAW_H 
#define _DEBUGDRAW_H 

namespace irr_bullet {
	namespace debug
	{
		enum DebugDrawModes 
		{ 
			DBG_NoDebug=0, 
			DBG_DrawWireframe = 1, 
			DBG_DrawAabb=2, 
			DBG_DrawFeaturesText=4, 
			DBG_DrawContactPoints=8, 
			DBG_NoDeactivation=16, 
			DBG_NoHelpText = 32, 
			DBG_DrawText=64, 
			DBG_ProfileTimings = 128, 
			DBG_EnableSatComparison = 256, 
			DBG_DisableBulletLCP = 512, 
			DBG_EnableCCD = 1024, 
			DBG_MAX_DEBUG_DRAW_MODE 
		}; 

		//Courtesy of randomMESH 
		class DebugDraw : public btIDebugDraw 
		{ 

		public: 

			DebugDraw(IrrlichtDevice* const device) : 
			  mode(DBG_NoDebug), driver(device->getVideoDriver()), logger(device->getLogger()) 
			  { 

			  } 

			  void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) 
			  { 
				  irr::video::SColor newColor(255, (u32)color[0], (u32)color[1], (u32)color[2]); 
				  if (color[0] <= 1.0 && color[0] > 0.0) 
					  newColor.setRed((u32)(color[0]*255.0)); 
				  if (color[1] <= 1.0 && color[1] > 0.0) 
					  newColor.setGreen((u32)(color[1]*255.0)); 
				  if (color[2] <= 1.0 && color[2] > 0.0) 
					  newColor.setBlue((u32)(color[2]*255.0)); 

				  this->driver->draw3DLine(irr::core::vector3df(from[0], from[1], from[2]), irr::core::vector3df(to[0], to[1], to[2]), newColor); 
			  } 

			  void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) 
			  { 
				  static const   irr::video::SColor CONTACTPOINT_COLOR(255, 255, 255, 0); 
				  const btVector3 to(PointOnB + normalOnB*distance); 
				  this->driver->draw3DLine(irr::core::vector3df(PointOnB[0], PointOnB[1], PointOnB[2]),irr::core::vector3df(to[0], to[1], to[2]),CONTACTPOINT_COLOR); 
			  } 

			  void reportErrorWarning(const char* text) 
			  { 
				  this->logger->log(text, irr::ELL_ERROR); 
			  } 

			  void draw3dText(const btVector3& location, const char* text) { } 
			  void setDebugMode(int mode) { this->mode = mode; } 
			  int getDebugMode() const { return this->mode; } 

		private: 

			int mode; 
			irr::video::IVideoDriver* const driver; 
			ILogger* logger; 
		}; 

	}
}

#endif 