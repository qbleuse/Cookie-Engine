#ifndef __COMPONENT_GAMEPLAY_HPP__
#define __COMPONENT_GAMEPLAY_HPP__

#include "EntityHandler.hpp"
#include "Gameplay/CGPLive.hpp"
#include "Gameplay/CGPMove.hpp"
#include "Gameplay/CGPAttack.hpp"
#include "Gameplay/CGPProducer.hpp"
#include "Gameplay/CGPWorker.hpp"
#include "Gameplay/CGPResource.hpp"


namespace Cookie
{
	namespace Gameplay
	{
		enum E_ARMY_NAME
		{
			E_DEFAULT_NAME,
			E_PLAYER,
			E_AI1
		};

		enum E_ARMY_TYPE
		{
			E_DEFAULT_TYPE,
			E_WORKER,
			E_UNIT,
			E_BUILDING
		};
	}

	namespace ECS
	{
		enum CGP_SIGNATURE
		{
			EMPTY_CGP     = 0b000000,
			LIVE          = 0b000001,
			MOVE          = 0b000010,
			ATTACK	      = 0b000100,
			PRODUCER      = 0b001000,
			WORKER        = 0b010000,
			RESOURCE      = 0b100000,
			ALL_CGP       = 0b111111
		};

		struct Cost
		{
			float costPrimary   {0};
			float costSecondary {0};
			float costSupply    {0};
			float timeToProduce {0};
		};

		class ComponentGameplay
		{

		public:
			Gameplay::E_ARMY_NAME  teamName {Gameplay::E_ARMY_NAME::E_DEFAULT_NAME};
			Gameplay::E_ARMY_TYPE  type {Gameplay::E_ARMY_TYPE::E_DEFAULT_TYPE};
			int                    signatureGameplay {0};
			Cost                   cost;
			ComponentTransform*    trs {nullptr}; //for armyCoordinator

			Gameplay::CGPLive      componentLive;
			Gameplay::CGPMove      componentMove;
			Gameplay::CGPAttack    componentAttack;
			Gameplay::CGPProducer  componentProducer;
			Gameplay::CGPWorker    componentWorker;
			Gameplay::CGPResource  componentResource;

			inline void AddComponent(int ComponentSignature) noexcept;
			inline void RemoveComponent(int ComponentSignature)noexcept;

			inline void ToDefault() noexcept;
			inline void SubComponentToDefault(int ComponentSignature)noexcept;

		};
	}
}

#include "ComponentGameplay.inl"

#endif // !__COMPONENT_GAMEPLAY_HPP__
