#ifndef GCL_TEST_HPP__
# define GCL_TEST_HPP__

#include "gcl/test/event.hpp"
#include "gcl/test/container/polymorphic_vector.hpp"

namespace gcl
{
	namespace test
	{
		void proceed()
		{
			gcl::test::event::proceed();
			gcl::test::container::polymorphic_vector::proceed();
		}
	}
}

#endif // GCL_TEST_HPP__