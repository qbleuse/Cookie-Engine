#include "D3D11Helper.hpp"
#include "Render/RendererRemote.hpp"
#include "Render/ShadowBuffer.hpp"

using namespace Cookie::Render;

ShadowBuffer::ShadowBuffer()
{
    if (CreateTexture())
    {
        CreateShaderResource();
        CreateDepthStencilView();
    }
}

ShadowBuffer::~ShadowBuffer()
{
    if (texBuffer)
    {
        texBuffer->Release();
        texBuffer = nullptr;
    }
    if (shaderResource)
    {
        shaderResource->Release();
        shaderResource = nullptr;
    }
    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = nullptr;
    }
}

bool ShadowBuffer::CreateTexture()
{
    D3D11_TEXTURE2D_DESC desc = {};

    desc.Width = SHADOW_MAP_RESOLUTION;
    desc.Height = SHADOW_MAP_RESOLUTION;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;

    HRESULT result = Render::RendererRemote::device->CreateTexture2D(&desc, nullptr, &texBuffer);
    if (FAILED(result))
    {
        printf("Failing Creating ShadowBuffer Texture: %s\n", std::system_category().message(result).c_str());
        return false;
    }


    return true;
}

bool ShadowBuffer::CreateShaderResource()
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format                      = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels         = 1;
    srvDesc.Texture2D.MostDetailedMip   = 0;

    HRESULT result = Render::RendererRemote::device->CreateShaderResourceView(texBuffer, &srvDesc, &shaderResource);
    if (FAILED(result))
    {
        printf("Failing Creating ShadowBuffer ShaderResource: %s\n", std::system_category().message(result).c_str());
        return false;
    }

    return true;
}

bool ShadowBuffer::CreateDepthStencilView()
{
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};

    desc.Format             = DXGI_FORMAT_D32_FLOAT;
    desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;

    HRESULT result = Render::RendererRemote::device->CreateDepthStencilView(texBuffer, &desc, &depthStencilView);
    if (FAILED(result))
    {
        printf("Failing Creating ShadowBuffer ShaderResource: %s\n", std::system_category().message(result).c_str());
        return false;
    }

    return true;
}

void ShadowBuffer::Clear()
{
    Render::RendererRemote::context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
}