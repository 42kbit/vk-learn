#include <vk/vk.h>
#ifdef __VK_VLAYERS_NEEDED

const char* vkapp_required_vlayers[] = {
	"VK_LAYER_KHRONOS_validation",
	NULL
};

gboolean
vkapp_matches_vlayers (struct vkapp* p,
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
		g_array_traverse (p->vlayers, g_array_cmp_VkLayerProperties_name, &args);
		if (!found) {
			*vlfailed_on = args.search_for;
			return FALSE;
		}
	}
	return TRUE;
} 
#endif