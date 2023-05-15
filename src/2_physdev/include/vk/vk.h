#ifndef __H_SRC_INCLUDE_VK_VK_H
#define __H_SRC_INCLUDE_VK_VK_H

#ifdef DEBUG
 #define __VK_VLAYERS_NEEDED 1
#else
 #define G_DISABLE_ASSERT 1
#endif
 
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glib.h>
#include <glibext/glibext.h>

#include <vk/common.h>
#include <vk/defs.h>
#include <vk/gets.h> 	/* Vulkan getters (vkEnumerate*) */
#include <vk/cmps.h>

#include <vk/app.h>

#ifdef DEBUG
#include <vk/messenger.h>
#ifdef __VK_VLAYERS_NEEDED
#include <vk/vlayers.h> /* Vulkan validation layers */
#endif /* __VK_VLAYERS_NEEDED */
#endif /* DEBUG */

#include <vk/pdev.h>
#include <vk/ldev.h>

#include <vk/queue.h>

#endif /* __H_SRC_INCLUDE_VK_VK_H */
