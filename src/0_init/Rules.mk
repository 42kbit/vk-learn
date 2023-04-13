ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/main.o

TGTS_$(d)	:=$(bd)/main

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=	\
		$(CF_$(d))

$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
