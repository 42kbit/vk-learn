# This should build cglm

# OS Dependent

CGLM_LIB_NAME_WCARD	:=libcglm
CGLM_LIB_NAME		:=libcglm.so

CGLM_LIB_DIR	:=$(bd)
CGLM_LIB	:=$(CGLM_LIB_DIR)/$(CGLM_LIB_NAME)

CGLM_CMAKE_DIR	:=$(bd)/cmake-tmp
CGLM_CMAKE_LIB	:=$(CGLM_CMAKE_DIR)/src

CGLM_INCLUDE_DIR	:=$(CGLM_DIR)/include

TGTS_$(d)	:=$(CGLM_LIB)
$(call append, TGT_LIB, $(TGTS_$(d)))

$(d): $(TGTS_$(d))
$(TGTS_$(d)):
	cmake -S $(CGLM_DIR)/cglm -B $(CGLM_CMAKE_DIR) -DCGLM_SHARED=ON 
	cmake --build $(CGLM_CMAKE_DIR)
	cp $(CGLM_CMAKE_DIR)/$(CGLM_LIB_NAME_WCARD)* $(CGLM_LIB_DIR)