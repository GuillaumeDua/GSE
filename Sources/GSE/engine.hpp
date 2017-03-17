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
# include <map>

namespace gse
{
	template
	<
		class ext_lib_wrapper_t = gse::ext_lib_wrapper::SFML
	>
	struct window
	{
		using draw_t = typename ext_lib_wrapper_t::draw;

		window(std::pair<unsigned, unsigned> && dimension,
			const std::string & name, const std::string & background_path)
			: render_window(sf::VideoMode(dimension.first, dimension.second, 32), name)
		{
			if (is_valid = background_texture.loadFromFile(background_path))
				background.setTexture(background_texture);
			render_window.draw(background);
		}
		window(window &) = delete;
		window(window &&) = delete;

		inline operator bool() const
		{
			return is_valid;
		}

		void clear()
		{
			render_window.clear();
			render_window.draw(background);
		}
		void display()
		{
			render_window.display();
		}

		inline operator typename draw_t::window_t &()
		{
			return render_window;
		}
		inline typename draw_t::window_t & get()
		{
			return render_window;
		}

	protected:
		typename draw_t::render_window_t	render_window;
		typename draw_t::texture_t			background_texture;
		typename draw_t::sprite_t			background;

		bool is_valid = false;
	};

	template <class ext_lib_wrapper_t = gse::ext_lib_wrapper::SFML>
	struct core
	{
		using draw_t = typename ext_lib_wrapper_t::draw;
		using window_t = typename window<ext_lib_wrapper_t>;
		using input_handler_t = typename gse::input<ext_lib_wrapper_t>::handler;

		core(std::unique_ptr<window_t> && w, input_handler_t && input)
			: window(std::forward<std::unique_ptr<window_t>>(w))
			, input_handler(std::forward<input_handler_t>(input))
		{}

		void	run()
		{
			is_running = true;
			while (is_running && !endCondition())
			{
				input();
				update();
				draw();
			}
		}
		inline void	stop()
		{
			is_running = false;
		}

	protected:
		void input()
		{
			// get inputs
			// |- handle inputs
		}
		void update()
		{
			// entities behave here
		}
		void draw(/*typename draw_t::window_t & window*/)
		{
			// entities drawn to window here
			window->display();
		}

		std::function<bool()>		endCondition = []() { return false; };
		std::atomic_bool			is_running = false;

		input_handler_t				input_handler;	// input
		entity_manager				entities;		// update
		std::unique_ptr<window_t>	window;			// draw
	};

	template <class ext_lib_wrapper = gse::ext_lib_wrapper::SFML>
	struct types
	{
		GCL_PREPROCESSOR__NOT_INSTANTIABLE(types);

		using ext_lib_wrapper_t = gse::ext_lib_wrapper::SFML;

		using window_t = gse::window<ext_lib_wrapper_t>;
		using input_handler_t = gse::input<ext_lib_wrapper_t>::handler;
		using engine_t = gse::core<ext_lib_wrapper_t>;
	};
}

#endif // GSE_CORE_HPP__