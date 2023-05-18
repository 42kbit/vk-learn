#include <glibext/gearray_funcs.h>
#include <glibext/gemacro.h>
#include <glib.h>
#include <string.h>

static gint __ge_cmp_default (gconstpointer p1, const gconstpointer p2, gpointer _udata)
{
	GE_VCOPY (udata, guint*, _udata);
	return memcmp (p1, p2, *udata);
}

gboolean ge_atcb_remove_dups (GArray* arr, guint idx,
			      gpointer _udata)
{
	GE_VCOPY (udata, struct ge_atcb_args_remove_dups*, _udata);
	GCompareDataFunc cmpfunc = __ge_cmp_default;
	gpointer cmpfunc_data = NULL;
	if (udata && udata->cmpfunc)
		cmpfunc = udata->cmpfunc;
	if (udata && udata->cmpfunc_data)
		cmpfunc_data = udata->cmpfunc_data;

	for (guint i = 0; i < arr->len; i++) {
		if (idx == i)
			continue; /* Do not compare self to self */
		int cmpres = 0;
		gchar* p1 = NULL;
		gchar* p2 = NULL;
		guint esize = g_array_get_element_size (arr);

		p1 = arr->data + i   * esize;
		p2 = arr->data + idx * esize;

		cmpfunc_data = &esize;
		cmpres = cmpfunc (p1, p2, cmpfunc_data);
		if (cmpres == 0) {
			g_array_remove_index_fast (arr, i);
			i--; /* Check this element again */
			continue;
		}
	}
	return TRUE;
}