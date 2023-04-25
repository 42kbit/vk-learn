#ifndef __H_SRC_INCLUDE_VK_LEARN_COMMON_H
#define __H_SRC_INCLUDE_VK_LEARN_COMMON_H

#include <glib.h>

/* Variable copy - a shortcut to write this:
 * VCOPY (sn, struct very_long_struct_name*, _p);
 * instead of this:
 * struct very_long_struct_name * sn = (struct very_long_struct_name*) _p;
*/
#define VCOPY(new, type, from)	\
	type new = (type)from

static inline void
g_array_traverse (GArray* arr,
		  bool (*func) (
			  GArray*  arr,
			  guint    idx,
			  gpointer user_data
		  ),
		  gpointer user_data)
{
	guint i;
	for (i = 0; i < arr->len; i++) {
		if (func (arr, i, user_data) != TRUE) {
			break;
		}
	}
}

#endif /* __H_SRC_INCLUDE_VK_LEARN_COMMON_H */
