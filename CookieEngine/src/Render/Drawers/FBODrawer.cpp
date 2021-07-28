#include "Render/D3D11Helper.hpp"
#include "Render/FrameBuffer.hpp"
#include "Render/Drawers/FBODrawer.hpp"

using namespace Cookie::Render;

/*===================== CONSTRUCTORS/DESTRUCTORS =====================*/

FBODrawer::FBODrawer()
{
    InitShader();
}

FBODrawer::~FBODrawer()
{
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (PSampler)
        PSampler->Release();
}

/*===================== INIT METHODS =====================*/

void FBODrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 30
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV; 
    };
    
    VOut main(uint vI :SV_VertexId )
    {
        VOut output;
    
        float2 uv = float2((vI << 1) & 2, vI & 2);
        output.uv = float2(uv.x,uv.y);
        output.position = float4(uv.x * 2 - 1, -uv.y * 2 + 1, 0, 1);
    
        return output;

    }
    )";

    Render::CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(#line 52

    Texture2D	diffuseTex2D : register(t0);    

    SamplerState WrapSampler : register(s0);

    float4 main(float4 position : SV_POSITION, float2 uv : UV) : SV_TARGET
    {
        return float4(diffuseTex2D.Sample(WrapSampler,uv).rgb,1.0);        
    })";

    Render::CompilePixel(source, &PShader);

    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.MipLODBias = 0.0f;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = 0;

    Render::CreateSampler(&samDesc, &PSampler);

    blob->Release();
}

/*===================== REALTIME METHODS =====================*/

void FBODrawer::Set()
{
    /* set shader */
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    Render::RendererRemote::context->IASetInputLayout(nullptr);

    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);
}

void FBODrawer::Draw(FrameBuffer& fbo)
{
    Render::RendererRemote::context->OMSetRenderTargets(1, &fbo.renderTargetView, nullptr);
    Render::RendererRemote::context->Draw(3, 0);
}