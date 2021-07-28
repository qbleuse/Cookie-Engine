#ifndef __COMPONENT_GAMEPLAY_INL__
#define __COMPONENT_GAMEPLAY_INL__

#include "Debug.hpp"

namespace Cookie
{
	namespace ECS
	{

		inline void ComponentGameplay::AddComponent(int ComponentSignature) noexcept
		{
			if (signatureGameplay & ComponentSignature)
			{
				CDebug.Warning("Try to Add a Component already present");
				return;
			}

			signatureGameplay += ComponentSignature;
		}
		inline void ComponentGameplay::RemoveComponent(int ComponentSignature)noexcept
		{
			if (signatureGameplay & ComponentSignature)
			{
				SubComponentToDefault(ComponentSignature);
				signatureGameplay -= ComponentSignature;
				return;
			}

			CDebug.Warning("Try to Remove a Component not present");
		}

		inline void ComponentGameplay::ToDefault()noexcept
		{
			SubComponentToDefault(signatureGameplay);

			teamName		   = Gameplay::E_ARMY_NAME::E_DEFAULT_NAME;
			type               = Gameplay::E_ARMY_TYPE::E_DEFAULT_TYPE;
			signatureGameplay  = CGP_SIGNATURE::EMPTY_CGP;
			cost.costPrimary   = 0;
			cost.costSecondary = 0;
			cost.costSupply    = 0;
			cost.timeToProduce = 0;
		}

		inline void ComponentGameplay::SubComponentToDefault(int ComponentSignature)noexcept
		{
			if (ComponentSignature & CGP_SIGNATURE::LIVE)
				componentLive.ToDefault();
			if (ComponentSignature & CGP_SIGNATURE::MOVE)
				componentMove.ToDefault();
			if (ComponentSignature & CGP_SIGNATURE::ATTACK)
				componentAttack.ToDefault();
			if (ComponentSignature & CGP_SIGNATURE::PRODUCER)
				componentProducer.ToDefault();
			if (ComponentSignature & CGP_SIGNATURE::WORKER)
				componentWorker.ToDefault();
			if (ComponentSignature & CGP_SIGNATURE::RESOURCE)
				componentResource.ToDefault();
		}

	}
}


#endif