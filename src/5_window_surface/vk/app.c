
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
#ifdef __VK_VLAYERS_NEEDED
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
#endif
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

	
	result = init_vkinstance (&p->instance, &vk_app_info, NULL, p->exts, &p->messenger);
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

	retcode = get_vkpdevs_from_instance (&p->pdevs, &p->instance);
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
	int   idxs[1] = { p->pd_used->qfamily.gfamily.idx };
	float prio[1] = { 1.f };
	ERET(init_vkldev_from_vkpdev (&p->ld_used, p->pd_used, idxs,
				      prio, G_N_ELEMENTS (idxs)));
	return 0;
}

static int init_vkapp_queues (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_gfamily_idx (p->pd_used);

	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Graphics queue not found!");
		return -EINVAL;
	}
	ERET (get_vkqueue_from_vkldev (&p->gqueue, &p->ld_used, gidx, 0));
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

void term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
#ifdef DEBUG
	result = term_vkmessenger (&p->messenger, NULL);
	g_assert (result == VK_SUCCESS);
#endif
#ifdef __VK_VLAYERS_NEEDED
	g_array_free (p->vlayers, TRUE);
#endif
	term_vkpdevs (p->pdevs);
	term_vkldev  (&p->ld_used);
	term_vksurface_khr (&p->surface);

	term_vkinstance (&p->instance);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}