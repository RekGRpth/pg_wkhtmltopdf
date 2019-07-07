EXTENSION = pg_wkhtmltopdf
MODULE_big = $(EXTENSION)

PG_CONFIG = pg_config
OBJS = $(EXTENSION).o
DATA = pg_wkhtmltopdf--1.0.sql

LIBS += -lwkhtmltox
SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

$(OBJS): Makefile