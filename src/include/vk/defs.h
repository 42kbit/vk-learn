#ifndef __H_SRC_INCLUDE_VK_DEFS_H
#define __H_SRC_INCLUDE_VK_DEFS_H

#define EVKDEFAULT 42

/*
	PFN_vkVoidFunction _func;
	_func = vkGetInstanceProcAddr (instance, "vkCreateDebugUtilsMessengerEXT");
						  
	if (!_func)
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	VCOPY (func, PFN_vkCreateDebugUtilsMessengerEXT, _func);
	return func (instance, create_info, callbacks, p);
*/

#define VKEXT_GETFN(var, fname, instance)			\
G_STMT_START {							\
	PFN_vkVoidFunction _func;				\
	_func = vkGetInstanceProcAddr (instance, #fname);	\
	var = (PFN_##fname)_func;				\
} G_STMT_END

#endif /* __H_SRC_INCLUDE_VK_DEFS_H */
