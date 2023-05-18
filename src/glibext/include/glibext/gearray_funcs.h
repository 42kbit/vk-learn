#ifndef __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEARRAY_FUNCS_H
#define __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEARRAY_FUNCS_H

#include <glib.h>

typedef gboolean (*GEBreakTraverseFunc) (GArray* arr, guint idx, gpointer udata);

static inline void ge_array_traverse (GArray* arr,
		  		      GEBreakTraverseFunc func,
		  		      gpointer user_data)
{
	guint i;
	for (i = 0; i < arr->len; i++) {
		if (func (arr, i, user_data) != TRUE) {
			break;
		}
	}
}

/* Defines some usefull callbacks. */
/* tcb  - Traverse CallBack */
/* atcb - Array Traverse CallBack*/

struct ge_atcb_args_remove_dups {
	GCompareDataFunc cmpfunc;
	gpointer cmpfunc_data;
};
gboolean ge_atcb_remove_dups (GArray* arr, guint idx, gpointer udata);

#endif /* __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEARRAY_FUNCS_H */
