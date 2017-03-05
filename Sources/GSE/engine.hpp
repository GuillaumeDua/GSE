#ifndef GSE_CORE_HPP__
# define GSE_CORE_HPP__

# include "entity.hpp"
# include "ext_libs_wrapper.hpp"
# include "input.hpp"

# include <set>
# include <memory>
# include <functional>
# include <algorithm>
# include <vector>
# include <future>

namespace gse
{
	// todo : use (sorted?) std::vector for more powerful range operations

	struct entity_container
	{
		using container_t = std::set<std::unique_ptr<entity>>;	// less efficient for insert/delete, more for range op than unordered_set

		entity_container() = default;
		entity_container(const entity_container &) = delete;
		entity_container(entity_container &&) = default;
		entity_container(std::initializer_list<std::unique_ptr<entity>> && entities_val)
			: content(entities_val)
		{}

		using visitor_t = std::function<void(std::unique_ptr<entity> &)>;
		void visit(visitor_t visitor)
		{
			std::for_each(std::begin(content), std::end(content), visitor);
			// todo : split by range-chunks into threads
		}

		void add(container_t::value_type && entity_value)
		{
			content.insert(std::forward<container_t::value_type>(entity_value));
		}
		void remove(container_t::value_type const & entity_value)
		{
			content.erase(entity_value);
		}

	protected:

		container_t content;
	};

	struct core
	{

		void	run()
		{
			while (is_running && !endCondition())
			{
				input();
				update();
				draw();
			}
		}

	protected:
		void input()
		{
			// get inputs
			// handle inputs
		}
		void update()
		{
			// entities behave here
		}
		void draw()
		{
			// background.draw()
			// entities drawn here
		}

		std::function<bool()>	endCondition = []() { return false; };
		std::atomic_bool		is_running = false;

		gse::input::handler		input_handler = gse::input::debug_handler_value::sockets;	// input
		entity_container		entities;													// update
		ext_lib_wrapper::SFML::draw::sprite_t background;									// draw
	};
}

#endif // GSE_CORE_HPP__