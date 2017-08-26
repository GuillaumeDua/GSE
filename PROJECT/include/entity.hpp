#ifndef GSE_ENTITY_HPP__
# define GSE_ENTITY_HPP__

# include <gcl_cpp/container/polymorphic_vector.hpp>

namespace gse
{
	struct entity
	{
		virtual ~entity() = 0;
		virtual void draw() = 0;
		virtual void behave() = 0;
	};

	template <class trait_t>
	struct entity_impl
	{

	};

	struct entity_manager : gcl::container::polymorphic_vector<entity>
	{
		void update()
		{
			// todo : split by thread chunks
			this->visit([](const auto & elem) { elem->behave(); });
		}
		void draw()
		{
			this->visit([](const auto & elem) { elem->draw(); });
		}
	};
}

#endif // GSE_ENTITY_HPP__