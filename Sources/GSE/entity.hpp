#ifndef GSE_ENTITY_HPP__
# define GSE_ENTITY_HPP__

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

#endif // GSE_ENTITY_HPP__