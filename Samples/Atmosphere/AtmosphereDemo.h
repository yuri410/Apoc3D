#ifndef TERRAINDEMO_H
#define TERRAINDEMO_H

#include "AtmosphereCommon.h"

namespace SampleAtmosphere
{
	class AtmosphereDemo : public Apoc3DEx::Game
	{
	public:
		AtmosphereDemo(RenderWindow* wnd);
		~AtmosphereDemo();

		/** This is the place to load or initialize something before any other 
		 *  events being handled.
		 */
		virtual void Initialize();
		/** This is opposed to Initialize, which is called last(after Unload)
		 *  and is supposed release the resources created in Initialize here.
		 */
		virtual void Finalize();

		/** Load will be called to load contents. 
		 *  Graphics content should be loaded here, if there are any to load as the game starts up.
		 */
		virtual void Load();

		/** Opposite to Load, this is the place to unload content, called once the game window is closed. */
		virtual void Unload();
		
		/** Allows the game to run logic such as updating the world,
		 *  checking for collisions, gathering input, playing audio and etc.
		 */
		virtual void Update(const AppTime* time);

		/** This is called when the game should draw a frame. */
		virtual void Draw(const AppTime* time);

		/** This is called before drawing a new frame. */
		virtual void OnFrameStart();

		/** This is called after a frame is drawn. */
		virtual void OnFrameEnd();

	private:
		static const int NumBlockOnEdge = 16;
		static const int MinBlockCoord = -NumBlockOnEdge/2;
		static const int MaxBlockCoord = NumBlockOnEdge/2-1;

		Sprite* m_sprite;

		FreeCamera* m_camera;



		VertexBuffer* m_quadBuffer;
		VertexDeclaration* m_quadVtxDecl;

		Material* m_quadTempMtrl;
		GeometryData m_quadTempGeoData;

		Texture* m_inscatterTex;
		Texture* m_irradianceTex;
		Texture* m_transmittanceTex;


		Material* m_skyMtrl;
		Model* m_unitBall;
		
		void UpdateCamera(const AppTime* time);

		void DrawQuad(AutomaticEffect* fx);
	};
}
#endif