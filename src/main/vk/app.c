
#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/vlayers.h>
#include <vk/app.h>
#include <vk/instance.h>
#include <vk/surface.h>
#include <vk/messenger.h>
#include <vk/common.h>
#include <vk/defs.h>
#include <vk/gets.h>

#include <ztarray.h>
#include <isdefined.h>

static struct vkapp __vkapp; /* Shall not be accessed directly */

static int init_vkapp_exts (struct vkapp* p, GError** e)
{
	p->exts = vk_get_required_ext ();
	return 0;
}

static int init_vkapp_glfw (struct vkapp* p, GError** e)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	return 0;
}

static int init_vkapp_glfw_window (struct vkapp* p, GError** e)
{
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);

	p->glfw_window = glfwCreateWindow (640, 480, "Hello Vulkan!", NULL, NULL);
	if (!p->glfw_window) {
		g_set_error (e, EVKDEFAULT, 1, "Failed to create GLFW window");
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(p->glfw_window);
	return 0;
}

static int init_vkapp_vlayers (struct vkapp* p, GError** e)
{
	if (!IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		p->vlayers = NULL;
		return 0;
	}

	p->vlayers = vk_get_vlayers ();
	const char* failed_at = NULL;
	if (!vkvlayers_matches_name (p->vlayers,
				    (const char**)vkapp_required_vlayers,
				    &failed_at))
	{
		g_set_error (e, EVKDEFAULT, ENODEV,
			     "Missing validation layers. Failed at: %s\n",
			     failed_at);
		return -ENODEV;
	}
	return 0;
}

static int init_vkapp_instance (struct vkapp* p, GError** e)
{
	VkResult result;
	VkApplicationInfo vk_app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello, Triangle!",
		.applicationVersion = VK_MAKE_VERSION (1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION (1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	
#ifdef DEBUG
	result = init_vkinstance (&p->instance, &vk_app_info, NULL, p->exts, &p->messenger);
#else  /* DEBUG */
	result = init_vkinstance (&p->instance, &vk_app_info, NULL, p->exts, NULL);
#endif /* DEBUG */
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed to create vulkan insatnce, VkResult: %d", result);
		return -1;
	}
	return 0;
}


static int init_vkapp_pdevs (struct vkapp* p, GError** e)
{
	guint all_devs = 0;
	int retcode;

	retcode = init_vkpdevs (&p->pdevs, &p->instance, &p->surface);
	if (retcode < 0) {
		g_set_error (e, EVKDEFAULT, ENODEV, "Vulkan devices not found");
		return -ENODEV;
	}

	retcode = get_vkpdevs_best (&p->pd_used, p->pdevs);

	if (!p->pd_used) {
		g_set_error (e, EVKDEFAULT, ENODEV,
			     "Found %d Vulkan devices, but none are suitable",
			     all_devs);
		return -ENODEV;
	}
	return 0;
}

static inline int init_vkapp_ldevs (struct vkapp* p, GError** e)
{
	GArray* idxs = g_array_sized_new (FALSE, FALSE, sizeof (int), 8);
	g_array_insert_val (idxs, 0, p->pd_used->qfamily.gfamily.idx);
	g_array_insert_val (idxs, 1, p->pd_used->qfamily.pfamily.idx);

	/* May be duplicates, so remove them. */
	ge_array_traverse  (idxs, ge_arraytcb_remove_duplicates, NULL); 

	float prio[8] = { 1.f };
	ERET(init_vkldev_from_vkpdev (&p->ld_used, p->pd_used, (int*)idxs->data,
				      prio, idxs->len));
	g_array_free (idxs, TRUE);
	return 0;
}

static int __init_vkapp_queues_graphics (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_gfamily_idx (p->pd_used);

	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Graphics queue not found!");
		return -EINVAL;
	}
	
	ERET (get_vkqueue_from_vkldev (&p->gqueue, &p->ld_used, gidx, 0));
	return 0;
}

static int __init_vkapp_queues_presentation (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_pfamily_idx (p->pd_used);
	
	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Presentation queue not found!");
		return -EINVAL;
	}
	
	ERET (get_vkqueue_from_vkldev (&p->pqueue, &p->ld_used, gidx, 0));
	return 0;
}

static int init_vkapp_queues (struct vkapp* p, GError** e)
{
	ERET (__init_vkapp_queues_graphics (p, e));
	ERET (__init_vkapp_queues_presentation (p, e));
	return 0;
}

static int init_vkapp_surface (struct vkapp* p, GError** e)
{
	VkResult result;
	result = init_vksurface_khr (&p->surface, &p->instance,
				     p->glfw_window, NULL);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Failed to init Vulkan Surface, VkResult: %d", result);
		return -1;
	}
	return 0;
}

int init_vkapp (struct vkapp** dst, GError** e)
{
	struct vkapp* p = &__vkapp;
	*dst = p;
	ZEROTYPE (p);

	ERET (init_vkapp_glfw	     (p, e));
	ERET (init_vkapp_glfw_window (p, e));
	ERET (init_vkapp_exts	     (p, e));
	ERET (init_vkapp_vlayers  (p, e));
	ERET (init_vkapp_instance (p, e));
	ERET (init_vkapp_surface  (p, e));
	ERET (init_vkapp_pdevs    (p, e));
	ERET (init_vkapp_ldevs    (p, e));
	ERET (init_vkapp_queues   (p, e));
	return 0;
}

static inline void __term_vkmessenger_if_debug (struct vkapp* p)
{
	if (!IS_DEFINED (DEBUG))
		return;
	VkResult result;
	result = term_vkmessenger (&p->messenger, NULL);
	g_assert (result == VK_SUCCESS);
}

void term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
	
	__term_vkmessenger_if_debug (p);

	if (IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		g_array_free (p->vlayers, TRUE);
	}
	term_vkpdevs (p->pdevs);
	term_vkldev  (&p->ld_used);
	term_vksurface_khr (&p->surface);

	term_vkinstance (&p->instance);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}