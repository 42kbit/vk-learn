#include <vk/vk.h>
#include <vk/ldev.h>
#include <ztarray.h>

#define VK_MAX_QUEUES_PER_LDEV 32

int init_vkldev_from_vkpdev (struct vkldev* dst,
			     struct vkpdev* pdev,
			     int* 	    idxs,    /* Array of indexes, identifying queues */
			     float*	    prios,   /* Array of priorities, with respect to idxs array */
			     guint          nqueues  /* Number of elements in both arrays (should be equal) */)
{
	g_assert (nqueues <= VK_MAX_QUEUES_PER_LDEV);

	VkResult result = VK_SUCCESS;
	VkDeviceQueueCreateInfo queues [VK_MAX_QUEUES_PER_LDEV] = {0};
	VkPhysicalDeviceFeatures pdev_features = {0};
	
	for (guint i = 0; i < nqueues; i++) {
		queues[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        	queues[i].queueFamilyIndex = idxs[i];
        	queues[i].queueCount = i + 1;
        	queues[i].pQueuePriorities = &prios[i];
	}

	VkDeviceCreateInfo ldev_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = queues,
		.queueCreateInfoCount = nqueues,
		.pEnabledFeatures = &pdev_features,
#ifndef __VK_VLAYERS_NEEDED
		.enabledLayerCount = 0
#else
		.ppEnabledLayerNames = vkapp_required_vlayers,
		.enabledLayerCount = count_ztarray_len ((void**)vkapp_required_vlayers)
#endif
	};
	
	result = vkCreateDevice (pdev->pdev, &ldev_create_info, NULL, &dst->ldev);
	g_assert (result == VK_SUCCESS);
	if (result != VK_SUCCESS)
		return -EINVAL;
	return 0;
}

int term_vkldev (struct vkldev* p) {
	vkDestroyDevice (p->ldev, NULL);
	return 0;
}