#ifndef _AISTEP_HPP__
#define _AISTEP_HPP__

#include <vector>
#include <string>

namespace Cookie
{
	namespace Gameplay
	{
		struct AIStep
		{
			int nbOfWorker {0};
			int nbOfUnits  {0};  //maybe use powerLevel instead
			std::vector<std::string> listOfBuildings;

			void Clear()
			{
				nbOfWorker = 0;
				nbOfUnits = 0;
				listOfBuildings.clear();
			}

			AIStep operator+(const AIStep& other)
			{
				AIStep toReturn = *this;

				toReturn.nbOfWorker += other.nbOfWorker;
				toReturn.nbOfUnits  += other.nbOfUnits;

				for (int i = 0; i < other.listOfBuildings.size(); ++i)
					toReturn.listOfBuildings.push_back(other.listOfBuildings[i]);

				return toReturn;
			}
			AIStep& operator+=(const AIStep& other) { *this = *this + other; return *this; }
		};
	}
}

#endif // !_AISTEP_HPP__