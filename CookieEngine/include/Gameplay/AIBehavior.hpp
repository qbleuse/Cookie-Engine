#ifndef _AIBEHAVIOR_HPP__
#define _AIBEHAVIOR_HPP__

#include <algorithm>
#include "Gameplay/AIStep.hpp"

namespace Cookie
{
	namespace Gameplay
	{
		struct AIBehavior
		{
			std::string			name {"No Name"}; //for UI and Save
			std::vector<AIStep> steps;

			void Clear()
			{
				name = "No Name";
				steps.clear();
			}
		};
	}
}

#endif // !_AIBEHAVIOR_HPP__