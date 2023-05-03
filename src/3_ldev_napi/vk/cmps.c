#include <vk/vk.h>

gboolean
g_array_cmp_VkLayerProperties_name (GArray*  arr,
				    guint    idx,
				    gpointer _udata)
{
	VCOPY (udata, struct __cmp_VkLayerProperties_name*, _udata);
	VkLayerProperties* current = &g_array_index (arr, VkLayerProperties, idx);

	int cmpres = strcmp (udata->search_for, current->layerName);
	if (cmpres == 0) {
		*udata->dst = current;
		return FALSE;
	}
	return TRUE;
}