#include "Core/Math/Mat4.hpp"
#include "D3D11Helper.hpp"
#include "DrawDataHandler.hpp"
#include "Render/RenderPass/GeometryPass.hpp"
#include "Render/Camera.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

struct VS_CONSTANT_BUFFER
{
    Mat4 proj = Mat4::Identity();
    Mat4 view = Mat4::Identity();
};

/*=========================== CONSTRUCTORS/DESTRUCTORS ===========================*/

GeometryPass::GeometryPass(int width, int height):
    /* albedo is 32bits, the other needs more precision so we put all the way up */
	posFBO		{ width,height,DXGI_FORMAT_R32G32B32A32_FLOAT},
	normalFBO	{ width,height,DXGI_FORMAT_R32G32B32A32_FLOAT},
	albedoFBO	{ width,height }
{
	InitShader();
    InitState();
    CreateDepth(width, height);
}

GeometryPass::~GeometryPass()
{
    if (depthBuffer)
        depthBuffer->Release();
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (CBuffer)
        CBuffer->Release();
    if (ILayout)
        ILayout->Release();
    if (PSampler)
        PSampler->Release();
    if (depthStencilState)
        depthStencilState->Release();
    if (rasterizerState)
        rasterizerState->Release();
    if (blendState)
        blendState->Release();
}

/*=========================== INIT METHODS ===========================*/

