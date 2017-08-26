#ifndef GSE_ENGINE_TEST_HPP__
# define GSE_ENGINE_TEST_HPP__

#include <engine.hpp>
#include <cassert>

#include <future>
#include <chrono>

using namespace std::chrono_literals;

namespace gse
{
	namespace test
	{
		namespace engine
		{
			void test1_window()
			{
				using gse_types = gse::types<gse::ext_lib_wrapper::SFML>;

				auto window = ::std::make_unique<gse_types::window_t>
				(
					std::make_pair(800, 600),
					"test window",
					"../ressources/images/fedora_verne.png"
				);
				assert(*window);

				gse_types::input_handler_t	input{ {} };
				gse_types::engine_t			engine{ std::move(window), std::move(input) };

				std::future<void> stopper = std::async(std::launch::async, [&engine]()
				{
					std::this_thread::sleep_for(3s);
					engine.stop();
				}); // not get
				engine.run(); // main thread, as UI restriction
			}

			void proceed()
			{
				test1_window();
			}
		}
	}
}

#endif // GSE_ENGINE_TEST_HPP__