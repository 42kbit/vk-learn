#ifndef __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GOPTIONAL_H
#define __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GOPTIONAL_H

#include <glib.h>

typedef struct GOptional {
	gboolean is_set;
} GOptional;

/* Usage example:
 * struct somevalue {
 * 	int value;
 *	struct GOptional optional;
 * };
 * 
 * void somevalue_set (struct GOptional* _p,
 *		       void* _udata)
 * {
 * 	struct somevalue* p = G_OFFSET_OF (_p, struct somevalue, optional);
 * 	VCOPY (udata, int*, _udata);
 * 	p->value = *udata;
 * }
 */

#define G_OPTIONAL_GENCB(type, cbname, v_member, o_member)	\
void								\
cbname (GOptional*	  _p,					\
	gpointer	  _udata)				\
{								\
	type* p;						\
	p = G_OFFSET_OF (_p, type, o_member);			\
	VCOPY (udata, type*, _udata);				\
	p->v_member = *(typeof(p->v_member)*)udata;		\
}

void
g_optional_set (GOptional*,
		void (*set_callback) (
			struct GOptional* _p,
			void*		  _udata
		),
		void*  _udata);

void
g_optional_init (GOptional* p);

#define g_optional_term(p) (void)0;

static inline gboolean
g_optional_is_set (struct GOptional* p) {
	return p->is_set;
}

#endif /* __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GOPTIONAL_H */
