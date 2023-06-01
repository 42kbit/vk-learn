#ifndef __H_SRC_MAIN_INCLUDE_VK_DEFS_H
#define __H_SRC_MAIN_INCLUDE_VK_DEFS_H

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

#define _VKEXT_GEN_LDEV_WRAPPER_INIT(sname, cinfo_type, fcrt_name)			\
VkResult init_##sname	      (struct sname* p,						\
			       struct vkldev* ldev,					\
			       const cinfo_type* cinfo,					\
			       const VkAllocationCallbacks *alloc_cbs)			\
{											\
	p->ldev = ldev;									\
	p->alloc_cbs = alloc_cbs;							\
	return fcrt_name (ldev->core, cinfo, alloc_cbs, &p->core);			\
}

#define _VKEXT_GEN_LDEV_WRAPPER_TERM(sname, frmv_name)					\
void term_##sname (struct sname* p)							\
{											\
	frmv_name (p->ldev->core, p->core, p->alloc_cbs);				\
}

#define VKEXT_GEN_LDEV_WRAPPER_INIT(sname, vkobj_name) \
	_VKEXT_GEN_LDEV_WRAPPER_INIT (sname, Vk##vkobj_name##CreateInfo, vkCreate##vkobj_name)

#define VKEXT_GEN_LDEV_WRAPPER_TERM(sname, vkobj_name) \
	_VKEXT_GEN_LDEV_WRAPPER_TERM (sname, vkDestroy##vkobj_name)

#define VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS(sname, vkobj_name)		\
	static inline VKEXT_GEN_LDEV_WRAPPER_INIT(sname, vkobj_name)	\
	static inline VKEXT_GEN_LDEV_WRAPPER_TERM(sname, vkobj_name)



#endif /* __H_SRC_MAIN_INCLUDE_VK_DEFS_H */
