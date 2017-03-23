#ifndef GCL_TUPLE_INFO__
# define GCL_TUPLE_INFO__

#include <tuple>
#include <memory>

namespace gcl
{
	namespace type_info
	{
		using id_type = uint32_t;

		template <typename ... Ts>
		struct tuple
		{
			tuple() = delete;

			using _Types = std::tuple<Ts...>;

			template <typename T>
			static constexpr inline size_t indexOf(void)
			{
				return indexOf_impl<T, 0, Ts...>();
			}

			template <size_t N>
			using TypeAt = typename std::tuple_element<N, _Types>::type;

		private:
			template <typename T_Needle, size_t It, typename T_It, typename ... T_HayStack>
			static constexpr inline size_t indexOf_impl(void)
			{
				return (std::is_same<T_Needle, T_It>::value ? It : indexOf_impl<T_Needle, It + 1, T_HayStack...>());
			}
			template <typename T_Needle, size_t It>
			static constexpr inline size_t indexOf_impl(void)
			{
				throw std::out_of_range("tuple::indexOf_impl : Not found");
			}
		};

		template <typename T>
		struct id
		{
			static const id_type value; // [todo]::[C++14] : MS compiler do not support variable-template
		};

		template <typename interface_t>
		struct holder final
		{
			using value_type = std::unique_ptr<interface_t>;

			holder() = delete;
			holder(holder &) = default;
			holder(holder &&) = default;

			holder(id_type _id, value_type && ptr)
				: value(std::move(ptr))
				, id(id)
			{}

			template <typename concret_t>
			holder(std::unique_ptr<concret_t> && ptr)
				: value(std::forward<std::unique_ptr<concret_t>>(ptr))
				, id(type_info::id<concret_t>::value)
			{
				check_<concret_t>();
			}
			template <typename concret_t>
			holder(concret_t * ptr)
				: value(std::move(ptr))
				, id(type_info::id<concret_t>::value)
			{
				check_<concret_t>();
			}

			const id_type id;
			const value_type value;

		private :
			template <typename concret_t>
			static constexpr void check_()
			{
				static_assert(!std::is_same<interface_t, concret_t>::value, "interface and concrete types are the same");
				static_assert(std::is_base_of<interface_t, concret_t>::value, "interface_t is not base of concrete_t");
			}
		};
	}

#pragma warning (disable : 4311 4302)
	template <typename T>
	const type_info::id_type type_info::id<T>::value = reinterpret_cast<type_info::id_type>(&(type_info::id<T>::value));
#pragma warning (default : 4311 4302)
}

#endif // GCL_TUPLE_INFO__
