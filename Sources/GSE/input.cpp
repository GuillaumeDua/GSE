#include "input.hpp"

std::shared_ptr<gse::input::handler::container_t>	gse::input::debug_handler_value::sockets = std::make_shared<gse::input::handler::container_t>(gse::input::handler::container_t
{
	{
		sf::Event::Closed,
		[&](const sf::Event & event) { std::cout << "Window closed" << std::endl; }
	},
	{
		sf::Event::Resized,
		[&](const sf::Event & event) { std::cout << "Resized : " << event.size.width << 'x' << event.size.height << std::endl; }
	},
	{
		sf::Event::LostFocus,
		[&](const sf::Event & event) { std::cout << "Focus lost" << std::endl; }
	},
	{
		sf::Event::GainedFocus,
		[&](const sf::Event & event) { std::cout << "Focus gained" << std::endl; }
	},
	{
		sf::Event::KeyPressed,
		[&](const sf::Event & event) { std::cout << "KeyPress : " << event.key.code << std::endl; }
	},
	{
		sf::Event::KeyReleased,
		[&](const sf::Event & event) { std::cout << "KeyReleased : " << event.key.code << std::endl; }
	},
	{
		sf::Event::TextEntered,
		[&](const sf::Event & event) { std::cout << "Text entered : " << event.text.unicode << " [" << static_cast<char>(event.text.unicode) << ']' << std::endl; }
	},
	{
		sf::Event::MouseWheelMoved,
		[&](const sf::Event & event) { std::cout << "Wheel movement : " << event.mouseWheel.delta << " on : " << event.mouseWheel.x << 'x' << event.mouseWheel.y << std::endl; }
	},
	{
		sf::Event::MouseButtonPressed,
		[&](const sf::Event & event) { std::cout << "MouseButtonPressed : " << event.mouseButton.button << " : " << event.mouseButton.x << 'x' << event.mouseButton.y << std::endl; }
	},
	{
		sf::Event::MouseMoved,
		[&](const sf::Event & event) { /*std::cout << "MouveMoved : " << event.mouseMove.x << 'x' << event.mouseMove.y << std::endl;*/ }
	},
	{
		sf::Event::MouseEntered,
		[&](const sf::Event & event) { std::cout << "MouseEntered" << std::endl; }
	},
	{
		sf::Event::MouseLeft,
		[&](const sf::Event & event) { std::cout << "MouseLeft" << std::endl; }
	}
});