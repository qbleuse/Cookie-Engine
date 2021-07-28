#ifndef __D3D11_HELPER_HPP__
#define __D3D11_HELPER_HPP__

#include "RendererRemote.hpp"
#include <system_error>
#include <d3dcompiler.h>

#ifdef SHADER_DEBUG
    #define SHADER_FLAG (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION)
#else
    #define SHADER_FLAG 0
#endif

namespace Cookie
{
	namespace Render
	{
		inline bool CompileVertex(std::string VSSource, ID3DBlob** VS, ID3D11VertexShader** VShader)
		{
            ID3DBlob* VSErr = nullptr;

            if (FAILED(D3DCompile(VSSource.c_str(), VSSource.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", SHADER_FLAG, 0, VS, &VSErr)))
            {
                printf("Failed To Compile Vertex Shader %s\n", (const char*)(VSErr->GetBufferPointer()));
                VSErr->Release();
                return false;
            }



            HRESULT result = Render::RendererRemote::device->CreateVertexShader((*VS)->GetBufferPointer(), (*VS)->GetBufferSize(), NULL, VShader);
            if (FAILED(result))
            {
                printf("Failed Creating Vertex Shader: %s\n", std::system_category().message(result).c_str());
                VSErr->Release();
                return false;
            }

            return true;
		}

        inline bool CompilePixel(std::string PSSource, ID3D11PixelShader** PShader)
        {
            ID3DBlob* PS = nullptr;
            ID3DBlob* PSErr = nullptr;

            if (FAILED(D3DCompile(PSSource.c_str(), PSSource.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", SHADER_FLAG, 0, &PS, &PSErr)))
            {
                printf("Failed To Compile Pixel Shader: %s\n", (const char*)(PSErr->GetBufferPointer()));
                PSErr->Release();
                return false;
            }

            HRESULT result = Render::RendererRemote::device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, PShader);
            if (FAILED(result))
            {
                printf("Failed Creating Pixel Shader: %s\n", std::system_category().message(result).c_str());
                PSErr->Release();
                return false;
            }

            PS->Release();

            return true;
        }

        inline bool CreateLayout(ID3DBlob** VS, D3D11_INPUT_ELEMENT_DESC* desc, unsigned int inputNb,ID3D11InputLayout** layoutPtr)
        {
            HRESULT result = Render::RendererRemote::device->CreateInputLayout(desc, inputNb, (*VS)->GetBufferPointer(), (*VS)->GetBufferSize(), layoutPtr);

            if (FAILED(result))
            {

                printf("Failed to create input layout: %s \n", std::system_category().message(result).c_str());
                return false;
            }

            return true;
        }

        inline bool CreateBuffer(void* bufferData, size_t bufferSize, ID3D11Buffer** bufferPtr)
        {
            D3D11_BUFFER_DESC bDesc = {};

            bDesc.ByteWidth             = bufferSize;
            bDesc.Usage                 = D3D11_USAGE_DYNAMIC;
            bDesc.BindFlags             = D3D11_BIND_CONSTANT_BUFFER;
            bDesc.CPUAccessFlags        = D3D11_CPU_ACCESS_WRITE;
            bDesc.MiscFlags             = 0;
            bDesc.StructureByteStride   = 0;

            D3D11_SUBRESOURCE_DATA InitData;
            InitData.pSysMem            = bufferData;
            InitData.SysMemPitch        = 0;
            InitData.SysMemSlicePitch   = 0;

            HRESULT result = Render::RendererRemote::device->CreateBuffer(&bDesc, &InitData, bufferPtr);

            if (FAILED(result))
            {
                printf("Failed Creating Buffer: %p of size %llu: %s \n", *bufferPtr, bufferSize, std::system_category().message(result).c_str());
                return false;
            }

            return true;
        }

        inline bool CreateSampler(D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** PSampler)
        {
            HRESULT result = Render::RendererRemote::device->CreateSamplerState(desc, PSampler);

            if (FAILED(result))
            {
                printf("Failed Creating sampler state %p : %s",*PSampler, std::system_category().message(result).c_str());
                return false;
            }

            return true;
        }

        inline bool WriteBuffer(const void* data, size_t bufferSize, UINT CBufferIndex, ID3D11Buffer** CBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE ms;

            HRESULT result = Render::RendererRemote::context->Map(*CBuffer, CBufferIndex, D3D11_MAP_WRITE_DISCARD, 0, &ms);

            if (FAILED(result))
            {
                printf("Failed to update Buffer nb %u: %p - %s\n", CBufferIndex, CBuffer, std::system_category().message(result).c_str());
                return false;
            }

            memcpy(ms.pData, data, bufferSize);

            Render::RendererRemote::context->Unmap(*CBuffer, CBufferIndex);

            return true;
        }


	}
}

#endif // !__D3D11_HELPER_HPP__