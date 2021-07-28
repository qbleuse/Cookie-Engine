#ifndef _CGPRESOURCE_HPP__
#define _CGPRESOURCE_HPP__


namespace Cookie
{
	namespace Gameplay
	{
		#define INITIAL_RESOURCE_RESERVE        1000
		#define MAX_WORKER_PER_RESOURCE         3
		#define MAX_RESOURCE_DISTANCE_FROM_BASE 40

		class CGPResource
		{
		public:

			float resourceReserve {INITIAL_RESOURCE_RESERVE};
			bool  isPrimary       {true};
			int   nbOfWorkerOnIt  {0};

			CGPResource() {}
			~CGPResource() {}

			inline void ToDefault() noexcept
			{
				resourceReserve = INITIAL_RESOURCE_RESERVE;
				isPrimary       = true;
				nbOfWorkerOnIt  = 0;
			}

		};


	}
}

#endif // !_CGPRESOURCE_HPP__