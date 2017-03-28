#ifndef GCL_TEST_HPP__
# define GCL_TEST_HPP__

#include "gcl/test/event.hpp"
#include "gcl/test/container/polymorphic_vector.hpp"
#include "gcl/test/type_info.hpp"

namespace gcl
{
	namespace test
	{
		template <typename ... tunits>
		static void proceed_one(tunits... tunit_val)
		{
			std::function<void()> tunit_list[] = { tunit_val... };
			for (auto & unit : tunit_list)
				unit();
		}

		static void proceed()
		{
			// todo : test::type return tunits... (test::type::test1, test::type::test2, etc...)
			/*proceed_one(
				gcl::test::event::handler,
				gcl::test::event::dispatcher,
				gcl::test::event::experimental::static_socket);*/

			// todo : sfinae reflexion check : has_proceed_symbol ? do test : "not implemented"

			// todo : clean output
			gcl::test::event::proceed();
			gcl::test::container::proceed();
			gcl::test::type_info::experimental::proceed();
		}
	}
}

#endif // GCL_TEST_HPP__