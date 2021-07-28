#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "Core/Math/Mat4.hpp"
#include <memory>
#include <array>

namespace Cookie
{
	namespace Render
	{
		/*================== SHADER INCLUDE ==================*/

		constexpr const char* blinnPhong = R"(
		static const float PI = 3.14159265359;

		struct POut
		{
		    float4 diffuse  : SV_TARGET0;
		    float4 specular : SV_TARGET1; 
		};

		cbuffer Cam : register(b0)
		{
		    float3 camPos;
			float2 screenSize;
		};
		
		float	compute_diffuse		(float3 lightDir, float3 normal)
		{
			float diff 			= max(dot(normal,lightDir),0.0);
		
			return diff;
		}
		
		float	compute_specular	(float3 lightDir, float3 normal, float3 position, float shininess)
		{
			float3 viewDir		= normalize(camPos - position);
			float3 halfAngleVec	= normalize(lightDir + viewDir);

			float scale = (shininess+2)/8;
		
			float spec			= scale*pow(max(dot(normal,halfAngleVec),0.0f),shininess);
		
			return  spec;
		}

		POut compute_lighting(float3 normal, float3 position, float3 lightDir, float3 lightColor, float metallic, float roughness)
		{
			POut output;

			float a			= roughness * roughness;
			float shininess = (2/max(a*a,0.0000000001)) - 1;
			output.specular	= float4(compute_specular(lightDir,normal,position,shininess) * lightColor,1.0);
			output.diffuse	= float4((compute_diffuse(lightDir,normal)) * lightColor,1.0);

			return output;
		}

		)";

		constexpr const char* PBR = R"(#line 48
		static const float PI = 3.14159265359;
		static const float3 basic = float3(1.0,1.0,1.0); 		

		struct POut
		{
		    float4 diffuse  : SV_TARGET0;
		    float4 specular : SV_TARGET1; 
		};

		cbuffer Cam : register(b0)
		{
		    float3	camPos;
			float2	screenSize;
			float	farPlane;
		};
		
		float DistributionGGX(float3 normal, float3 halfAngleVec, float roughness)
		{
			float a	 = roughness*roughness;
			float a2 = a*a;
			float NdotH  = max(dot(normal, halfAngleVec), 0.0);
			float NdotH2 = NdotH*NdotH;

			float denom = (NdotH2 * (a2 - 1.0) + 1.0);
			denom = PI * denom * denom;

			return a2/max(denom,0.00000001);
		}

		float GeometrySchlickGGX(float NdotV, float roughness)
		{
		    float r = (roughness + 1.0);
		    float k = (r*r) / 8.0;
		
		    float denom = NdotV * (1.0 - k) + k;
			
		    return NdotV / denom;
		}

		float GeometrySmith(float3 normal, float3 viewDir, float3 lightDir, float roughness)
		{
		    float NdotV = max(dot(normal, viewDir), 0.0);
		    float NdotL = max(dot(normal, lightDir), 0.0);
		    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
		    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
			
		    return ggx1 * ggx2;
		}

		float3 fresnelSchlick(float cosTheta, float3 F0)
		{
		    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
		} 

		POut compute_lighting(float3 normal, float3 position, float3 lightDir, float3 lightColor, float metallic, float roughness)
		{
			POut output;

			float3 viewDir		= normalize(camPos - position);
			float3 halfAngleVec	= normalize(lightDir + viewDir);

			float3 F0			= 0.04;
			F0					= lerp(F0,basic,metallic);

			float NDF	= DistributionGGX(normal, halfAngleVec, roughness);        
			float G		= GeometrySmith(normal, viewDir, lightDir, roughness);      
			float3 F	= fresnelSchlick(max(dot(halfAngleVec, viewDir), 0.0), F0);

			float3 kD = 1.0 - F;
			kD *= 1.0 - metallic;

			float3 numerator		=  NDF * G * F;
			float denominator	=  4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);

			float NdotL			= max(dot(normal, lightDir), 0.0);
			output.diffuse		= float4((kD* basic)*NdotL*lightColor,1.0);
			output.specular     = float4((numerator / max(denominator, 0.001))*NdotL*lightColor,1.0);   
			
			return output;
		}

		)";

		/*================== END SHADER INCLUDE ==================*/

		struct DirLight
		{
				Core::Math::Vec3 dir;
				Core::Math::Vec3 color = {1.0f,1.0f,1.0f};
				bool castShadow = false;
				Core::Math::Mat4 lightViewProj;
		};

		#define POINT_LIGHT_MAX_NB 64

		struct PointLight
		{
				Core::Math::Vec3 pos	= { 0.0f,0.0f,0.0f };
				float radius			= 0.0f;
				Core::Math::Vec3 color  = { 1.0f,1.0f,1.0f };

		};

		struct LightsArray
		{
			public:
				DirLight dirLight;
				bool useDir = true;
				std::array<PointLight,POINT_LIGHT_MAX_NB> pointLights;
				unsigned int usedPoints = 0;
		};
	}
}

#endif // !__LIGHT_HPP__
