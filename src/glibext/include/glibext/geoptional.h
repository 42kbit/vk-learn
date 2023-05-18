#ifndef __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEOPTIONAL_H
#define __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEOPTIONAL_H

#include <glib.h>

typedef struct GEOptional {
	gboolean is_set;
} GEOptional;

/* Usage example:
 * struct somevalue {
 * 	int value;
 *	struct GEOptional optional;
 * };
 * 
 * void somevalue_set (struct GEOptional* _p,
 *		       void* _udata)
 * {
 * 	struct somevalue* p = G_OFFSET_OF (_p, struct somevalue, optional);
 * 	VCOPY (udata, int*, _udata);
 * 	p->value = *udata;
 * }
 */

#define GE_OPTIONAL_GENCB(type, cbname, v_member, o_member)	\
void								\
cbname (GEOptional*	  _p,					\
	gpointer	  _udata)				\
{								\
	type* p;						\
	p = G_OFFSET_OF (_p, type, o_member);			\
	VCOPY (udata, type*, _udata);				\
	p->v_member = *(typeof(p->v_member)*)udata;		\
}

void
ge_optional_set (GEOptional*,
		void (*set_callback) (
			struct GEOptional* _p,
			void*		  _udata
		),
		void*  _udata);

static inline void g_optional_init (GEOptional* p) {
	p->is_set = FALSE;
}

#define ge_optional_term(p) (void)0;

static inline gboolean
ge_optional_is_set (struct GEOptional* p) {
	return p->is_set;
}





/* Faster way of setting optional. See pdev.c */
#define GE_GEN_SETOPT(type, fname, m_value, m_opt)	\
void fname (type* p, glib_typeof (p->m_value) newval) {	\
	p->m_value = newval;				\
	p->m_opt = TRUE;				\
}

#define GE_GEN_GETOPT(type, fname, m_opt)			\
gboolean fname (type* p) {					\
	return p->m_opt;					\
}

#define GE_GEN_INITOPT(type, fname, m_opt)			\
void fname (type* p) {						\
	p->m_opt = FALSE;					\
}

#define GE_GEN_GETVAL(type, fname, m_value)			\
glib_typeof (((type*)0)->m_value) fname (type* p) {		\
	return p->m_value;					\
}

#define GE_GEN_OPT(type, fname, m_value, m_opt)					\
	static inline GE_GEN_INITOPT(type, initopt_##fname, m_opt);		\
	static inline GE_GEN_SETOPT (type, setopt_##fname, m_value, m_opt);	\
	static inline GE_GEN_GETOPT (type, getopt_##fname, m_opt);		\
	static inline GE_GEN_GETVAL (type, getval_##fname, m_value);

#endif /* __H_SRC_GLIBEXT_INCLUDE_GLIBEXT_GEOPTIONAL_H */
