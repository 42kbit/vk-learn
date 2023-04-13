# TODO: Decompose the file in multiple .mk files


# every line with sources, has a mirror in $(DIR_OUT)
# so ant/main.c, will output at $(DIR_OUT)/ant/main.o

DIR_OUT	?=.out
DIR_OBJS :=$(DIR_OUT)/.objs

DIR_BIN	?=$(DIR_OUT)/bin
DIR_SBIN?=$(DIR_OUT)/sbin
DIR_LIB	?=$(DIR_OUT)/lib
DIR_ETC	?=$(DIR_OUT)/etc

# Build System Essentials

BSE_DIR			:=debils-essentials
BSE_CFG_DIR		:=$(BSE_DIR)/cfg
BSE_PROFILE_DIR		:=$(BSE_DIR)/profiles

BSE_CFG_FILE_NAME	:=Makefile.config
BSE_CFG_FILE_SRC	:=$(BSE_CFG_DIR)/$(BSE_CFG_FILE_NAME)-template
BSE_CFG_FILE_DST	:=./.$(BSE_CFG_FILE_NAME)

ifneq ("$(wildcard $(BSE_CFG_FILE_DST))","")
$(eval include $(BSE_CFG_FILE_DST))
else
$(info $(BSE_CFG_FILE_DST) file not present! Copying from $(BSE_CFG_FILE_SRC))
# TODO: Make OS agnostic.
$(shell cp $(BSE_CFG_FILE_SRC) $(BSE_CFG_FILE_DST))
$(error now configure $(BSE_CFG_FILE_DST) and go on)
endif

include $(BSE_DIR)/var.mk
include $(BSE_DIR)/lib.mk

# TODO: Maybe remove this
# In config file, if variable starts with 
CONFIG_ALL	:=$(filter CONFIG_%,$(.VARIABLES))
$(foreach c,$(CONFIG_ALL),\
	$(eval CF_ALL +=-D$(c)=$($(c)) ) \
	$(eval ASF_ALL +=-D$(c)=$($(c)) ))

ifeq ($(NOSTYLE),y)
style = $(1)
else
style = @$(1); echo $(2)
endif

AS_COMP	=$(call style,$(__AS_COMP),[AS]		$@)
CC_COMP	=$(call style,$(__CC_COMP),[CC]		$@)
CC_LINK	=$(call style,$(__CC_LINK),[CLD]	$@)
L_LINK	=$(call style,$(__L_LINK),[LD]		$@)

OBJS_TOTAL	:=

# cincdeps (C Include Dependency)
# Includes dependency for C, currently only for gcc

# includes deps from .c files, with use of COBJS_$(d).
# parses COBJS_$(d) to get source files, than makes deps
# from it.
cincdeps =$(eval include $(patsubst %.c,\
	$(DIR_OBJS)/%.d,\
	$(patsubst $(DIR_OBJS)/%.o,%.c,$(COBJS_$(d)))) )

# LOGIC

.PHONY: all init targets clean all

all: init targets
	@echo done

ROOT		:=$(PWD)

# Copy paste of $(dinclude), but for root mkfile
SUBDIRS	:=$(ROOT_TOP)
$(foreach val,$(SUBDIRS),\
	$(eval dir:=$(val))\
	$(rbeg)\
	$(eval include $(dir)/Rules.mk)\
	$(call dinclude)\
	$(rend))

$(OBJS_TOTAL): $(CFG_FILE)
$(info $(OBJS_TOTAL))

$(DIR_OBJS)/%.o: %.c
	$(dirguard)
	$(CC_COMP)

$(DIR_OBJS)/%.o: %.S
	$(dirguard)
	$(AS_COMP)

# first echo writes directory, where .o file located
# to dep file.
#
# .out/					   <- now in depfile
#
# next C compiler generates make dependency, with
# given flags (include dirs needed), and writes it to
#
# first  - for directory wide
# second - specific for file
#
# .out/foo/file.o: foo/file.c bar/header.h <- now in depfile
#

$(DIR_OBJS)/%.d: %.c
	$(dirguard)
	$(CDEPS)

init:
# init build tree
	$(call style,mkdir -p $(DIR_OUT) \
	$(addprefix $(DIR_OUT)/,$(shell find $(ROOT_TOP) -type d)),\
	[DR] Initializing build tree...)
# TGT_* CLEAN are user defined
targets: $(TGT_BIN) $(TGT_SBIN) $(TGT_ETC) $(TGT_LIB)

clean:
	rm -rf $(DIR_OUT)
clean_%:
	rm -rf $(DIR_OUT)/$(patsubst clean_%,%,$@)
	rm -rf $(DIR_OBJS)/$(patsubst clean_%,%,$@)
rmcfg:
	rm $(BSE_CFG_FILE_DST)