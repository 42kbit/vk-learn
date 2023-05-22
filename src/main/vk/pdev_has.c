#include <glib.h>
#include <glibext/glibext.h>
#include <vk/pdev.h>
#include <vk/surface_caps.h>
#include <string.h>
#include <vk/surface_caps.h>

int vkpdev_has_exts (struct vkpdev* p,
		     const char * const * exts,
		     guint nexts)
{
	int cmpres = 0;
	for (guint i = 0; i < nexts; i++) {
		const char * iter = exts [i];
		if (!vkpdev_has_ext (p, iter))
			return 0;
	}
	return 1;
}

int vkpdev_has_swapchain_support (struct vkpdev* _p, struct vksurface_khr* _surface)
{
	guint32 npmodes = vksurface_caps_khr_pmodes_cnt (_p, _surface);
	guint32 sfmts   = vksurface_caps_khr_sfmts_cnt  (_p, _surface);
	return npmodes != 0 && sfmts != 0;
}