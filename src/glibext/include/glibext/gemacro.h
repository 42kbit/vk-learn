#ifndef __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_MACRO_H
#define __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_MACRO_H

/* Variable copy - a shortcut to write this:
 * VCOPY (sn, struct very_long_struct_name*, _p);
 * instead of this:
 * struct very_long_struct_name * sn = (struct very_long_struct_name*) _p;
*/
#define GE_VCOPY(new, type, from)	\
	type new = (type)from

#define GE_ZEROTYPE(var)		\
	memset (var, 0, sizeof (*var))

#define GE_OFFSET_OF(p, type, member)	\
	((gpointer)((guint8*)(p) - G_STRUCT_OFFSET (type, member)))

/*
 * On error return.
 * If function (argument x) returns negative value (error)
 * return from current function with the same error code, else
 * continue execution.
 *
 * Usage:
 * ERET (func (a, b, c));
*/

#define GE_ERET(x)					\
G_STMT_START {						\
	int __retval = 0;				\
	if ((__retval = x) < 0)				\
		return __retval;			\
} G_STMT_END

#endif /* __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_MACRO_H */
