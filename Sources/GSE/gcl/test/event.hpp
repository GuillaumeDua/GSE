#include "../event.hpp"
#include <string>
#include <type_traits>

namespace gcl
{
	namespace test
	{
		struct event
		{
			using gcl_event_t = gcl::event<>;

			struct A_event : gcl_event_t::interface_t
			{
				A_event() = default;
				explicit A_event(int value)
					: a_var(value)
				{}
				int a_var = 0;

				static std::size_t counter;
			};
			struct B_event : gcl_event_t::interface_t
			{
				B_event() = default;
				B_event(std::string && value)
					: b_var(value)
				{}
				std::string b_var = "default";

				static std::size_t counter;
			};
			struct C_event : gcl_event_t::interface_t
			{
				C_event() = default;
				explicit C_event(int value)
					: c_var(value)
				{}
				int c_var = 0;

				static std::size_t counter;
			};

			struct handler
			{
				static void proceed()
				{
					using handler_t = gcl_event_t::select_handler::get
					<
						gcl_event_t::select_handler::many,
						gcl_event_t::select_handler::one
					>::handler_t;
					static_assert(std::is_same<handler_t, gcl_event_t::many_to_one_handler>::value, "gcl::test::event::handler::proceed : select_handler fail");

					std::unique_ptr<gcl_event_t::handler> handler(new handler_t
					{
						{ gcl::type_info::id<B_event>::value, [](const gcl_event_t::interface_t & ev) { B_event::counter++; } },
						{ gcl::type_info::id<C_event>::value, [](const gcl_event_t::interface_t & ev) { C_event::counter++; } }
					});

					handler->add_listener<A_event>([](const gcl_event_t::interface_t & ev) { A_event::counter++; });
					if (A_event::counter != 0) throw std::runtime_error("gcl::test::event::handler : bad A_event::counter value");
					handler->on(A_event{});
					if (A_event::counter != 1) throw std::runtime_error("gcl::test::event::handler : bad A_event::counter value");

					gcl_event_t::interface_t * ev = new B_event();
					if (B_event::counter != 0) throw std::runtime_error("gcl::test::event::handler : bad B_event::counter value");
					handler->on(gcl::type_info::id<B_event>::value, *ev);
					if (B_event::counter != 1) throw std::runtime_error("gcl::test::event::handler : bad B_event::counter value");

					gcl::type_info::holder<gcl_event_t::interface_t> event_value_holder(new C_event());
					if (C_event::counter != 0) throw std::runtime_error("gcl::test::event::handler : bad C_event::counter value");
					handler->on(event_value_holder);
					if (C_event::counter != 1) throw std::runtime_error("gcl::test::event::handler : bad C_event::counter value");
				}
			};
			struct dispatcher
			{
				static void proceed()
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
			};
			struct experimental
			{
				struct static_socket
				{
					static void proceed()
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
				};

				using pack_t = std::tuple<static_socket>;
			};

			struct route {};

			using pack_t = std::tuple<handler, dispatcher, experimental, route>;
		};
	}
}

std::size_t gcl::test::event::A_event::counter = 0;
std::size_t gcl::test::event::B_event::counter = 0;
std::size_t gcl::test::event::C_event::counter = 0;