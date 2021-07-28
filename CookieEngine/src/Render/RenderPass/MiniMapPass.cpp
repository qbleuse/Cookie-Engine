#include "Core/Math/Mat4.hpp"
#include "D3D11Helper.hpp"
#include "DrawDataHandler.hpp"
#include "ECS/ComponentGameplay.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "FrameBuffer.hpp"
#include "Render/RenderPass/MiniMapPass.hpp"
#include "Resources/Scene.hpp"
#include "Render/Camera.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

constexpr float zEpsilon = 50.0f;

struct CAM_CONSTANT_BUFFER
{
	Mat4 proj = Mat4::Identity();
	Mat4 view = Mat4::Identity();
};

struct VS_CONSTANT_BUFFER
{
	Vec4 color1 = { PLAYER_ARMY_COLOR,(float)Cookie::Gameplay::E_ARMY_NAME::E_PLAYER };
	Vec4 color2 = { AI1_ARMY_COLOR,(float)Cookie::Gameplay::E_ARMY_NAME::E_AI1 };
};


/*=========================== CONSTRUCTORS/DESTRUCTORS ===========================*/

MiniMapPass::MiniMapPass():
    viewport{0.0f,0.0f,0.0f,0.0f,0.0f,1.0f}
{
	miniMapView = Mat4::Translate({0.0f,-zEpsilon,0.0f}) * Mat4::RotateX(ToRadians(90.0f));

	InitState();
}

MiniMapPass::~MiniMapPass()
{
	if (depthStencilState)
	{
		depthStencilState->Release();
	}
	if (rasterState)
	{
		rasterState->Release();
	}
    if (depthBuffer)
    {
        depthBuffer->Release();
    }
}

/*=========================== INIT METHODS ===========================*/

void MiniMapPass::InitState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* enable depth test */
    depthStencilDesc.DepthEnable	= true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable	= false;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* enable cull back */
    rasterDesc.CullMode                 = D3D11_CULL_BACK;
    rasterDesc.DepthClipEnable          = true;
    rasterDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise    = true;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterState);
}


void MiniMapPass::CreateDepth(int width, int height)
{
    ID3D11Texture2D* depthTexture = nullptr;

    D3D11_TEXTURE2D_DESC depthBufferDesc = {};

    // Set up the description of the depth buffer.
    depthBufferDesc.Width               = width;
    depthBufferDesc.Height              = height;
    depthBufferDesc.MipLevels           = 1;
    depthBufferDesc.ArraySize           = 1;
    depthBufferDesc.Format              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count    = 1;
    depthBufferDesc.SampleDesc.Quality  = 0;
    depthBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL;

    HRESULT result = RendererRemote::device->CreateTexture2D(&depthBufferDesc, NULL, &depthTexture);
    if (FAILED(result))
    {
        printf("Failing Creating Texture %p: %s\n", depthTexture, std::system_category().message(result).c_str());
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = RendererRemote::device->CreateDepthStencilView(depthTexture, &depthStencilViewDesc, &depthBuffer);
    if (FAILED(result))
    {
        printf("Failing Creating depth Buffer %p: %s\n", depthTexture, std::system_category().message(result).c_str());
        return;
    }

    depthTexture->Release();
}

/*=========================== REALTIME METHODS ===========================*/

void MiniMapPass::Draw(DrawDataHandler& drawData, const FrameBuffer& framebuffer)
{
	RendererRemote::context->OMSetRenderTargets(1, &framebuffer.renderTargetView, depthBuffer);

	const Camera& cam			= *drawData.currentCam;
	const Resources::Map& map	= drawData.currentScene->map;

    /* create an orthographic projection */
	float width     = map.trs.scale.x;
	float height    = map.trs.scale.z;
	ortho = Mat4::Ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, -zEpsilon, zEpsilon);

    /* set viewport */
	viewport.Width  = framebuffer.width;
	viewport.Height = framebuffer.height;
	Render::RendererRemote::context->RSSetViewports(1, &viewport);

    /* set constant buffers */
	ID3D11Buffer* CBuffers[2] = { miniModelDrawer.CBuffer, miniModelDrawer.CamCBuffer };
	Render::RendererRemote::context->VSSetConstantBuffers(0, 2, CBuffers);

    /* set cam constants */
	CAM_CONSTANT_BUFFER buffer = { ortho, miniMapView };
	Render::WriteBuffer(&buffer, sizeof(buffer), 0, &miniModelDrawer.CamCBuffer);

    /* draw the models */
	miniModelDrawer.Set();
	miniModelDrawer.Draw(drawData.staticDrawData);
	miniModelDrawer.Draw(drawData.dynamicDrawData);

	// Now set the rasterizer state.
	Render::RendererRemote::context->RSSetState(rasterState);
	// Set the depth stencil state.
	Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);

    /* draw the map */
	miniMapDrawer.Set(cam,map);
	miniMapDrawer.Draw();
}