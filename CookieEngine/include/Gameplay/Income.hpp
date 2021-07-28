#ifndef _INCOME_HPP__
#define _INCOME_HPP__

#include <string>

namespace Cookie
{
	namespace Gameplay
	{
		#define DEFAULT_INCOME_PRIMARY    1000
		#define DEFAULT_INCOME_SECONDARY  500
		#define DEFAULT_INCOME_SUPPLY_MAX 200

		struct Income
		{
			float primary        {DEFAULT_INCOME_PRIMARY};
			float secondary      {DEFAULT_INCOME_SECONDARY};
			float supplyCurrent  {0};
			float supplyMax      {DEFAULT_INCOME_SUPPLY_MAX};

			void Reset()
			{
				primary       = DEFAULT_INCOME_PRIMARY;
				secondary     = DEFAULT_INCOME_SECONDARY;
				supplyCurrent = 0;
				supplyMax     = DEFAULT_INCOME_SUPPLY_MAX;
			}
		};

	}
}

#endif // !_INCOME_HPP__