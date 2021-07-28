#include "Core/Math/Mat4.hpp"
#include "Render/D3D11Helper.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "ECS/ComponentGameplay.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Render/Drawers/MiniModelDrawer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

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

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

MiniModelDrawer::MiniModelDrawer()
{
	InitShader();
    AllocateInstance(1);
}

MiniModelDrawer::~MiniModelDrawer()
{
	if (VShader)
		VShader->Release();
	if (PShader)
		PShader->Release();
	if (CBuffer)
		CBuffer->Release();
	if (CamCBuffer)
		CamCBuffer->Release();
	if (ILayout)
		ILayout->Release();
	if (IMatricesBuffer)
        IMatricesBuffer->Release();
    if (IArmyBuffer)
        IArmyBuffer->Release();
}

/*======================= INIT METHODS =======================*/

void MiniModelDrawer::InitShader()
{
	ID3DBlob* blob = nullptr;

	std::string source = (const char*)R"(#line 27
	struct VOut
	{
		float4 position : SV_POSITION;
		float3 color : COLOR;
	};

	cbuffer COLOR_BUFFER : register(b0)
	{
		float4    firstColor;
		float4    secondColor;
	};

	cbuffer CAM_CONSTANT : register(b1)
	{
		float4x4  proj;
		float4x4  view;
	};

	
	VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL, float4x4 model : WORLD, float colorId : COLORID)
	{
		VOut output;
	
		output.position = mul(mul(mul(float4(position,1.0),model),view), proj);

		if (colorId == firstColor.a)
		{
			output.color = firstColor.rgb;
		}
		else if (colorId == secondColor.a)
		{
			output.color = secondColor.rgb;
		}
		else
		{
			output.color = float3(0.5,0.5,0.5);
		}

		return output;
	}
	)";

	Render::CompileVertex(source, &blob, &VShader);

	source = (const char*)R"(#line 93

	float4 main(float4 position : SV_POSITION, float3 color : COLOR) : SV_TARGET
	{
		return float4(color,1.0);
	})";

	Render::CompilePixel(source, &PShader);

	struct Vertex
	{
		Core::Math::Vec3 position;
		Core::Math::Vec2 uv;
		Core::Math::Vec3 normal;
	};

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex,position),  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",       0, DXGI_FORMAT_R32G32_FLOAT,    0,     offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0},

		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, offsetof(Mat4, c[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, offsetof(Mat4, c[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, offsetof(Mat4, c[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, offsetof(Mat4, c[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{ "COLORID",	0, DXGI_FORMAT_R32_FLOAT,			2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	Render::CreateLayout(&blob, ied, 8, &ILayout);

	VS_CONSTANT_BUFFER buffer = {};

	Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &CBuffer);

	CAM_CONSTANT_BUFFER camBuffer = {};

	Render::CreateBuffer(&camBuffer, sizeof(CAM_CONSTANT_BUFFER), &CamCBuffer);

	blob->Release();
}

/*======================= REALTIME METHODS =======================*/

void MiniModelDrawer::AllocateInstance(unsigned int size)
{
	/* Allocating for matrices .... */
    if (IMatricesBuffer)
        IMatricesBuffer->Release();

    IBufferSize = size;

    D3D11_BUFFER_DESC bDesc = {};

    bDesc.ByteWidth			= IBufferSize * sizeof(Mat4);
    bDesc.Usage				= D3D11_USAGE_DYNAMIC;
    bDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
    bDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;


    HRESULT result = Render::RendererRemote::device->CreateBuffer(&bDesc, nullptr, &IMatricesBuffer);

    if (FAILED(result))
    {
        printf("Failed Creating Buffer: %p of size %u: %s \n", IMatricesBuffer, bDesc.ByteWidth, std::system_category().message(result).c_str());
    }

	/* and army indices at the same time */
	if (IArmyBuffer)
		IArmyBuffer->Release();

    bDesc.ByteWidth = IBufferSize * sizeof(float);

    result = Render::RendererRemote::device->CreateBuffer(&bDesc, nullptr, &IArmyBuffer);

    if (FAILED(result))
    {
        printf("Failed Creating Buffer: %p of size %u: %s \n", IArmyBuffer, bDesc.ByteWidth, std::system_category().message(result).c_str());
    }
}


void MiniModelDrawer::Set()
{
	/* set shader */
	Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
	Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

	Render::RendererRemote::context->IASetInputLayout(ILayout);
}

void MiniModelDrawer::Draw(const std::vector<DrawData>& toDraw)
{
	/* we want no texture as we want to display the models depending on
	 * wich army they are in */
    ID3D11ShaderResourceView* fbos[3] = { nullptr, nullptr, nullptr };
    Render::RendererRemote::context->PSSetShaderResources(0, 3, fbos);

    for (int i = 0; i < toDraw.size(); i++)
    {
        const std::vector<Mat4>& mat								= toDraw[i].matrices;
        const std::vector<const ECS::ComponentGameplay*>& gameplays = toDraw[i].gameplays;

        if (mat.size() > IBufferSize)
            AllocateInstance(mat.size());

		/* write the matrices */
        WriteBuffer(mat.data(), sizeof(Mat4) * mat.size(), 0, &IMatricesBuffer);

		/* write the army indices */
        std::vector<float> ids;
        for (int j = 0; j < gameplays.size(); j++)
        {
            ids.push_back(static_cast<float>(gameplays.at(j)->teamName));
        }
        WriteBuffer(ids.data(), sizeof(float) * gameplays.size(), 0, &IArmyBuffer);

		/* set the buffers */
        UINT stride[3]				= { ((2 * sizeof(Core::Math::Vec3)) + sizeof(Core::Math::Vec2)), sizeof(Mat4), sizeof(float) };
        UINT offset[3]				= { 0 , 0, 0 };
        ID3D11Buffer* VBuffers[3]	= { toDraw[i].mesh->VBuffer, IMatricesBuffer, IArmyBuffer };
        RendererRemote::context->IASetIndexBuffer(toDraw[i].mesh->IBuffer, DXGI_FORMAT_R32_UINT, 0);
        RendererRemote::context->IASetVertexBuffers(0, 3, VBuffers, stride, offset);

		/* then draw */
        RendererRemote::context->DrawIndexedInstanced(toDraw[i].mesh->INb, mat.size(), 0, 0, 0);
    }
}