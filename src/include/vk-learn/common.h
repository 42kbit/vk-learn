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

#define ZEROTYPE(var)		\
	memset (var, 0, sizeof (*var))

static inline void
g_array_remove_by_predicate (GArray* p,
			     gboolean (*pred) (
			             GArray*  arr,
				     guint    idx,
				     gpointer _udata
			     ),
			     gpointer _udata)
{
	for (guint i = 0; i < p->len; i++) {
		if (pred (p, i, _udata) == FALSE) {
			g_array_remove_index_fast (p, i);
		}
	}
}

static inline void
g_array_traverse (GArray* arr,
		  gboolean (*func) (
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
