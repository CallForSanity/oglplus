/*
 *  .file oglplus/enums/primitive_type_range.ipp
 *
 *  Automatically generated header file. DO NOT modify manually,
 *  edit 'source/enums/primitive_type.txt' instead.
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

OGLPLUS_LIB_FUNC aux::CastIterRange<
	const GLenum*,
	PrimitiveType
> EnumValueRange(PrimitiveType*)
OGLPLUS_NOEXCEPT(true)
#if (!OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)) && \
	!defined(OGLPLUS_IMPL_EVN_PRIMITIVETYPE)
#define OGLPLUS_IMPL_EVN_PRIMITIVETYPE
{
static const GLenum _values[] = {
#if defined GL_POINTS
GL_POINTS,
#endif
#if defined GL_LINE_STRIP
GL_LINE_STRIP,
#endif
#if defined GL_LINE_LOOP
GL_LINE_LOOP,
#endif
#if defined GL_LINES
GL_LINES,
#endif
#if defined GL_TRIANGLE_STRIP
GL_TRIANGLE_STRIP,
#endif
#if defined GL_TRIANGLE_FAN
GL_TRIANGLE_FAN,
#endif
#if defined GL_TRIANGLES
GL_TRIANGLES,
#endif
#if defined GL_LINES_ADJACENCY
GL_LINES_ADJACENCY,
#endif
#if defined GL_LINE_STRIP_ADJACENCY
GL_LINE_STRIP_ADJACENCY,
#endif
#if defined GL_TRIANGLES_ADJACENCY
GL_TRIANGLES_ADJACENCY,
#endif
#if defined GL_TRIANGLE_STRIP_ADJACENCY
GL_TRIANGLE_STRIP_ADJACENCY,
#endif
#if defined GL_PATCHES
GL_PATCHES,
#endif
0
};
return aux::CastIterRange<
	const GLenum*,
	PrimitiveType
>(_values, _values+sizeof(_values)/sizeof(_values[0])-1);
}
#else
;
#endif

