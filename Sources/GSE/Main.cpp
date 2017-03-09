#include "test/engine.hpp"
#include "gcl/test/test.hpp"

int	main(int ac, char *av[])
{
	gcl::test::proceed();
	
	gse::test::engine::proceed();

	system("pause");
	return 0;
}