get_subdirs = $(shell find $(1) -type d)
subdirs_append_flags = $(foreach di, $(call get_subdirs, $(1)), $(eval $(2)_$(di) += $(3)))

# $(CC) - compile C 
# $(CF_ALL) - generic C flags
# $(CF_$(patsubst %/,%,$(dir $<))) - C flags, specific for
# 					directory, object
# 					file depends to
# $(CF_$@) - C flags, specific for compiled obj file
# $(LF_ALL) - generic linker flags
# $(LF_$@) - link flags, specific for file

__CC_COMP=$(CC)					\
	 $(CF_ALL)				\
	 $(CF_$@)				\
	 $(CF_$(patsubst %/,%,$(dir $<)))	\
	 -o $@ -c $<

__AS_COMP=$(AS)					\
	 $(ASF_ALL)				\
	 $(ASF_$@)				\
	 $(ASF_$(patsubst %/,%,$(dir $<)))	\
	 -o $@ -c $<

__CC_LINK=$(CC)					\
	 $(LF_ALL)				\
	 $(LF_$@)				\
	 -o $@ $(filter %.o,$^)

__L_LINK=$(LD)					\
	 $(LF_ALL)				\
	 $(LF_$@)				\
	 -o $@ $(filter %.o,$^)

# sp = stack pointer
# dirstack_* = stack of directories
# d = current directory (in this very include)
# bd = mirror of d in build directory
# od = mirror of d in objs directory

define dstack_push
$(eval sp		:=$(sp).x)
$(eval dirstack_$(sp)	:=$(d))
$(eval d		:=$(dir))
$(eval bd		:=$(DIR_OUT)/$(d))
$(eval od		:=$(DIR_OBJS)/$(d))
endef

define dstack_pop
$(eval d	:=$(dirstack_$(sp)))
$(eval bd	:=$(DIR_OUT)/$(d))
$(eval od	:=$(DIR_OBJS)/$(d))
$(eval sp	:=$(basename $(sp)))
endef

define dinclude
$(foreach val,$(SUBDIRS_$(d)),\
	$(eval dir:=$(d)/$(val))\
	$(rbeg)\
	$(eval include $(dir)/Rules.mk)\
	$(call dinclude)\
	$(rend))
endef

define rbeg
$(dstack_push)
endef

define rend
$(eval
ifneq ($(MAKECMDGOALS),clean)
$$(cincdeps)
endif
)
$(eval OBJS_TOTAL +=$(OBJS_$(d)))
$(dstack_pop)
endef

dirguard=@mkdir -p $(@D)

append =$(eval $(1):=$($(1)) $(2))

