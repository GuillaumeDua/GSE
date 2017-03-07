#ifndef GCL_CONTAINER_TEST_POLYMORPHIC_VECTOR_HPP__
# define GCL_CONTAINER_TEST_POLYMORPHIC_VECTOR_HPP__

# include "gcl/container/polymorphic_vector.hpp"
# include <cassert>

namespace gcl
{
	namespace test
	{
		namespace container
		{
			namespace polymorphic_vector
			{
				void proceed()
				{
					struct A
					{
						virtual ~A() {}
						virtual void do_stuff() = 0;
					};
					struct B : A
					{
						B(int i)
							: _i(i)
						{}
						int _i;
						void do_stuff() {}
					};
					struct C : A
					{
						C(const std::string & str)
							: _str(str)
						{}
						std::string _str;
						void do_stuff() {}
					};

					gcl::container::polymorphic_vector<A> container;

					container.emplace_back<B>(1);
					container.emplace_back<C>("one");

					for (auto & elem : container.get())
					{
						elem->do_stuff();
					}
					// B = B._i +1
					container.visit<B>([](A & elem)
					{
						reinterpret_cast<B*>(&elem)->_i += 1;
					});
					// C = C._str + 'X'
					container.visit<C>([](A & elem)
					{
						reinterpret_cast<C*>(&elem)->_str += 'X';
					});

					B * b_ptr = reinterpret_cast<B*>(container.get<B>().front());
					assert(b_ptr);
					assert(b_ptr->_i == 2);

					C * c_ptr = reinterpret_cast<C*>(container.get<C>().front());
					assert(c_ptr);
					assert(c_ptr->_str.back() == 'X');
				}
			}
		}
	}
}


#endif // GCL_CONTAINER_TEST_POLYMORPHIC_VECTOR_HPP__