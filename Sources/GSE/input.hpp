#ifndef GSE_INPUT_HPP__
# define GSE_INPUT_HPP__

# include "ext_libs_wrapper.hpp"

# include "gcl/preprocessor.h"

# include <memory>
# include <unordered_map>
# include <functional>
# include <iostream>

namespace gse
{
	template <class ext_lib_wrapper_t>
	struct input
	{
		GCL_PREPROCESSOR__NOT_INSTANTIABLE(input)

		using event_t = typename ext_lib_wrapper_t::input::event_t;
		using event_id_t = typename ext_lib_wrapper_t::input::event_id_t;

		struct handler
		{
			// todo : multimap / map<key_t, std::vector<value_t>> [?]
			using container_t = std::unordered_map<event_id_t, std::function<void(sf::Event &)>>;

			handler(std::shared_ptr<container_t> const & sock)
				: sockets{sock}
			{}

			inline handler & operator=(std::shared_ptr<container_t> const & s)
			{
				sockets = s;
			}
			inline auto const & operator[](const event_id_t & ev)
			{
				return sockets[ev];
			}

			inline std::shared_ptr<container_t> const & get()
			{
				return sockets;
			}

		protected:
			std::shared_ptr<container_t> sockets;
		};

#ifdef _DEBUG
		struct debug_handler_value
		{
			static std::shared_ptr<typename handler::container_t> sockets;
		};
#endif
	};
}


#endif // GSE_INPUT_HPP__