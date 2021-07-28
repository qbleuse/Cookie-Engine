#include "Core/Math/Mat4.hpp"
#include "Core/Primitives.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Render/DebugRenderer.hpp"


using namespace Cookie::Render;
using namespace Cookie::Resources;
using namespace Cookie::Core;
using namespace Cookie::Core::Math;

/*========== CONSTRUCTORS/DESTRUCTORS ==========*/

DebugRenderer::DebugRenderer():
    physDbgRenderer{InitEdit()}
{

    AllocateVBuffer(1);
    InitRasterizerState();
    InitShader();
    physDbgRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    physDbgRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
    physDbgRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
    physDbgRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    physDbgRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
}

DebugRenderer::~DebugRenderer()
{
    if (VBuffer)
        VBuffer->Release();
    if (rasterState)
        rasterState->Release();
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (CBuffer)
        CBuffer->Release();
    if (ILayout)
        ILayout->Release();
}


/*========== INIT METHODS ==========*/

rp3d::DebugRenderer& DebugRenderer::InitEdit()
{
    Physics::PhysicsHandle::editWorld = Physics::PhysicsHandle::physCom->createPhysicsWorld();
    return Physics::PhysicsHandle::editWorld->getDebugRenderer();
}

void DebugRenderer::AllocateVBuffer(size_t vBufferSize)
{
    bDesc.ByteWidth             = vBufferSize * sizeof(float);
    bDesc.Usage                 = D3D11_USAGE_DYNAMIC;
    bDesc.BindFlags             = D3D11_BIND_VERTEX_BUFFER;
    bDesc.CPUAccessFlags        = D3D11_CPU_ACCESS_WRITE;
    bDesc.MiscFlags             = 0;
    bDesc.StructureByteStride   = 0;

    if (FAILED(Render::RendererRemote::device->CreateBuffer(&bDesc, nullptr, &VBuffer)))
    {
        printf("Failed Creating Buffer: %p of size %llu \n", VBuffer, vBufferSize * sizeof(float));
    }
}

void DebugRenderer::InitRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable    = false;
    rasterDesc.CullMode                 = D3D11_CULL_FRONT;
    rasterDesc.DepthBias                = 0;
    rasterDesc.DepthBiasClamp           = 0.0f;
    rasterDesc.DepthClipEnable          = true;
    rasterDesc.FillMode                 = D3D11_FILL_WIREFRAME;
    rasterDesc.FrontCounterClockwise    = false;
    rasterDesc.MultisampleEnable        = false;
    rasterDesc.ScissorEnable            = false;
    rasterDesc.SlopeScaledDepthBias     = 0.0f;

    Render::RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterState);;
}

void DebugRenderer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 76
    struct VOut
    {
        float4 position : SV_POSITION;
        uint  color : COLOR;
    };

    cbuffer VS_CONSTANT_BUFFER : register(b0)
    {
        float4x4  viewProj;
    };
    
    VOut main(float3 position : POSITION, uint color : COLOR)
    {
        VOut output;
    
        output.position = mul(float4(position,1.0), viewProj);
        output.color = color;
    
        return output;

    }
    )";

    Render::CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(#line 102
    float4 main(float4 position : SV_POSITION, uint color : COLOR) : SV_TARGET
    {
        float3 finalColor;
        finalColor.r = floor(color / 65536);
        finalColor.g = floor((color - finalColor.r * 65536) / 256.0);
        finalColor.b = floor(color - finalColor.r * 65536 - finalColor.g * 256.0);
        return float4(finalColor,1.0);
    })";

    Render::CompilePixel(source,&PShader);


    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(rp3d::DebugRenderer::DebugTriangle,point1), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32_UINT, 0,  offsetof(rp3d::DebugRenderer::DebugTriangle,color1), D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    Render::CreateLayout(&blob,ied,2,&ILayout);

    Render::CreateBuffer(Core::Math::Mat4::Identity().e,sizeof(Core::Math::Mat4),&CBuffer);

    blob->Release();
}

/*========== RUNTIME METHODS ==========*/

