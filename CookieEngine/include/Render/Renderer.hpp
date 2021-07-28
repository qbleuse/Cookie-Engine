#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__


#include "Core/Window.hpp"
#include "Drawers/Skybox.hpp"
#include "Drawers/FBODrawer.hpp"
#include "Render/RendererRemote.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Render/RenderPass/GeometryPass.hpp"
#include "Render/RenderPass/ShadowPass.hpp"
#include "Render/RenderPass/LightPass.hpp"
#include "Render/RenderPass/ComposePass.hpp"
#include "Render/RenderPass/GameplayPass.hpp"
#include "Render/RenderPass/MiniMapPass.hpp"

namespace Cookie
{
	class Game;

	namespace Resources
	{
		class ResourcesManager;
	};

	namespace Render
	{
		class Camera;

		/* Renderer is the big class that will handle the use 
		 * of other smaller render class and contain to have
		 * the wanted rendering result.
		 * The Renderer is a Deffered Renderer. */
		class Renderer
		{
			private:
				IDXGISwapChain*				swapchain		= nullptr;
				ID3D11RenderTargetView*		backbuffer		= nullptr;
					
			public:
				/* the window opened with glfw */
				Core::Window	window;
				/* the static infos that all rendering class need, 
				 * they are initialized at Renderer construction */
				RendererRemote	remote;

				/* the struct to render in a window */
				D3D11_VIEWPORT	viewport;
				D3D11_PRIMITIVE_TOPOLOGY topo{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

				/* the class that will recover the data to draw, 
				 * do occlusion culling and batching */
				DrawDataHandler drawData;
				SkyBox			skyBox;

				/* As it is a deffered Renderer, we have multiple pass,
				 * composition being the last one to draw the map. */
				GeometryPass	geomPass;
				ShadowPass		shadPass;
				LightPass		lightPass;
				ComposePass		compPass;

				/* draw special behavior needed in rts games, such as outlining 
				 * and selection quad. */
				GameplayPass	gamePass;

				/* completly separate from the drawing of game and draws the minimap */
				MiniMapPass		miniMapPass;

			private:
				/* useful class to "copy" a framebuffer to another one */
				Render::FBODrawer	fboDrawer;

			private:
				RendererRemote InitDevice(bool windowed = true);

				/* recreate the backbuffer of swapchain after window is being resized or created */
				bool CreateDrawBuffer(int width, int height);

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Renderer(bool windowed  = true);
				~Renderer();
				
				/* Draw the game from the cam's POV, in the fbo given in parameters */
				void Draw(const Camera* cam, FrameBuffer& fbo);

				/* Draw the game in the fbo from up in orthographic proj taking all the map */
				void DrawMiniMap(FrameBuffer& fbo);

				/* call to swapchain to swap the front and backbuffer */
				void Render()const;

				/* clear all the framebuffers and depth buffers */
				void Clear();

				/* set the back buffer as render target */
				void SetBackBuffer();

				/* draw the framebuffer given in parameter in the framebuffer 
				 * currently bound in rendertarget nb 0*/
				void DrawFrameBuffer(FrameBuffer& fbo);

				/* clear the frameBuffer given in parameter */
				void ClearFrameBuffer(FrameBuffer& fbo);

				/* resize all the framebuffers, usually called when window changes size */
				void ResizeBuffer(int width, int height);
		};
	}
}

#endif /*__RENDERER_HPP__*/