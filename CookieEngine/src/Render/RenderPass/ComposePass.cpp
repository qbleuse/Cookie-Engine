#include "D3D11Helper.hpp"
#include "FrameBuffer.hpp"
#include "RenderPass/ComposePass.hpp"

using namespace Cookie::Render;

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

ComposePass::ComposePass()
{
    InitShader();
    InitState();
}

ComposePass::~ComposePass()
{
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (PSampler)
        PSampler->Release();
    if (depthStencilState)
        depthStencilState->Release();
    if (rasterizerState)
        rasterizerState->Release();
    if (blendState)
        blendState->Release();
}

/*======================= INIT METHODS =======================*/

void ComposePass::InitShader()
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
    
        float2 uv       = float2((vI << 1) & 2, vI & 2);
        output.uv       = uv;
        output.position = float4(uv.x * 2 - 1, -uv.y * 2 + 1, 0, 1);
    
        return output;
    }
    )";

    CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(#line 58

    Texture2D	diffuseTex  : register(t0);    
    Texture2D	specularTex : register(t1);  
    Texture2D	albedoTex   : register(t2);  

    SamplerState ClampSampler : register(s0);

    float4 main(float4 position : SV_POSITION, float2 uv : UV) : SV_TARGET0
    {
        float3  finalColor    = diffuseTex.Sample(ClampSampler,uv).xyz + specularTex.Sample(ClampSampler,uv).xyz;
        finalColor *= pow(albedoTex.Sample(ClampSampler,uv).rgb,2.2);
        
        float3 denominator = (finalColor + 1.0);

        float3 mapped = finalColor/denominator;

        return float4(pow(mapped,0.454545),1.0);
    })";

    CompilePixel(source, &PShader);

    /* sampler to sample the diffuse, specular and albedo */
    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.MaxAnisotropy   = 1;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;

    Render::CreateSampler(&samDesc, &PSampler);

    blob->Release();
}

void ComposePass::InitState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* we are force to use it because skybox may rewrite it otherwise */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable  = false;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* cull back because do not need front */
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FillMode = D3D11_FILL_SOLID;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    D3D11_BLEND_DESC blenDesc = {  };

    /* normal blending to remove additive blending of light pass  */
    blenDesc.RenderTarget[0].RenderTargetWriteMask  = D3D11_COLOR_WRITE_ENABLE_ALL;

    RendererRemote::device->CreateBlendState(&blenDesc, &blendState);
}

/*======================= REALTIME METHODS =======================*/


void ComposePass::Set(FrameBuffer& diffuse, FrameBuffer& specular, FrameBuffer& albedo)
{
    // Now set the rasterizer state.
    Render::RendererRemote::context->RSSetState(rasterizerState);
    // Set the depth stencil state.
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
    // Set the Blend State.
    const float blendFactor[4] = { 1.0f,1.0f,1.0f,0.0f };
    Render::RendererRemote::context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    /* set shader */
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    /* exploit dx11 behavior*/
    Render::RendererRemote::context->IASetInputLayout(nullptr);
    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);

    /* set texture */
    ID3D11ShaderResourceView* fbos[3] = {diffuse.shaderResource, specular.shaderResource, albedo.shaderResource };
    Render::RendererRemote::context->PSSetShaderResources(0, 3, fbos);
}

void ComposePass::Draw()
{
    /* we don't draw a quad but a triangle,
     * we exploit the default behavior of
     * having no vertex buffer of dx11 */
    Render::RendererRemote::context->Draw(3, 0);
}