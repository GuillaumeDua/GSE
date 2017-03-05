#ifndef GSE_INPUT_HPP__
# define GSE_INPUT_HPP__

# include "ext_libs_wrapper.hpp"

# include <memory>
# include <unordered_map>
# include <functional>
# include <iostream>

namespace gse
{
	namespace input
	{
		using event_t = gse::ext_lib_wrapper::SFML::input::event_t;

		struct handler
		{
			using container_t = std::unordered_map < event_t::EventType, std::function<void(sf::Event &)>>;

			handler(std::shared_ptr<container_t> & sock)
				: sockets{sock}
			{}

		protected:
			std::shared_ptr<container_t> sockets;
		};

#ifdef _DEBUG
		struct debug_handler_value
		{
			static std::shared_ptr<handler::container_t> sockets;
		};
#endif
	}
}


#endif // GSE_INPUT_HPP__