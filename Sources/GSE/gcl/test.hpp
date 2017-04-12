#ifndef GCL_TEST_HPP__
# define GCL_TEST_HPP__

#include "gcl/preprocessor.h"

#include <type_traits>
#include <iomanip>
#include <sstream>

// todo : type_trait : test_component<name>::pass / test_component<name>::fail as constexpr booleans value

namespace gcl
{
	namespace test
	{
		struct fail_exception : std::exception {};

		namespace indent_style
		{
			static const size_t default_name_w = 50;

			template <std::size_t preindent_w>
			struct log_t
			{
				static void print(const char (&token)[4], const std::string & label, const std::string & value = "", std::ostream & os = std::cout)
				{
					static const std::size_t preindent_width_v = 3 * preindent_w;
					static const std::size_t msg_width_v = default_name_w - preindent_width_v;

					// [name_too_long...]
					std::string label_v;
					if (label.length() > msg_width_v)
					{
						label_v = label.substr(0, msg_width_v - 3);
						label_v += "...";
					}
					else label_v = label;

					os
						<< std::left
						<< std::setw(preindent_width_v) << "" << token << " : "
						<< std::setw(msg_width_v) << label_v << " : "
						;

					if (value.length() == 0)
						os.flush();
					else
						os << value << std::endl;
				}
			};
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

		//template
		//<
		//	class ...tunits,
		//	template <typename ... tunits> class pack
		//>
		//static void proceed_one(pack<tunits...> &&)
		//{
		//	using test_t = std::function<void()>;
		//	test_t tunit_list[] = { tunits::proceed ... };
		//	for (auto & unit : tunit_list)
		//	{
		//		std::cout << "[+] " << typeid(decltype(unit)).name() << std::endl;
		//		//unit();
		//	}
		//}
		
		template <class component_t, std::size_t preindent_w = 0>
		struct component
		{
			GCL_PREPROCESSOR__NOT_INSTANTIABLE(component);

			using type = typename component<component_t, preindent_w>;
			using log_t = typename indent_style::log_t<preindent_w>;

			static void test()
			{
				auto component_name = typeid(component_t).name();

				log_t::print("[+]", component_name, " ");

				component<component_t>::test_impl();
				std::cout << std::endl;
			}

		// protected: // until VS2015 frienship issue is not fix

			// 1 - has test pack ? proceed : skip
			template <bool has_pack = type_traits::has_pack<component_t>::value>
			static void test_impl()
			{
				static_assert(type_traits::has_pack<component_t>::value, "component test pack is mandatory");
				test_subcomponents(component_t::pack_t{});
			}
			template <>
			static void test_impl<false>()
			{
				log_t::print(" |-", "[No test pack detected]");
			}
			// 2 - foreach test in pack
			//     has proceed ? do test : "not implemented"
			//                             or is pack
			template <template <class...> class pack, class subcomponent_t, class ... T>
			static void test_subcomponents(pack<subcomponent_t, T...>)
			{
				test_subcomponents(pack<subcomponent_t>{});
				test_subcomponents(pack<T...>{});
			}
			template <template <class...> class pack, class subcomponent_t>
			static void test_subcomponents(pack<subcomponent_t>)
			{
				subcomponent<subcomponent_t, preindent_w>::proceed();
			}
		};

		template <typename subcomponent_t, size_t preindent_w = 0>
		struct subcomponent
		{
			using log_t = typename indent_style::log_t<preindent_w>;
			/*template <bool b>
			friend void component<subcomponent_t>::test_impl<b>();*/

			template <bool has_proceed = type_traits::has_proceed<subcomponent_t>::value>
			static void proceed()
			{
				std::string subcomponent_name = typeid(subcomponent_t).name();
				subcomponent_name = subcomponent_name.substr(subcomponent_name.rfind("::") + 2);
				log_t::print(" |-", subcomponent_name);

				try
				{
					auto func = subcomponent_t::proceed;
					// subcomponent_t::proceed(); // todo : uncomment
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
			template <>
			static void proceed<false>()
			{
				std::string subcomponent_name = typeid(subcomponent_t).name();
				subcomponent_name = subcomponent_name.substr(subcomponent_name.rfind("::") + 2);
				log_t::print(" |-", subcomponent_name, "[IS_PACK]");
				component<subcomponent_t, preindent_w + 1>::test_impl();
			}
		};
	}
}

#endif // GCL_TEST_HPP__