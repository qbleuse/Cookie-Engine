#ifndef _PLAYERDATA_HPP__
#define _PLAYERDATA_HPP__

#include "Vec3.hpp"
#include "CGPProducer.hpp"
#include "CGPWorker.hpp"


namespace Cookie
{
	namespace Gameplay
	{
		struct PlayerData
		{
			Core::Math::Vec3 mousePosInWorld {0, 0, 0};

			bool makingASelectionQuad {false};
			Core::Math::Vec3 selectionQuadStart{ 0, 0, 0 };
			
			CGPProducer*     buildingToBuild          {nullptr};
			CGPWorker*       workerWhoBuild           {nullptr};
			Core::Math::Vec3 buildingPos              {0, 0, 0};
			int              indexOfBuildingInWorker  {0};
			bool		     isBuildingValid          {false};
		};
	}
}

#endif // !_PLAYERDATA_HPP__