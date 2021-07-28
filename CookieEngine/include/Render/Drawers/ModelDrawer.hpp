#ifndef __MODEL_DRAWER_HPP__
#define __MODEL_DRAWER_HPP__

#include <vector>

struct ID3D11Buffer;

namespace Cookie
{
	namespace Render
	{
		class DrawDataHandler;
		struct DrawData;

		/* class that handles the drawing of DrawData.
		 * (meaning the drawing of instanced models)
		 * */
		class ModelDrawer
		{
		private:
			/* the Instance Buffer and size for the models matrices */
			ID3D11Buffer*		IBuffer{ nullptr };
			unsigned int		IBufferSize = 0;

		private:
			void AllocateInstance(unsigned int size);

		public:
			ModelDrawer();
			~ModelDrawer();

			void Draw(const std::vector<DrawData>& toDraw, bool drawOccluded);
		};
	}
}

#endif /* __MODEL_DRAWER_HPP__ */