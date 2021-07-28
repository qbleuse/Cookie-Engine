#include "Render/RendererRemote.hpp"
#include "Resources/Mesh.hpp"

#include <vector>

using namespace Cookie::Resources;

Mesh::Mesh(std::string _name, aiMesh* mesh):
    name{_name}
{
    AABBMin = { mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z };
    AABBMax = { mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z };

    INb     = mesh->mNumFaces * 3;//a face is a triangle as triangulate flag is enabled
    InitVBuffer(mesh);
    InitIBuffer(mesh);
}

#undef min
#undef max

Mesh::Mesh(std::string meshName, std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int inb):
    AABBMin{std::numeric_limits<float>().max(),std::numeric_limits<float>().max(),std::numeric_limits<float>().max() },
    AABBMax{-std::numeric_limits<float>().max(),-std::numeric_limits<float>().max(),-std::numeric_limits<float>().max() },
    name{ meshName }
{
    INb = inb;
    ComputeAABB(vertices);

    InitVBuffer(vertices);
    InitIBuffer(indices);
}

Mesh::~Mesh()
{
    if (VBuffer)
    {
        VBuffer->Release();
    }
    if (IBuffer)
    {
        IBuffer->Release();
    }
}

void Mesh::ComputeAABB(const std::vector<float>& vertices)
{
    for (int i = 0; i < vertices.size(); i+=8)//position, uv, normal
    {

        if (vertices[i] > AABBMax.x)
        {
            AABBMax.x = vertices[i];
        }
        if (vertices[i] < AABBMin.x)
        {
            AABBMin.x = vertices[i];
        }
        if (vertices[i+1] > AABBMax.y)
        {
            AABBMax.y = vertices[i+1];
        }
        if (vertices[i + 1] < AABBMin.y)
        {
            AABBMin.y = vertices[i + 1];
        }
        if (vertices[i+2] > AABBMax.z)
        {
            AABBMax.z = vertices[i+2];
        }
        if (vertices[i + 2] < AABBMin.z)
        {
            AABBMin.z = vertices[i + 2];
        }
    }
}

void Mesh::InitVBuffer(aiMesh* mesh)
{
    D3D11_BUFFER_DESC bDesc = {};

    std::vector<float> vertices;

    /* find the max vertex size */
    float xLength = AABBMax.x - AABBMin.x;
    float yLength = AABBMax.y - AABBMin.y;
    float zLength = AABBMax.z - AABBMin.z;

    float max = std::max(xLength, std::max(yLength, zLength));

    AABBMax /= max;
    AABBMin /= max;

    /* push vertices and normalize them in a box between -0.5 and 0.5 */
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x/max);
        vertices.push_back(mesh->mVertices[i].y/max);
        vertices.push_back(mesh->mVertices[i].z/max);
        vertices.push_back(mesh->mTextureCoords[0][i].x);
        vertices.push_back(mesh->mTextureCoords[0][i].y);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);
    }

    bDesc.ByteWidth             = vertices.size() * sizeof(float);
    bDesc.Usage                 = D3D11_USAGE_DEFAULT;
    bDesc.BindFlags             = D3D11_BIND_VERTEX_BUFFER;
    bDesc.CPUAccessFlags        = 0;
    bDesc.MiscFlags             = 0;
    bDesc.StructureByteStride   = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem            = vertices.data();
    InitData.SysMemPitch        = 0;
    InitData.SysMemSlicePitch   = 0;

    if (FAILED(Render::RendererRemote::device->CreateBuffer(&bDesc, &InitData, &VBuffer)))
    {
        printf("Failed Creating Buffer: %p of size %llu \n", VBuffer, sizeof(InitData.pSysMem));
    }
}

void Mesh::InitIBuffer(aiMesh* mesh)
{
    D3D11_BUFFER_DESC bDesc = {};

    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace iFace = mesh->mFaces[i];
        for (unsigned int j = 0; j < iFace.mNumIndices; j++)
        {
            indices.push_back(iFace.mIndices[j]);
        }
    }

    bDesc.ByteWidth             = indices.size() * sizeof(unsigned int);
    bDesc.Usage                 = D3D11_USAGE_DEFAULT;
    bDesc.BindFlags             = D3D11_BIND_INDEX_BUFFER;
    bDesc.CPUAccessFlags        = 0;
    bDesc.MiscFlags             = 0;
    bDesc.StructureByteStride   = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = indices.data();
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    if (FAILED(Render::RendererRemote::device->CreateBuffer(&bDesc, &InitData, &IBuffer)))
    {
        printf("Failed Creating Buffer: %p of size %llu \n", IBuffer, sizeof(InitData.pSysMem));
    }
}

void Mesh::InitVBuffer(std::vector<float>& vertices)
{
    D3D11_BUFFER_DESC bDesc = {};

    bDesc.ByteWidth = vertices.size() * sizeof(float);
    bDesc.Usage     = D3D11_USAGE_DEFAULT;
    bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bDesc.CPUAccessFlags = 0;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;


    /* find the max vertex size */
    float xLength = AABBMax.x - AABBMin.x;
    float yLength = AABBMax.y - AABBMin.y;
    float zLength = AABBMax.z - AABBMin.z;

    float max = std::max(xLength, std::max(yLength, zLength));

    AABBMax /= max;
    AABBMin /= max;

    /* push vertices and normalize them in a box between -0.5 and 0.5 */
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = vertices[i] / max;
    }

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices.data();
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    if (FAILED(Render::RendererRemote::device->CreateBuffer(&bDesc, &InitData, &VBuffer)))
    {
        printf("Failed Creating Buffer: %p of size %llu \n", VBuffer, sizeof(InitData.pSysMem));
    }
}

void Mesh::InitIBuffer(std::vector<unsigned int>& indices)
{
    D3D11_BUFFER_DESC bDesc = {};

    bDesc.ByteWidth = indices.size() * sizeof(unsigned int);
    bDesc.Usage = D3D11_USAGE_DEFAULT;
    bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bDesc.CPUAccessFlags = 0;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = indices.data();
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    if (FAILED(Render::RendererRemote::device->CreateBuffer(&bDesc, &InitData, &IBuffer)))
    {
        printf("Failed Creating Buffer: %p of size %llu \n", IBuffer, sizeof(InitData.pSysMem));
    }
}

void Mesh::Set()const
{
    UINT stride = ((2 * sizeof(Core::Math::Vec3)) + sizeof(Core::Math::Vec2));
    UINT offset = 0;
    Render::RendererRemote::context->IASetVertexBuffers(0,1,&VBuffer,&stride,&offset);
    Render::RendererRemote::context->IASetIndexBuffer(IBuffer, DXGI_FORMAT_R32_UINT,0);
}

void Mesh::Draw()const
{
    Render::RendererRemote::context->DrawIndexed(INb, 0, 0);
}