INCLUDE_TOP	:=$(d)/include
GLIBEXT_TOP	:=$(d)/glibext

# note that glibext is checked first
SUBDIRS_$(d)	:=glibext 0_init 1_vlayers 2_physdev 3_ldev 3_ldev_napi 4_queues