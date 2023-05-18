#include <glibext/geoptional.h>

void
ge_optional_set (struct GEOptional* optional,
		void (*set_callback) (
		        struct GEOptional* _p,
		        void* 	       _udata
		),
		void*  _udata)
{
	optional->is_set = TRUE;
	set_callback (optional, _udata);
}
