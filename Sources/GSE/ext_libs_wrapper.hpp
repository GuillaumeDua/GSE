#ifndef GSE_EXT_LIBS_WRAPPER_HPP__
# define GSE_EXT_LIBS_WRAPPER_HPP__

# include <SFML\Window\Event.hpp>
# include <SFML\Graphics.hpp>

namespace gse
{
	namespace ext_lib_wrapper
	{
		// todo : porta with other libs / stand-alone systems

		struct SFML
		{
			struct input
			{
				using event_t = sf::Event;
			};
			struct draw
			{
				using render_window_t = sf::RenderWindow;
				using window_t = sf::Window;
				using image_t = sf::Image;
				using texture_t = sf::Texture;
				using sprite_t = sf::Sprite;
				using color_t = sf::Color;
			};
		};
	}
}

#endif // GSE_EXT_LIBS_WRAPPER_HPP__
