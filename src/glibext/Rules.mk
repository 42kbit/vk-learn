
ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/goptional.o

TGTS_$(d)	:=$(bd)/libglibext.so

GLIBEXT_LIB	:=$(TGTS_$(d))
GLIBEXT_LIB_DIR	:=$(bd)
GLIBEXT_INCLUDE_DIR	:=$(d)/include

TGT_BIN		:=$(TGT_BIN) $(TGTS_$(d))

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-Wno-unused-parameter	\
		-I$(INCLUDE_TOP)	\
		-I$(GLIB_INCLUDE_DIR)	\
		-I$(GLIBEXT_INCLUDE_DIR)

LF_$(TGTS_$(d))		:=		\
		$(LF_$(d))		\
		-L$(GLIB_LIB_DIR)	\
		-lglib-2.0		\
		-shared

$(COBJS_$(d)): $(GLIB_LIB)
$(d): $(TGTS_$(d))
$(TGTS_$(d)): $(GLIB_LIB)$(OBJS_$(d))
	$(L_LINK)