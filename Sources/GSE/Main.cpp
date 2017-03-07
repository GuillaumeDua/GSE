#include "gcl/test/event.hpp"
#include "gcl/test/container/polymorphic_vector.hpp"

int	main(int ac, char *av[])
{
	gcl::test::event::proceed();
	gcl::test::container::polymorphic_vector::proceed();

	system("pause");
	return 0;
}