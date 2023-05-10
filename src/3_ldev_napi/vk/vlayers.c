#include <vk/vk.h>
#include <vk/vlayers.h>

#ifdef __VK_VLAYERS_NEEDED

const char* vkapp_required_vlayers[] = {
	"VK_LAYER_KHRONOS_validation",
	NULL
};

/* vk_get_vlayers, is valid, only and only if
   struct vkvlayers contatins only VkLayerProperties.
   else assert will drop error, and function should be rewritten.
*/
G_STATIC_ASSERT (sizeof (struct vkvlayer) == sizeof (VkLayerProperties));

GArray* vk_get_vlayers (void)
{
	VkResult result;
	GArray * vlayers;
	guint    vlayers_cnt;
	/* Assume vkEnumerateInstanceLayerProperties does throw VK_SUCCESS */
	result = vkEnumerateInstanceLayerProperties (&vlayers_cnt, NULL);
	g_assert (result == VK_SUCCESS);
	
	if (vlayers_cnt == 0)
		return NULL;

	vlayers = g_array_sized_new (FALSE, FALSE, sizeof (VkLayerProperties), vlayers_cnt);
	result = vkEnumerateInstanceLayerProperties (&vlayers_cnt, (VkLayerProperties*)vlayers->data);
	g_assert (result == VK_SUCCESS);

	g_array_set_size (vlayers, vlayers_cnt);
	return vlayers;
}

struct __cmp_VkLayerProperties_name {
	VkLayerProperties** dst;
	const char* search_for;
};

gboolean __g_array_cmp_VkLayerProperties_name (GArray*  arr,
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

gboolean vkvlayers_matches_name (GArray*       p,
				 const char**  vlneeded,
				 const char**  vlfailed_on)
{
	VkLayerProperties* found;
	struct __cmp_VkLayerProperties_name args;

	guint i = 0;
	for (i = 0; vlneeded[i] != NULL; i++) {
		found = NULL;
		args.dst = &found;
		args.search_for = vlneeded[i];
		g_array_traverse (p, __g_array_cmp_VkLayerProperties_name, &args);
		if (!found) {
			*vlfailed_on = args.search_for;
			return FALSE;
		}
	}
	return TRUE;
} 

#endif