ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/main.o

TGTS_$(d)	:=$(bd)/main

$(call append, TGT_BIN, $(TGTS_$(d)))

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-I$(GLFW_INCLUDE_DIR)		\
		-I$(CGLM_INCLUDE_DIR)

LF_$(TGTS_$(d))		:=		\
		$(LF_$(d))		\
		-L$(GLFW_LIB_DIR)	\
		-L$(CGLM_LIB_DIR)	\
		-lglfw			\
		-lcglm			\
		-lvulkan

$(d): $(TGTS_$(d))
$(TGTS_$(d)): $(GLFW_LIB) $(OBJS_$(d))
	$(L_LINK)