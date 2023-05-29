#include <vk/app.h>


int
main(void)
{
	struct vkapp* vkapp;
	GError*	      ecode = NULL;

	init_vkapp (&vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan init error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
		goto app_term;
	}

	vkapp_enter_mainloop (vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan mainloop error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
		goto app_term;
	}
	
app_term:;
	term_vkapp (vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan termination error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
	}
	return 0;
}