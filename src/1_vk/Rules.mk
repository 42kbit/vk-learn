ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/main.o

TGTS_$(d)	:=$(bd)/main

TGT_BIN		:=$(TGT_BIN) $(TGTS_$(d))

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-I$(GLFW_INCLUDE_DIR)	\
		-I$(CGLM_INCLUDE_DIR)	\
		-I$(INCLUDE_TOP)	\
		-I$(GLIB_INCLUDE_DIR)

LF_$(TGTS_$(d))		:=		\
		$(LF_$(d))		\
		-L$(GLFW_LIB_DIR)	\
		-L$(CGLM_LIB_DIR)	\
		-L$(GLIB_LIB_DIR)	\
		-lglfw			\
		-lcglm			\
		-lglib-2.0		\
		-lvulkan

$(COBJS_$(d)): $(CGLM_LIB) $(GLIB_LIB) $(GLFW_LIB)

$(d): $(TGTS_$(d))
$(TGTS_$(d)): $(CGLM_LIB) $(GLIB_LIB) $(GLFW_LIB) $(OBJS_$(d))
	$(L_LINK)