void GeometryPass::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 27
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV; 
        float3 normal : NORMAL;
        float3 fragPos : FRAGPOS;
        float4 view    : VIEW;
    };

    cbuffer CAM_CONSTANT : register(b0)
    {
        float4x4  proj;
        float4x4  view;
    };

    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL, float4x4 model : WORLD)
    {
        VOut output;
    
        output.fragPos  = mul(float4(position,1.0),model).xyz;
        output.view     = mul(float4(output.fragPos,1.0),view);
        output.position = mul(output.view, proj);
        output.uv       = uv;
        output.normal   = normalize(mul(normal,(float3x3)model));

    
        return output;

    }
    )";

    Render::CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(#line 93

    Texture2D	albedoTex           : register(t0);
    Texture2D	normalTex           : register(t1); 
    Texture2D	metallicRoughness   : register(t2); 

    SamplerState WrapSampler : register(s0);

    /* this is being took from this link: http://www.thetenthplanet.de/archives/1180 
     * and traduced to hlsl */
    float3x3 cotangent_frame(float3 normal, float3 p, float2 uv)
    {
        // get edge vectors of the pixel triangle
        float3 dp1 = ddx( p );
        float3 dp2 = ddy( p );
        float2 duv1 = ddx( uv );
        float2 duv2 = ddy( uv );
     
        // solve the linear system
        float3 dp2perp = cross( dp2, normal );
        float3 dp1perp = cross( normal, dp1 );
        float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
        float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
     
        // construct a scale-invariant frame 
        float invmax = rsqrt( max( dot(T,T), dot(B,B) ) );
        return float3x3( T * invmax, B * invmax, normal );
    }

    float3 perturb_normal(float3 normal, float3 viewEye, float2 uv, float3 texNormal)
    {
        // assume N, the interpolated vertex normal and 
        // V, the view vector (vertex to eye)
        texNormal = (texNormal * 2.0 - 1.0);
        float3x3 TBN = cotangent_frame(normal, viewEye, uv);
        return normalize(mul(texNormal,TBN));
    }

    struct POut
    {
        float4 position : SV_TARGET0;
        float4 normal : SV_TARGET1; 
        float4 albedo : SV_TARGET2;
    };

    POut main(float4 position : SV_POSITION, float2 uv : UV, float3 normal : NORMAL, float3 fragPos : FRAGPOS, float4 view : VIEW) 
    {
        POut pOutput;

        float3 texNormal        = normalTex.Sample(WrapSampler,uv).xyz;
        float3 viewEye          = normalize(view.xyz);
        float3 perturbNormal    = perturb_normal(normal,viewEye,uv,texNormal);

        /* we put the material info in the alpha component of all the framebuffers 
         * b = metallic, g = roughness, r = ambient occlusion. */
        pOutput.position    = float4(fragPos,metallicRoughness.Sample(WrapSampler,uv).b);
        pOutput.normal      = lerp(float4(normal,metallicRoughness.Sample(WrapSampler,uv).g),float4(perturbNormal,metallicRoughness.Sample(WrapSampler,uv).g),step(0.1,dot(texNormal,texNormal)));
        pOutput.albedo      = float4(albedoTex.Sample(WrapSampler,uv).rgb,metallicRoughness.Sample(WrapSampler,uv).r);

        return pOutput;
    })";

    Render::CompilePixel(source, &PShader);

    struct Vertex
    {
        Core::Math::Vec3 position;
        Core::Math::Vec2 uv;
        Core::Math::Vec3 normal;
    };

    /* create the input layout object */
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex,position),  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV",       0, DXGI_FORMAT_R32G32_FLOAT,    0,     offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0},

        { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(Mat4, c[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(Mat4, c[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(Mat4, c[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(Mat4, c[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    Render::CreateLayout(&blob, ied, 7, &ILayout);

    /* creating constant buffer */
    VS_CONSTANT_BUFFER buffer = {};
    Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &CBuffer);

    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy   = 1;
    /* a sampler can be a comparison sampler in dx11,  
     * this is not what we want*/
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;

    Render::CreateSampler(&samDesc, &PSampler);

    blob->Release();
}

void GeometryPass::InitState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* the depth test is enabled */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.StencilEnable  = false;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* we are culling the back face of the meshes */
    rasterDesc.CullMode                 = D3D11_CULL_BACK;
    rasterDesc.DepthClipEnable          = true;
    rasterDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise    = true;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    /* we create one and set it because the blend state can change and we do not want any alpha */
    D3D11_BLEND_DESC blenDesc = {  };
    blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    RendererRemote::device->CreateBlendState(&blenDesc, &blendState);
}

void GeometryPass::CreateDepth(int width, int height)
{
    ID3D11Texture2D* depthTexture = nullptr;

    D3D11_TEXTURE2D_DESC depthBufferDesc = {};

    /* Set up the description of the depth buffer texture, it does not have any use as is */
    depthBufferDesc.Width       = width;
    depthBufferDesc.Height      = height;
    depthBufferDesc.MipLevels   = 1;
    depthBufferDesc.ArraySize   = 1;
    /* 24 bits of depth, 1 byte of stencil */
    depthBufferDesc.Format              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    /* no multi-sampling */
    depthBufferDesc.SampleDesc.Count    = 1;
    depthBufferDesc.SampleDesc.Quality  = 0;
    /* and creating as depth Stencil  */
    depthBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL;

    HRESULT result = RendererRemote::device->CreateTexture2D(&depthBufferDesc, NULL, &depthTexture);
    if (FAILED(result))
    {
        printf("Failing Creating Texture %p: %s\n", depthTexture, std::system_category().message(result).c_str());
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

    /* Set up the depth stencil view description, which is what we will use */
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

void GeometryPass::Set()
{
    /* Now set the rasterizer state. */
    Render::RendererRemote::context->RSSetState(rasterizerState);
    /* Set the depth stencil state. */
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
    /* Set the Blend State. */
    const float blendFactor[4] = { 1.0f,1.0f,1.0f,0.0f };
    Render::RendererRemote::context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    /* setup shader */
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);

}

void GeometryPass::Draw(DrawDataHandler& drawData)
{
    const Camera& cam = *drawData.currentCam;
    
    /* setup the framebuffers */
	ID3D11RenderTargetView* fbos[3] = {posFBO.renderTargetView,normalFBO.renderTargetView,albedoFBO.renderTargetView};
	Render::RendererRemote::context->OMSetRenderTargets(3, fbos, depthBuffer);

    /* setup the cam constant buffers */
    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &CBuffer);
    VS_CONSTANT_BUFFER buffer = { cam.GetProj(), cam.GetView() };
    Render::WriteBuffer(&buffer,sizeof(buffer),0,&CBuffer);

    /* then draw the models */
    Render::RendererRemote::context->IASetInputLayout(ILayout);
    drawData.Draw();

    /* setup the cam constant buffers in the 
     * right place for the shader of the map */
    ID3D11Buffer* CBuffers[2] = { nullptr, CBuffer };
    Render::RendererRemote::context->VSSetConstantBuffers(0, 2, CBuffers);

    /* then drawing the map */
    drawData.mapDrawer.Draw();

    /* setting the layout up because next pass use the same */
    Render::RendererRemote::context->IASetInputLayout(ILayout);
}

void GeometryPass::Clear()
{
	float black[4] = { 0.0f,0.0f,0.0f,0.0f };

	Render::RendererRemote::context->ClearRenderTargetView(posFBO.renderTargetView,		black);
	Render::RendererRemote::context->ClearRenderTargetView(normalFBO.renderTargetView,  black);
	Render::RendererRemote::context->ClearRenderTargetView(albedoFBO.renderTargetView,	black);
}