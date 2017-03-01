#include "../event.hpp"

namespace test
{
	namespace event
	{
		void handler()
		{
			struct A_event : gse::event::type
			{
				virtual ~A_event() override {}
				int a_var = 42;
			};
			struct B_event : gse::event::type
			{
				~B_event() {}
				int b_var = 13;
			};

			std::unique_ptr<gse::event::handler> handler(new gse::event::single_handler
			{
				{ gcl::type_info::id<A_event>::value, [](const gse::event::type & ev) { std::cout << "A type catch" << std::endl; } },
				{ gcl::type_info::id<B_event>::value, [](const gse::event::type & ev) { std::cout << "B type catch" << std::endl; } }
			});

			handler->on(A_event{});

			gse::event::type * ev = new B_event();
			handler->on(gcl::type_info::id<B_event>::value, *ev);

			gcl::type_info::holder<gse::event::type> event_value_holder(new B_event());
			handler->on(event_value_holder);
		}

		namespace experimental
		{
			void static_socket()
			{
				struct Event_A {};
				struct Event_B {};
				struct Event_C {};

				{	// event_socket
					gse::event::experimental::static_socket<Event_A, Event_B, Event_C> myEventHandler;

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

		static void proceed()
		{
			handler();
			experimental::static_socket();
		}
	}
}