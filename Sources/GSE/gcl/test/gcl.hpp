#ifndef GCL_TEST_GCL_HPP__
# define GCL_TEST_GCL_HPP__

// todo : replace by gcl/gcl.hpp ?

#include "gcl/test/event.hpp"
#include "gcl/test/container/polymorphic_vector.hpp"
#include "gcl/test/type_info.hpp"

#include "gcl/test.hpp"

namespace gcl
{
	namespace test
	{
		static void proceed()
		{
			// todo : test::type return tunits... (test::type::test1, test::type::test2, etc...)
			// proceed_one(gcl::test::event::pack_t {});
			struct toto {};
			gcl::test::component<toto>::test();
			gcl::test::component<gcl::test::event>::test();

			// todo :
			// sfinae reflexion check : has_proceed_symbol
			// ? do test
			// : "not implemented" || is itself a pack ?

			// todo :
			// proceed() may be void() or bool()

			// todo : clean output
			/*gcl::test::event::proceed();
			gcl::test::container::proceed();
			gcl::test::type_info::experimental::proceed();*/
		}
	}
}

#endif // GCL_TEST_GCL_HPP__