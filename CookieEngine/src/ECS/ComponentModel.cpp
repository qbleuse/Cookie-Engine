#include "Render/D3D11Helper.hpp"
#include "Mat4.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "ECS/ComponentModel.hpp"

using namespace Cookie::ECS;;

/*============================ CONSTRUCTORS ============================*/

ComponentModel::ComponentModel()
{

}

ComponentModel::~ComponentModel()
{

}

/*============================ REALTIME METHODS ============================*/

void ComponentModel::ToDefault()
{
    mesh = nullptr;
    icon = nullptr;
    albedo = nullptr;
    normal = nullptr;
    metallicRoughness = nullptr;
}