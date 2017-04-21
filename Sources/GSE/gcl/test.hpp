#ifndef GCL_TEST_HPP__
# define GCL_TEST_HPP__

#include "gcl/preprocessor.h"

#include <type_traits>
#include <iomanip>
#include <sstream>
#include <functional>

// todo : static SFINAE tests
// type_trait : test_component<name>::pass / test_component<name>::fail as constexpr booleans value

namespace gcl
{
	namespace test
	{
		struct fail_exception : std::exception
		{
			fail_exception(const std::string & msg)
				: std::exception(msg.c_str())
			{}
			fail_exception(const char* msg)
				: std::exception(msg)
			{}
		};

		// todo : custom exception stack/pack ?

		struct check
		{
			GCL_PREPROCESSOR__NOT_INSTANTIABLE(check);

			template <typename T>
			static inline void expect(const T & to_test, const std::string & user_msg)
			{
				if (!to_test())
					throw fail_exception(user_msg);
			}
			template <>
			static inline void expect<bool>(const bool & test_result, const std::string & user_msg)
			{
				if (!test_result)
					throw fail_exception(user_msg);
			}

			template <typename T>
			static inline void expect(const std::string & file, const std::size_t line, const std::string & func, const T & to_test, const std::string & user_msg = "")
			{
				expect(to_test, get_where(file, line, func) + (user_msg.length() == 0 ? "" : " : ") + user_msg);
			}

#define GCL_TEST__EXPECT(...) gcl::test::check::expect(__FILE__, __LINE__, __func__, ##__VA_ARGS__)

		private:
			static inline std::string get_where(const std::string & file, const std::size_t line, const std::string & func)
			{
				auto filepos = file.find_last_of("\\/");
				std::string filename{ filepos == std::string::npos ? file : file.substr(filepos + 1) };
				return std::string{ "in " + filename + ":" + std::to_string(line) + ", at function \"" + func + "\"" };
			}
		};

		namespace indent_style
		{
			static const size_t default_name_w = 50;
			static const size_t default_margin_w = 3;
			static const size_t token_w = 4;

			template <std::size_t preindent_w>
			struct log_t
			{
				GCL_PREPROCESSOR__NOT_INSTANTIABLE(log_t);

				static void print(const char (&token)[token_w], const std::string & label, const std::string & value = "", std::ostream & os = std::cout)
				{
					static const std::size_t preindent_width_v = default_margin_w * preindent_w;
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
						<< std::setw(msg_width_v) << label_v << "   "
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

		template <class component_t, std::size_t preindent_w = 0>
		struct component
		{
			GCL_PREPROCESSOR__NOT_INSTANTIABLE(component);

			using type = typename component<component_t, preindent_w>;
			using log_t = typename indent_style::log_t<preindent_w>;

			static void test()
			{
				auto component_name = typeid(component_t).name();

				log_t::print("[+]", component_name);

				component<component_t>::test_impl();
				std::cout << std::endl;
			}

		// protected: // until VS2015 frienship issue is not fix

			// 1 - has test pack ? proceed : skip
			template <bool has_pack = type_traits::has_pack<component_t>::value>
			static void test_impl()
			{
				static_assert(type_traits::has_pack<component_t>::value, "component test pack is mandatory");
				std::cout << std::endl;
				test_subcomponents(component_t::pack_t{});
			}
			template <>
			static void test_impl<false>()
			{
				std::cout << "[SKIP]" << std::endl;
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
			GCL_PREPROCESSOR__NOT_INSTANTIABLE(subcomponent);

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
					auto output = do_proceed(subcomponent_t::proceed);
					std::cout << "[PASSED]";

					if (output.length() != 0)
					{
						std::cout << std::endl;
						indent_style::log_t<preindent_w + 1>::print(" >>", output);
					}

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
				log_t::print(" |-", subcomponent_name); // [IS_PACK]
				component<subcomponent_t, preindent_w + 1>::test_impl();
			}

		protected:
			template <typename T, typename ... P>
			static std::string do_proceed(T(*func)(P...))
			{
				auto ret = func();
				std::ostringstream oss;
				oss << std::left << std::boolalpha << ret;
				return oss.str();
			}
			template <typename ... P>
			static std::string do_proceed(void(*func)(P...))
			{
				func();
				return std::string{};
			}
		};
	}
}

#endif // GCL_TEST_HPP__