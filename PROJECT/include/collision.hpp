#ifndef GSE_COLLISION_HPP__
# define GSE_COLLISION_HPP__

# include "gcl/type_info.hpp"

namespace gse
{
	namespace types
	{
		struct hitbox
		{

		};
		template <typename T>
		struct hitbox_impl : T
		{
			static const gcl::type_info::id_type gcl::type_info::id<T>;
		};
	}
	namespace algorythm
	{
		struct AABB final
		{
			static bool	IsCollision(const HitBox & a, const HitBox & b)
			{
				return (
					a.GetPosition().first < b.GetPosition().first + b.GetSize().first
					&& a.GetPosition().first + a.GetSize().first > b.GetPosition().first
					&& a.GetPosition().second < b.GetPosition().second + b.GetSize().second
					&& a.GetSize().second + a.GetPosition().second > b.GetPosition().second
					);
			}
		};
	}
	namespace engine
	{
		struct quadtree
		{

		};
	}
}

#endif // GSE_COLLISION_HPP__
