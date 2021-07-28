#ifndef __RENDERER_REMOTE_HPP__
#define __RENDERER_REMOTE_HPP__

#include <d3d11.h>

namespace Cookie
{
	namespace Render
	{


		struct RendererRemote
		{
				static ID3D11Device*		device;
				static ID3D11DeviceContext*	context;


			/* CONSTRUCTORS/DESTRUCTORS */
				RendererRemote();
				RendererRemote(const RendererRemote& remote);
				~RendererRemote();
		};
	}
}

#endif /* __RENDERER_REMOTE_HPP__ */