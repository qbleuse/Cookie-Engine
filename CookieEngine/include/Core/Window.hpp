#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <GLFW/glfw3.h>

namespace Cookie
{
	namespace Core
	{
		class Window
		{

			public:
			GLFWwindow* window = nullptr;
			int width	= 0;
			int height	= 0;

			private:
				void SetIcon();

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Window();
				~Window();

				
		};
	}
}

#endif /*__WINDOW_HPP__*/