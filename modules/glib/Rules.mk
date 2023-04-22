# This should build glib

# OS Dependent

GLIB_LIB_NAME_WCARD	:=libglib-2.0
GLIB_LIB_NAME		:=libglib-2.0.so

GLIB_LIB_DIR	:=$(bd)/usr/local/lib
GLIB_LIB	:=$(GLIB_LIB_DIR)/$(GLIB_LIB_NAME)

GLIB_MESON_DIR	:=$(bd)/meson-tmp
GLIB_MESON_LIB	:=$(GLIB_MESON_DIR)/src

GLIB_INCLUDE_DIR:=$(bd)/usr/local/include/glib-2.0

TGTS_$(d)	:=$(GLIB_LIB)
TGT_LIB		:=$(TGT_LIB) $(TGTS_$(d))

$(d): $(TGTS_$(d))
$(TGTS_$(d)):
	meson setup $(GLIB_MESON_DIR) $(GLIB_DIR)/glib
	meson compile -C $(GLIB_MESON_DIR)
	DESTDIR=../ meson install -C $(GLIB_MESON_DIR)
	ln -s $(abspath $(GLIB_LIB_DIR)/glib-2.0/include/glibconfig.h) $(abspath $(GLIB_INCLUDE_DIR)/glibconfig.h)