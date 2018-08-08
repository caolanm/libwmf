SUBDIRS = extra ipa . convert

CFLAGS = @CFLAGS@ @WMF_CFLAGS@

DEFS = @DEFS@ @WMF_DEFS@ \
	-DPACKAGE=\"@PACKAGE@\" \
	-DVERSION=\"@VERSION@\" \
	-DWMF_FONTDIR=\"@WMF_FONTDIR@\" \
	-DWMF_GS_FONTDIR=\"@WMF_GS_FONTDIR@\" \
	-DWMF_SYS_FONTMAP=\"@WMF_SYS_FONTMAP@\" \
	-DWMF_XTRA_FONTMAP=\"@WMF_XTRA_FONTMAP@\" \
	-DWMF_GS_FONTMAP=\"@WMF_GS_FONTMAP@\"

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir)

lib_LTLIBRARIES = libwmflite.la libwmf.la

libwmflite_la_SOURCES = api.c bbuf.c meta.c player.c

libwmflite_la_LDFLAGS = -lm \
	-no-undefined \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE) -export-dynamic

libwmf_la_SOURCES = font.c stream.c wmf.c

if LIBWMF_OPT_TRIO
LIBTRIO = extra/trio/libtrio.la
else
LIBTRIO = 
endif

if LIBWMF_OPT_SYS_GD
LIBGD = 
else
LIBGD = extra/gd/libgd.la
endif

libwmf_la_LIBADD = ipa/libipa.la libwmflite.la $(LIBGD) $(LIBTRIO)

libwmf_la_LDFLAGS = @WMF_LIBFLAGS@ \
	-no-undefined \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE) -export-dynamic

PLAYERHEADERS = \
	player/clip.h \
	player/color.h \
	player/coord.h \
	player/dc.h \
	player/defaults.h \
	player/meta.h \
	player/record.h \
	player/region.h

EXTRA_DIST = wmfdefs.h player.h $(PLAYERHEADERS) api.h bbuf.h font.h stream.h
