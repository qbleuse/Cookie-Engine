#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "ShadowBuffer.hpp"
#include "Game.hpp"
#include "Resources/Scene.hpp"
#include "Render/Renderer.hpp"
#include "ImGui/imgui.h"


using namespace Cookie::Render;
using namespace Cookie::Core::Math;

/*========================= CONSTRUCTORS/DESTRUCTORS ===========================*/

Renderer::Renderer(bool windowed):
    /* It is where dx11 is enabled for the first time */
    remote {InitDevice(windowed)},
    /* The viewport is not 1 in z to be sure the skybox is not clipped by depth buffer when drawn */
    viewport {0.0f,0.0f,static_cast<float>(window.width),static_cast<float>(window.height),0.0f,0.9999999f},
    /* The pass that have framebuffers inside them need dimension when created */
    geomPass{window.width,window.height},
    lightPass{window.width,window.height}
{
    /* create the backbuffer */
    CreateDrawBuffer(window.width,window.height);

    /* Set for the first time the viewport and topology */
    remote.context->RSSetViewports(1, &viewport);
    remote.context->IASetPrimitiveTopology(topo);

    /* the drawData is responsible to draw the models ans the map 
     * so it should have the depthbuffer and the constant buffer of the camera
     * to be able to Draw */
    drawData.depthStencilView   = geomPass.depthBuffer;
    drawData.CamCBuffer         = geomPass.CBuffer;
}

Renderer::~Renderer()
{
    /* clear all settings */
    remote.context->ClearState();

    /* Release all ref we have on dx11 objects */
    if (swapchain)
        swapchain->Release();
    if (backbuffer)
        backbuffer->Release();
    if (remote.context)
    {
        remote.context->Release();
    }
    if (remote.device)
        remote.device->Release();

    /* Technically, it frees memory only here, by calling that */
    remote.context->Flush();
}

/*========================= INIT METHODS =========================*/

RendererRemote Renderer::InitDevice(bool windowed)
{
    RendererRemote _remote;

    DXGI_SWAP_CHAIN_DESC scd = {};

    /* fill the swap chain description struct */

    /* one back buffer */
    scd.BufferCount         = 1;                 
    /* use 32-bit color */
    scd.BufferDesc.Format   = DXGI_FORMAT_R8G8B8A8_UNORM;   
    /* how swap chain is to be used (we want to render in different framebuffer) */
    scd.BufferUsage         = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    /* the window to be used */
    scd.OutputWindow        = glfwGetWin32Window(window.window);   
    /* actually we do not use this one but specify to be nothing to be clearer */
    scd.BufferDesc.Scaling  = DXGI_MODE_SCALING_UNSPECIFIED;
    /* same for this one */
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    /* how many multisamples (so here we don not have one, also because we are in deffered renderer) */
    scd.SampleDesc.Count    = 1;
    /* ... so we don't use this one */
    scd.SampleDesc.Quality  = 0;
    /* depends on init */
    scd.Windowed            = windowed;                                    
    /* basically, when we swap the precedent front buffer is cleared */
    scd.SwapEffect          = DXGI_SWAP_EFFECT_DISCARD;

    /* create a device, device context and swap chain using the information in the scd struct*/
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        /* for debug purposes */
#if 1
        D3D11_CREATE_DEVICE_DEBUG,
#else
        NULL,
#endif
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &_remote.device,
        NULL,
        &_remote.context);

    if (FAILED(result))
    {
        printf("Failed Creating Device: %s\n", std::system_category().message(result).c_str());
        return _remote;
    }

    if (!windowed)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        window.width    = mode->width;
        window.height   = mode->height;
    }

    return _remote;
}

bool Renderer::CreateDrawBuffer(int width, int height)
{
    /* get the address of the back buffer, usually created with the swapchain */
    ID3D11Texture2D* pBackBuffer = nullptr;

    if (FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
        return false;

    /* use the back buffer address to create the render target */
    if (FAILED(remote.device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer)))
        return false;

    pBackBuffer->Release();

    return true;
}

/*========================= CALLBACK METHODS =========================*/

