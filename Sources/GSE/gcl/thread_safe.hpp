#ifndef GCL_THREAD_SAFE__
# define GCL_THREAD_SAFE__

#include <queue>
#include <mutex>

namespace gcl
{
	namespace thread_safe
	{
		template <typename T>
		struct queue
		{
			using element_type = std::decay<T>::type;

			void push(element_type && ev)
			{
				std::unique_lock<std::mutex> lock(cache_mutex);
				cache.push(std::make_unique<element_type>(std::move(ev)));
			}
			std::unique_ptr<element_type> && pop()
			{
				if (cache.empty()) return;
				std::unique_ptr<element_type> elem;
				{
					std::unique_lock<std::mutex> lock(cache_mutex);
					elem = std::move(cache.back());
					cache.pop();
				}
				return std::move(elem);
			}

		private:
			std::queue<std::unique_ptr<event>> cache;
			std::mutex cache_mutex;
		};
	}
}

#endif // GCL_THREAD_SAFE__