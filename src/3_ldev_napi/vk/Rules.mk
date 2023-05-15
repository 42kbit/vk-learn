ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/gets.o		\
		  $(od)/vlayers.o	\
		  $(od)/messenger.o	\
		  $(od)/pdev.o		\
		  $(od)/pdev_best.o	\
		  $(od)/ldev.o		\
		  $(od)/app.o

TGT_BIN		:=$(TGT_BIN) $(TGTS_$(d))

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-Wno-unused-parameter	\
		-I$(GLFW_INCLUDE_DIR)	\
		-I$(CGLM_INCLUDE_DIR)	\
		-I$(d)/../include	\
		-I$(GLIB_INCLUDE_DIR)	\
		-I$(GLIBEXT_INCLUDE_DIR)

LF_$(TGTS_$(d))		:=		\
		$(LF_$(d))		\
		-L$(GLFW_LIB_DIR)	\
		-L$(CGLM_LIB_DIR)	\
		-L$(GLIB_LIB_DIR)	\
		-L$(GLIBEXT_LIB_DIR)	\
		-lglfw			\
		-lcglm			\
		-lglib-2.0		\
		-lglibext		\
		-lvulkan

$(COBJS_$(d)): $(GLIBEXT_LIB) $(CGLM_LIB) $(GLIB_LIB) $(GLFW_LIB)
$(d): $(TGT_VKEXAMPLE_3)
$(TGT_VKEXAMPLE_3): $(OBJS_$(d))