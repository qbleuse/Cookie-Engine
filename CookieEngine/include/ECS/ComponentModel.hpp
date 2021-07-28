#ifndef __COMPONENT_MODEL_HPP__
#define __COMPONENT_MODEL_HPP__

struct ID3D11Buffer;

namespace Cookie
{
    namespace Resources
    {
        class Mesh;
        class Shader;
        class Texture;
    }

    namespace ECS
    {

        class ComponentModel
        {
        public:
            Resources::Mesh*    mesh    { nullptr };
            Resources::Texture* albedo  { nullptr };
            Resources::Texture* normal  { nullptr };
            Resources::Texture* metallicRoughness { nullptr };
            Resources::Texture* icon    { nullptr };


            //Material material;

        public:
            ComponentModel();
            ~ComponentModel();

            void ToDefault();
        };

    }
}

#endif