void DebugRenderer::UpdateVBuffer(size_t vBufferSize, void* data)
{
    D3D11_MAPPED_SUBRESOURCE ms;

    if (FAILED(Render::RendererRemote::context->Map(VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
    {
        printf("Failed to update Buffer nb %u: %p\n", 0, VBuffer);
        return;
    }

    memcpy(ms.pData, data, vBufferSize);

    Render::RendererRemote::context->Unmap(VBuffer, 0);
}

void DebugRenderer::AddDebugElement(const std::vector<Primitives::DebugVertex>& dbgElement)
{
    for (int i = 0; i < dbgElement.size(); i++)
    {
        debugElement.push_back(dbgElement.at(i));
    }
}

void DebugRenderer::AddLine(Vec3 a, Vec3 b, uint32_t color)
{
    AddDebugElement(Primitives::CreateLine(a, b, color, color));
}
void DebugRenderer::AddQuad(Vec3 a, Vec3 c, uint32_t color)
{
    Vec3 b = {c.x, a.y, a.z};
    Vec3 d = {a.x, a.y, c.z};

    AddDebugElement(Primitives::CreateLine(a, b, color, color));
    AddDebugElement(Primitives::CreateLine(b, c, color, color));
    AddDebugElement(Primitives::CreateLine(c, d, color, color));
    AddDebugElement(Primitives::CreateLine(d, a, color, color));
}
void DebugRenderer::AddQuad(Vec3 center, float halfWidth, float halfDepth, uint32_t color)
{
    Vec3 a = center + Vec3{-halfWidth, 0, -halfDepth};
    Vec3 b = center + Vec3{halfWidth,  0, -halfDepth};
    Vec3 c = center + Vec3{halfWidth,  0,  halfDepth};
    Vec3 d = center + Vec3{-halfWidth, 0,  halfDepth};

    AddDebugElement(Primitives::CreateLine(a, b, color, color));
    AddDebugElement(Primitives::CreateLine(b, c, color, color));
    AddDebugElement(Primitives::CreateLine(c, d, color, color));
    AddDebugElement(Primitives::CreateLine(d, a, color, color));
}

void DebugRenderer::Draw(const Mat4& viewProj)
{
    if (showDebug)
    {
        /* Set D3D11 Context To draw debug primitives*/
        UINT stride = sizeof(Core::Math::Vec3) + sizeof(uint32_t);
        UINT offset = 0;

        /* Shader */
        Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
        Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

        Render::RendererRemote::context->IASetInputLayout(ILayout);
        Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &CBuffer);

        Render::WriteBuffer(viewProj.e,sizeof(viewProj),0,&CBuffer);

        /* save previous State to put it back at the end*/
        D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        Render::RendererRemote::context->IAGetPrimitiveTopology(&topo);

        ID3D11RasterizerState* previousState = nullptr;
        Render::RendererRemote::context->RSGetState(&previousState);
        Render::RendererRemote::context->RSSetState(rasterState);

        /* ask rp3d to compute the primitives for us */
        physDbgRenderer.reset();
        physDbgRenderer.computeDebugRenderingPrimitives(*Physics::PhysicsHandle::physSim.worldSim);

        /* draw triangles first */
        if (physDbgRenderer.getNbTriangles() > 0)
        {
            /* set to draw triangle */
            Render::RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            /* We use a single vertex buffer for all primitives so we reallocate if we don't have enough space */
            if (bDesc.ByteWidth < (physDbgRenderer.getNbTriangles() * sizeof(rp3d::DebugRenderer::DebugTriangle)))
            {
                VBuffer->Release();
                AllocateVBuffer((physDbgRenderer.getNbTriangles() * sizeof(rp3d::DebugRenderer::DebugTriangle)));
            }
            /* put the vertices inside the vertex buffer */
            UpdateVBuffer((physDbgRenderer.getNbTriangles() * sizeof(rp3d::DebugRenderer::DebugTriangle)), (void*)physDbgRenderer.getTrianglesArray());

            /* set and draw */
            Render::RendererRemote::context->IASetVertexBuffers(0, 1, &VBuffer, &stride, &offset);
            Render::RendererRemote::context->Draw(physDbgRenderer.getNbTriangles() * 3, 0);
        }

        /* draw lines */
        if (physDbgRenderer.getNbLines() > 0)
        {
            /* set to draw lines */
            Render::RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            /* same as for the triangles above */
            if (bDesc.ByteWidth < (physDbgRenderer.getNbLines() * sizeof(rp3d::DebugRenderer::DebugLine)))
            {
                VBuffer->Release();
                AllocateVBuffer((physDbgRenderer.getNbLines() * sizeof(rp3d::DebugRenderer::DebugLine)));
            }

            UpdateVBuffer((physDbgRenderer.getNbLines() * sizeof(rp3d::DebugRenderer::DebugLine)), (void*)physDbgRenderer.getLinesArray());

            /* set and draw */
            Render::RendererRemote::context->IASetVertexBuffers(0, 1, &VBuffer, &stride, &offset);
            Render::RendererRemote::context->Draw(physDbgRenderer.getNbLines() * 2, 0);
        }

        /* that is for our debug element such as raycast */
        if (debugElement.size() > 0)
        {
            /* we only draw wireframe in debug */

            Render::RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            if (bDesc.ByteWidth < (debugElement.size() * sizeof(Core::Primitives::DebugVertex)))
            {
                VBuffer->Release();
                AllocateVBuffer((debugElement.size() * sizeof(Core::Primitives::DebugVertex)));
            }

            UpdateVBuffer(debugElement.size() * sizeof(Core::Primitives::DebugVertex), (void*)debugElement.data());

            Render::RendererRemote::context->IASetVertexBuffers(0, 1, &VBuffer, &stride, &offset);
            Render::RendererRemote::context->Draw(debugElement.size(), 0);

            /* clear the buffer if it had something inside */
            debugElement.clear();
        }

        /* put back previous context */
        Render::RendererRemote::context->IASetPrimitiveTopology(topo);
        Render::RendererRemote::context->RSSetState(previousState);

        previousState->Release();
    }
}