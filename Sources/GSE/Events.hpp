#ifndef GGE_EVENTS__
# define GGE_EVENTS__

#include <iostream>
#include <array>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <future>

#include "gcl/type_info.hpp"

template <typename ... t_events>
struct event_socket
{
	using socket_callback = std::function<void()>;
	using socket_type = std::vector<socket_callback>;
	using tuple_info_t = gcl::type_info::tuple<t_events...>;

	template <typename T>
	socket_type & on()
	{
		return sockets[tuple_info_t::indexOf<T>()];
	}
	template <typename T>
	void add(socket_callback cb)
	{
		sockets[tuple_info_t::indexOf<T>()].push_back(cb);
	}

	template <typename T>
	void trigger()
	{
		for (auto & elem : on<T>())
			elem();
	}
	template <size_t N>
	void trigger()
	{
		for (auto & elem : sockets[N])
			elem();
	}
	template <typename T>
	void trigger(const T &)
	{
		trigger<T>();
	}
	template <>
	void trigger<size_t>(const size_t & N)
	{
		trigger<N>();
	}

protected:

	std::array<socket_type, sizeof...(t_events)> sockets;
};

struct event
{
	virtual ~event() {}
};

struct event_handler
{
	using typeinfo_holder_t = gcl::type_info::holder<event>;
	using event_callback_t = std::function<void(const event &)>;

	event_handler() = default;
	event_handler(event_handler &&) = delete;
	event_handler(event_handler &) = delete;
	virtual ~event_handler() {}

	template <typename event_t>
	void on(const event_t & ev)
	{
		static_assert(std::is_base_of<event, event_t>::value, "event_t do not derive from event");
		this->on(gcl::type_info::id<event_t>::value, ev);
	}
	void on(const typeinfo_holder_t & holder)
	{
		this->on(holder.id, *(holder.value));
	}
	virtual void on(gcl::type_info::id_type event_id, const event & ev) = 0;

protected:
	template <typename event_t>
	void AddListener(event_callback_t && callback)
	{
		AddListener(gcl::type_info::id<event_t>::value, callback);
	}
	virtual void AddListener(gcl::type_info::id_type event_id, event_callback_t && callback) = 0;
};

struct simple_event_handler : event_handler
{
	using event_callback_t = event_handler::event_callback_t;
	using socket_container_t = std::unordered_map<gcl::type_info::id_type, event_callback_t>;

	simple_event_handler() = default;
	simple_event_handler(std::initializer_list<socket_container_t::value_type> && il)
		: sockets{ il }
	{}

	void on(gcl::type_info::id_type event_id, const event & ev) override
	{
		sockets.at(event_id)(ev);
	}

protected:
	void AddListener(gcl::type_info::id_type event_id, event_callback_t && callback)  override
	{
		sockets[event_id] = callback;
	}
	socket_container_t sockets;
};

struct multi_event_handler : event_handler
{
	using event_callback_t = event_handler::event_callback_t;
	using socket_container_t = std::unordered_multimap<gcl::type_info::id_type, event_callback_t>;

	multi_event_handler(std::initializer_list<socket_container_t::value_type> && il)
		: sockets{ il }
	{}

	void on(gcl::type_info::id_type event_id, const event & ev) override
	{
		auto range = sockets.equal_range(event_id);
		for (auto it = range.first; it != range.second; ++it)
			it->second(ev);
	}

protected:
	void AddListener(gcl::type_info::id_type event_id, event_callback_t && callback)  override
	{
		sockets.insert(socket_container_t::value_type{ event_id, callback });
	}
	socket_container_t sockets;
};

template <typename T>
struct Route
{
	using node_type = std::function<void(const T &)>;
	using node_list_type = typename std::vector<node_type>;

	Route() = default;
	Route(std::initializer_list<typename node_list_type::value_type> il)
		: nodes{ il }
	{}
	Route(const Route &) = default;
	Route(const Route &&) = delete;

	void add_node(node_type && node)
	{
		nodes.emplace_back(std::move(node));
	}
	void drive_sync(const T & elem)
	{
		for (auto & node : nodes)
		{
			node(elem);
		}
	}
	template <class InputIterator>
	void drive_sync(InputIterator first, InputIterator last)
	{
		while (first != last)
		{
			drive_sync(*first);
			++first;
		}
	}
	template <class InputIterator>
	void drive_async(InputIterator first, InputIterator last)
	{
		using future_type = std::future<void>;
		std::vector<future_type>	futures;
		std::vector<std::mutex>		node_mutexes(nodes.size(), std::mutex{});

		while (first != last)
		{
			futures.emplace_back(std::move(std::async(std::launch::async, [&mutexes]() {
				for (uint32_t i = 0; i < nodes.size(); ++i)
				{
					std::unique_lock<std::mutex> lock(mutexes[i]);
					nodes[i](elem);
				}
			})));
			++first;
		}
		
		for (auto & future : futures)
			future.get();
	}

protected:
	node_list_type	nodes;
};


// Route<event_t>	event_route;

struct event_dispatcher
{	// subject
	template <typename event_t>
	void subscribe(const std::shared_ptr<event_handler> & handler)
	{
		subscribers[gcl::type_info::id<T>::value].insert(handler);
	}
	template <typename event_t>
	void unscubscribe(const std::shared_ptr<event_handler> & handler)
	{
		subscribers.at(gcl::type_info::id<T>::value).erase(handler);
	}

	template <typename event_t>
	void dispatch(const event_t & event)
	{
		for (auto & subscriber : subscribers.at(gcl::type_info::id<T>::value))
			subscriber.on_notify(event);
	}


protected:
	using handler_set_t = std::unordered_set<std::shared_ptr<event_handler>>;
	std::unordered_map<gcl::type_info::id_type, handler_set_t> subscribers;
};

void EventAPI_test()
{
	struct Event_A {};
	struct Event_B {};
	struct Event_C {};

	{	// event_socket
		event_socket<Event_A, Event_B, Event_C> myEventHandler;

		myEventHandler.add<Event_A>([]() { std::cout << "A triggered !" << std::endl; });
		myEventHandler.add<Event_B>([]() { std::cout << "B triggered !" << std::endl; });
		myEventHandler.add<Event_C>([]() { std::cout << "C triggered !" << std::endl; });
		myEventHandler.add<Event_A>([]() { std::cout << "A triggered another time" << std::endl; });
		myEventHandler.add<Event_B>([]() { std::cout << "B triggered another time" << std::endl; });
		myEventHandler.add<Event_C>([]() { std::cout << "C triggered another time" << std::endl; });

		myEventHandler.trigger<Event_B>();
		myEventHandler.trigger<0>();
		myEventHandler.trigger(Event_C{});
	}
}

#endif // GGE_EVENTS__