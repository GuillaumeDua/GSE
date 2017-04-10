#ifndef GCL_TEST_HPP__
# define GCL_TEST_HPP__

#include "gcl/test/event.hpp"
#include "gcl/test/container/polymorphic_vector.hpp"
#include "gcl/test/type_info.hpp"

#include <type_traits>
#include <iomanip>

// todo : type_trait : test_component<name>::pass / test_component<name>::fail as constexpr booleans value

namespace gcl
{
	namespace test
	{
		struct fail_exception : std::exception {};

		namespace indent_style
		{
			static const size_t default_name_w = 40;
		}

		namespace type_traits
		{
			template <typename T, typename = void>
			struct has_pack : std::false_type {};
			template <typename T>
			struct has_pack<T, std::void_t<typename T::pack_t>> : std::true_type {};

			template <typename T, typename = void>
			struct has_proceed : std::false_type {};
			template <typename T>
			struct has_proceed<T, std::void_t<decltype(&T::proceed)>> : std::true_type {};
		}

		template
		<
			class ...tunits,
			template <typename ... tunits> class pack
		>
		static void proceed_one(pack<tunits...> &&)
		{
			using test_t = std::function<void()>;

			test_t tunit_list[] = { tunits::proceed ... };
			for (auto & unit : tunit_list)
			{
				std::cout << "[+] " << typeid(decltype(unit)).name() << std::endl;
				//unit();
			}
		}

		template <template <class...> class pack, class subcomponent_t, class ... T>
		static void test_subcomponents(pack<subcomponent_t, T...>)
		{
			test_subcomponents(pack<subcomponent_t>{});
			test_subcomponents(pack<T...>{});
		}

		template <template <class...> class pack, class subcomponent_t>
		static void test_subcomponents/*<pack, subcomponent_t>*/(pack<subcomponent_t>)
		{
			auto subcomponent_name = typeid(subcomponent_t).name();
			std::cout << " |- : " << std::left << std::setw(indent_style::default_name_w) << subcomponent_name << " : ";
			try
			{
				//subcomponent_t::proceed();
				std::cout << "[PASSED]";
			}
			catch (const fail_exception & ex)
			{
				std::cout << "[FAILED]";
				std::cout << "    : " << ex.what();
			}
			catch (const std::exception & ex)
			{
				std::cout << "[CRASHED]";
				std::cout << "    : " << ex.what();
			}
			catch (...)
			{
				std::cout << "[CRASHED]";
				std::cout << "    : FATAL_ERROR";
			}
			std::cout << std::endl;
		}

		template <class component_t>
		struct component_is
		{
			template <bool has_pack = type_traits::has_pack<component_t>::value>
			static void test()
			{
				static_assert(type_traits::has_pack<component_t>::value, "component test pack is mandatory");
				test_subcomponents(component_t::pack_t{});
			}
			template <>
			static void test<false>()
			{
				std::cout << " |- : [No test pack detected]";
			}
		};

		template <typename component_t>
		static void test_component()
		{
			auto component_name = typeid(component_t).name();

			std::cout << "[+] : " << std::left << std::setw(indent_style::default_name_w) << component_name << std::endl;

			component_is<component_t>::test();

			std::cout << std::endl;
		}

		static void proceed()
		{
			// todo : test::type return tunits... (test::type::test1, test::type::test2, etc...)
			// proceed_one(gcl::test::event::pack_t {});
			struct toto {};
			test_component<toto>();
			test_component<gcl::test::event>();

			// gcl::test::event::experimental::

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

#endif // GCL_TEST_HPP__