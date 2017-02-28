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

namespace gse
{
	namespace event
	{
		struct type
		{
			virtual ~type() {}
		};

		struct handler
		{
			using typeinfo_holder_t = gcl::type_info::holder<type>;
			using event_callback_t = std::function<void(const type &)>;

			handler() = default;
			handler(handler &&) = delete;
			handler(handler &) = delete;
			virtual ~handler() {}

			template <typename event_t>
			void on(const event_t & ev)
			{
				static_assert(std::is_base_of<type, event_t>::value, "event_t do not derive from type");
				this->on(gcl::type_info::id<event_t>::value, ev);
			}
			void on(const typeinfo_holder_t & holder)
			{
				this->on(holder.id, *(holder.value));
			}
			virtual void on(gcl::type_info::id_type event_id, const type & ev) = 0;

		protected:
			template <typename event_t>
			void AddListener(event_callback_t && callback)
			{
				AddListener(gcl::type_info::id<event_t>::value, callback);
			}
			virtual void AddListener(gcl::type_info::id_type event_id, event_callback_t && callback) = 0;
		};

		struct single_handler : handler
		{
			using event_callback_t = handler::event_callback_t;
			using socket_container_t = std::unordered_map<gcl::type_info::id_type, event_callback_t>;

			single_handler() = default;
			single_handler(std::initializer_list<socket_container_t::value_type> && il)
				: sockets{ il }
			{}

			void on(gcl::type_info::id_type event_id, const type & ev) override
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

		struct multi_handler : handler
		{
			using event_callback_t = handler::event_callback_t;
			using socket_container_t = std::unordered_multimap<gcl::type_info::id_type, event_callback_t>;

			multi_handler(std::initializer_list<socket_container_t::value_type> && il)
				: sockets{ il }
			{}

			void on(gcl::type_info::id_type event_id, const type & ev) override
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
		struct route
		{
			using value_type = T;
			using node_type = std::function<void(const value_type &)>;
			using node_list_type = typename std::vector<node_type>;

			route() = default;
			route(std::initializer_list<typename node_list_type::value_type> il)
				: nodes{ il }
			{}
			route(const route &) = default;
			route(const route &&) = delete;

			void add_node(node_type && node)
			{
				nodes.emplace_back(std::move(node));
			}
			void drive_sync(const value_type & elem)
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

		struct dispatcher
		{	// subject
			template <typename event_t>
			void subscribe(const std::shared_ptr<handler> & handler)
			{
				subscribers[gcl::type_info::id<T>::value].insert(handler);
			}
			template <typename event_t>
			void unsubscribe(const std::shared_ptr<handler> & handler)
			{
				subscribers.at(gcl::type_info::id<T>::value).erase(handler);
			}

			template <typename event_t>
			void dispatch(const event_t & ev)
			{
				static_assert(std::is_base_of<event::type, event_t>::value, "event_t do not derive from type");
				dispatch(gcl::type_info::id<T>::value, ev);
			}
			void dispath(const gcl::type_info::holder<gse::event::type> & holder)
			{
				dispath(holder.id, *(holder.value));
			}
			void dispath(gcl::type_info::id_type event_id, const event::type & ev)
			{
				for (auto & subscriber : subscribers.at(event_id))
					subscriber->on(ev);
			}

		protected:
			using handler_set_t = std::unordered_set<std::shared_ptr<handler>>;
			std::unordered_map<gcl::type_info::id_type, handler_set_t> subscribers;
		};

		struct system
		{
			using event_holder_t = gcl::type_info::holder<event::type>;

			template <typename event_t>
			void process(const event_t & ev)
			{
				static_assert(std::is_base_of<event::type, event_t>::value, "event_t do not derive from type");
				process(gcl::type_info::id<T>::value, ev);
			}
			void process(const gcl::type_info::holder<gse::event::type> & holder)
			{
				// todo
			}
			void process(gcl::type_info::id_type event_id, const event::type & ev)
			{
				process(event_holder_t(event_id, std::make_unique<event::type>(ev)));
			}

			using event_route_t = event::route<event_holder_t>;
			event_route_t route;
			event::dispatcher dispatcher;
		};

		namespace experimental
		{
			template <typename ... t_events>
			struct static_socket
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
		}
	}
}

#endif // GGE_EVENTS__