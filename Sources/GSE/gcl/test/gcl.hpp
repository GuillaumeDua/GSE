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
			struct toto {};
			gcl::test::component<toto>::test();
			gcl::test::component<gcl::test::event>::test();
		}
	}
}

#endif // GCL_TEST_GCL_HPP__