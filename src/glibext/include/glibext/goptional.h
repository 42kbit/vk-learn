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





/* Faster way of setting optional. See pdev.c */
#define G_GEN_SETOPT(type, fname, m_value, m_opt)	\
void fname (type* p, glib_typeof (p->m_value) newval) {	\
	p->m_value = newval;				\
	p->m_opt = TRUE;				\
}

#define G_GEN_GETOPT(type, fname, m_opt)			\
gboolean fname (type* p) {					\
	return p->m_opt;					\
}

#define G_GEN_INITOPT(type, fname, m_opt)			\
void fname (type* p) {						\
	p->m_opt = FALSE;					\
}

#define G_GEN_OPT(type, fname, m_value, m_opt)					\
	static inline G_GEN_INITOPT(type, initopt_##fname, m_opt)		\
	static inline G_GEN_SETOPT (type, setopt_##fname, m_value, m_opt);	\
	static inline G_GEN_GETOPT (type, getopt_##fname, m_opt)

#endif /* __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GOPTIONAL_H */
