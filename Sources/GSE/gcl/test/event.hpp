#include "../event.hpp"
#include <string>

namespace test
{
	namespace event
	{
		using gcl_event_t = gcl::event<>;

		struct A_event : gcl_event_t::interface_t
		{
			A_event() = default;
			explicit A_event(int value)
				: a_var(value)
			{}
			int a_var = 0;
		};
		struct B_event : gcl_event_t::interface_t
		{
			B_event() = default;
			B_event(std::string && value)
				: b_var(value)
			{}
			std::string b_var = "default";
		};
		struct C_event : gcl_event_t::interface_t
		{
			C_event() = default;
			explicit C_event(int value)
				: c_var(value)
			{}
			int c_var = 0;
		};

		void handler()
		{
			std::unique_ptr<gcl_event_t::handler> handler(new gcl_event_t::many_to_one_handler
			{
				{ gcl::type_info::id<B_event>::value, [](const gcl_event_t::interface_t & ev) { std::cout << "B interface_t catch" << std::endl; } },
				{ gcl::type_info::id<C_event>::value, [](const gcl_event_t::interface_t & ev) { std::cout << "C interface_t catch" << std::endl; } }
			});

			handler->add_listener<A_event>([](const gcl_event_t::interface_t & ev) { std::cout << "A interface_t catch" << std::endl; });
			handler->on(A_event{});

			gcl_event_t::interface_t * ev = new B_event();
			handler->on(gcl::type_info::id<B_event>::value, *ev);

			gcl::type_info::holder<gcl_event_t::interface_t> event_value_holder(new C_event());
			handler->on(event_value_holder);
		}

		void dispatcher()
		{
			gcl_event_t::dispatcher dispatcher;

			using handler_t = gcl_event_t::handler;
			using event_t = gcl_event_t::interface_t;

			std::shared_ptr<handler_t> handler1(new gcl_event_t::many_to_one_handler());
			std::shared_ptr<handler_t> handler2(new gcl_event_t::many_to_many_handler());
			
			std::cout
				<< gcl::type_info::id<gcl_event_t::interface_t>::value << " => gcl::type_info::id<gcl_event_t::interface_t>::value" << std::endl
				<< gcl::type_info::id<A_event>::value << " => gcl::type_info::id<A_event>::value" << std::endl
				;

			handler1->add_listener<A_event>(std::move([](const event_t & ev) { std::cout << "A_event -> handler1 : many_to_one_handler : 1 : " << static_cast<const A_event &>(ev).a_var << std::endl; }));
			/*handler1->add_listener<A_event>(std::move([](const event_t & ev) { std::cout << "A_event -> handler2 : many_to_many_handler  : 1 : " << static_cast<const A_event &>(ev).a_var << std::endl; }));
			handler1->add_listener<A_event>(std::move([](const event_t & ev) { std::cout << "A_event -> handler2 : many_to_many_handler  : 2 : " << static_cast<const A_event &>(ev).a_var << std::endl; }));
			handler1->add_listener<B_event>(std::move([](const event_t & ev) { std::cout << "B_event -> handler2 : many_to_many_handler  : 3 : " << static_cast<const B_event &>(ev).b_var << std::endl; }));*/

			dispatcher.subscribe<A_event>(handler1);
			//dispatcher.subscribe<A_event>(handler2);

			std::cout << "dispatching A_event :" << std::endl;
			dispatcher.dispatch(A_event{ 42 });
			/*std::cout << "dispatching B_event :" << std::endl;
			dispatcher.dispatch(B_event{ "Hello, world" });
			std::cout << "Remove handler1" << std::endl;
			dispatcher.unsubscribe<A_event>(handler1);
			std::cout << "dispatching A_event :" << std::endl;
			dispatcher.dispatch(A_event{ 42 });*/
		}

		namespace experimental
		{
			void static_socket()
			{
				struct Event_A {};
				struct Event_B {};
				struct Event_C {};

				{	// event_socket
					gcl_event_t::experimental::static_socket<Event_A, Event_B, Event_C> myEventHandler;

					myEventHandler.add<Event_A>([]() { std::cout << "A event : first cb" << std::endl; });
					myEventHandler.add<Event_B>([]() { std::cout << "B event : first cb" << std::endl; });
					myEventHandler.add<Event_C>([]() { std::cout << "C event : first cb" << std::endl; });
					myEventHandler.add<Event_A>([]() { std::cout << "A event : another cb" << std::endl; });
					myEventHandler.add<Event_B>([]() { std::cout << "B event : another cb" << std::endl; });
					myEventHandler.add<Event_C>([]() { std::cout << "C event : another cb" << std::endl; });

					myEventHandler.trigger<Event_B>();
					myEventHandler.trigger<0>();
					myEventHandler.trigger(Event_C{});
				}
			}
		}

		static bool proceed()
		{
			try
			{
				// handler();
				dispatcher();
				// experimental::static_socket();
			}
			catch (const std::exception & ex)
			{
				std::cerr << std::endl << "failed : " << ex.what() << std::endl;
				return false;
			}
			return true;
		}
	}
}