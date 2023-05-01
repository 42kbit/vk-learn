#include <glibext/goptional.h>

void
g_optional_set (struct GOptional* optional,
		void (*set_callback) (
		        struct GOptional* _p,
		        void* 	       _udata
		),
		void*  _udata)
{
	optional->is_set = TRUE;
	set_callback (optional, _udata);
}

void
g_optional_init (GOptional* p) {
	p->is_set = FALSE;
}