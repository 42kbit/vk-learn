#include <glib.h>
#include <string.h>

gboolean ge_arraytcb_remove_duplicates (GArray* arr, guint idx,
					gpointer udata)
{
	for (guint i = 0; i < arr->len; i++) {
		if (idx == i)
			continue; /* Do not compare self to self */
		int cmpres = 0;
		gchar* p1 = NULL;
		gchar* p2 = NULL;
		const guint esize = g_array_get_element_size (arr);

		p1 = arr->data + i   * esize;
		p2 = arr->data + idx * esize;

		cmpres = memcmp (p1, p2, esize);
		if (cmpres == 0) {
			g_array_remove_index_fast (arr, i);
			i--; /* Check this element again */
			continue;
		}
	}
	return TRUE;
}