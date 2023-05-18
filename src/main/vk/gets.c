#include <glib.h>
#include <glibext/glibext.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

GArray* vk_get_required_ext (const char * const * exts, guint nexts)
{
	GArray* p;
	const char** glfw_exts;
	guint glfw_n;

	glfw_exts = glfwGetRequiredInstanceExtensions (&glfw_n);
	
	const guint exts_total = glfw_n + nexts;
	
	if (G_UNLIKELY (exts_total == 0)) {
		return NULL;
	}

	p = g_array_sized_new (FALSE, TRUE, sizeof (const char*), exts_total);
	g_array_append_vals (p, glfw_exts, glfw_n);
	g_array_append_vals (p, exts, nexts);

	ge_array_traverse (p, ge_atcb_remove_dups, NULL);
	return p;
}

GArray*
vk_get_queue_family_props (VkPhysicalDevice physdev)
{
	GArray* p;
	guint nfamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties (physdev,
						  &nfamilies,
						  NULL);
	if (nfamilies == 0)
		return NULL;
	p = g_array_sized_new (FALSE, FALSE, sizeof (VkQueueFamilyProperties), nfamilies);
	g_array_set_size (p, nfamilies);
	vkGetPhysicalDeviceQueueFamilyProperties (physdev, &nfamilies,
						  (VkQueueFamilyProperties*) p->data);
	return p;
}