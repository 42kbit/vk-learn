#ifndef __H_SRC_INCLUDE_ZTARRAY_H
#define __H_SRC_INCLUDE_ZTARRAY_H

#include <stddef.h>

static inline ptrdiff_t count_ztarray_len (void** p) {
	void** iter = p;
	for (; *iter != NULL; iter++);
	return (iter - p);
}

#endif /* __H_SRC_INCLUDE_ZTARRAY_H */
