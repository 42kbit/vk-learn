# This should build glfw

# OS Dependent

GLFW_LIB_NAME_WCARD	:=libglfw
GLFW_LIB_NAME		:=libglfw.so

GLFW_LIB_DIR	:=$(bd)
GLFW_LIB	:=$(GLFW_LIB_DIR)/$(GLFW_LIB_NAME)

GLFW_CMAKE_DIR	:=$(bd)/cmake-tmp
GLFW_CMAKE_LIB	:=$(GLFW_CMAKE_DIR)/src

GLFW_INCLUDE_DIR	:=$(GLFW_DIR)/include

TGTS_$(d)	:=$(GLFW_LIB)
$(call append, TGT_LIB, $(TGTS_$(d)))

$(d): $(TGTS_$(d))
$(TGTS_$(d)):
	cmake -S $(GLFW_DIR)/glfw -B $(GLFW_CMAKE_DIR) -D BUILD_SHARED_LIBS=ON 
	cmake --build $(GLFW_CMAKE_DIR)
	cp $(GLFW_CMAKE_LIB)/$(GLFW_LIB_NAME_WCARD)* $(GLFW_LIB_DIR)