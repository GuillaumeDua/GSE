#ifndef GSE_ENGINE_TEST_HPP__
# define GSE_ENGINE_TEST_HPP__

#include "../engine.hpp"
#include <cassert>

namespace gse
{
	namespace test
	{
		namespace engine
		{
			void test1_window()
			{
				gse::window<> window
				{ 
					std::make_pair(800, 600),
					"test window",
					"../../image/fedora_verne.png"
				};
				assert(window);

				while (true)
				{
					window.display();
				}
			}

			void proceed()
			{
				test1_window();
			}
		}
	}
}

#endif // GSE_ENGINE_TEST_HPP__