void Renderer::ResizeBuffer(int width, int height)
{
    /* remove all settings to be carefull */
    remote.context->ClearState();

    /* realease all buffer taht needs to be realeased before */
    geomPass.depthBuffer->Release();
    backbuffer->Release();


    /* checking if we are going inyo full screen are not */
    if (glfwGetWindowMonitor(window.window) != nullptr)
    {
        /* get resolution of monitor*/
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        /* switch to full screen*/
        glfwSetWindowMonitor(window.window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);

        window.width    = mode->width;
        window.height   = mode->height;
        width   = mode->width;
        height  = mode->height;
    }

    /* leaving 0 will let the driver do the work of finding the size for us 
     * (without having to specify resolution and other stuff) */
    HRESULT result = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

    if (FAILED(result))
    {
        printf("%s", (std::string("Failing Resizing SwapChain Buffer : ") + std::system_category().message(result)).c_str());
    }

    /* recreate the buffers with the right size */
    geomPass.CreateDepth(width,height);
    drawData.depthStencilView = geomPass.depthBuffer;
    CreateDrawBuffer(width,height);

    remote.context->OMSetRenderTargets(1, &backbuffer, nullptr);

    /* resize all the framebuffers */
    geomPass.posFBO.Resize(width,height);
    geomPass.normalFBO.Resize(width, height);
    geomPass.albedoFBO.Resize(width, height);
    lightPass.diffuseFBO.Resize(width, height);
    lightPass.specularFBO.Resize(width, height);

    /* we call to free all the things that have been destroyed at this point and 
     * release the old framebuffer that can be huge depending 
     * on the size of the window and the resolution*/
    remote.context->Flush();

    /* change the viewport be at the size of the window */
    viewport.Width  = width;
    viewport.Height = height;

    remote.context->RSSetViewports(1, &viewport);
    remote.context->IASetPrimitiveTopology(topo);
}

/*========================= RENDER METHODS =========================*/

void Renderer::Draw(const Camera* cam, FrameBuffer& framebuffer)
{
    /* create this one to clear up the views when we don't need more than one */
    ID3D11RenderTargetView* nullViews[] = { nullptr,nullptr,nullptr,nullptr };
    /* set viewport as it may have changed in precedent pass (for example minimap) */
    Render::RendererRemote::context->RSSetViewports(1, &viewport);

    /* fill the draw datas to be drawn later on */
    drawData.SetDrawData(cam);

    /* fill the geometry */
    geomPass.Set();
    geomPass.Draw(drawData);

    remote.context->OMSetRenderTargets(4, nullViews, nullptr);

    /* draw the shadow in depth buffer */
    shadPass.Set();
    shadPass.Draw(drawData);
    remote.context->RSSetViewports(1, &viewport);

    /* compute lighting with shadow */
    lightPass.Set(geomPass.posFBO, geomPass.normalFBO, geomPass.albedoFBO);
    lightPass.Draw(shadPass.shadowMap, drawData);

    /* do tone mapping */
    remote.context->OMSetRenderTargets(1, &framebuffer.renderTargetView, nullptr);
    compPass.Set(lightPass.diffuseFBO,lightPass.specularFBO,geomPass.albedoFBO);
    compPass.Draw();

    /* add skybox */
    remote.context->OMSetRenderTargets(1, &framebuffer.renderTargetView, geomPass.depthBuffer);
    skyBox.Draw(cam->GetProj(), cam->GetView());

    /* kind of post process, add game's specific render objects */
    gamePass.Set();
    gamePass.Draw(drawData);
}

void Renderer::DrawMiniMap(FrameBuffer& fbo)
{
    /* in terms of state, they have actually a lot in common so 
     * use the already existent to not create new ones */
    geomPass.Set();

    miniMapPass.Draw(drawData,fbo);

    remote.context->ClearDepthStencilView(miniMapPass.depthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::DrawFrameBuffer(FrameBuffer& fbo)
{
    fboDrawer.Set();
    remote.context->PSSetShaderResources(0, 1, &fbo.shaderResource);
    /* we don't draw a quad but a triangle, 
     * we exploit the default behavior of 
     * having no vertex buffer of dx11*/
    remote.context->Draw(3, 0);
}

void Renderer::Clear()
{
    /* clear all currently bound render target (I do not use over 4)*/
    ID3D11RenderTargetView* nullViews[] = { nullptr,nullptr,nullptr,nullptr };
    remote.context->OMSetRenderTargets(4, nullViews, nullptr);

    /* clear all bound textures (I do not use over 3) */
    ID3D11ShaderResourceView* resources[3] = { nullptr,nullptr,nullptr };
    Render::RendererRemote::context->PSSetShaderResources(0, 3, resources);
    Core::Math::Vec4 clearColor = {0.0f,0.0f,0.0f,1.0f};

    /* clear the depth buffers */
    remote.context->ClearRenderTargetView(backbuffer, clearColor.e);
    remote.context->ClearDepthStencilView(geomPass.depthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    shadPass.Clear();

    /* clear the framebuffers */
    geomPass.Clear();
    lightPass.Clear();
    drawData.Clear();
}

void Renderer::ClearFrameBuffer(FrameBuffer& fbo)
{
    Core::Math::Vec4 clearColor = { 0.0f,0.0f,0.0f,1.0f };
    remote.context->ClearRenderTargetView(fbo.renderTargetView, clearColor.e);
}

void Renderer::SetBackBuffer()
{
    remote.context->OMSetRenderTargets(1, &backbuffer, nullptr);
}

void Renderer::Render()const
{

    remote.context->OMSetRenderTargets(1, &backbuffer, nullptr);
    /* switch the back buffer and the front buffer,
     * vsync is enabled by the 1 */
    swapchain->Present(1, 0);
}
