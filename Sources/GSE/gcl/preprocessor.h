#ifndef GCL_PREPROCESSOR_H__
# define GCL_PREPROCESSOR_H__

#define GCL_PREPROCESSOR__NOT_INSTANTIABLE(type) \
	type() = delete;				\
	type(const type &) = delete;	\
	type(type &&) = delete; 

#endif GCL_PREPROCESSOR_H__