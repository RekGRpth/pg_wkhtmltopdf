$(OBJS): Makefile
DATA = pg_wkhtmltopdf--1.0.sql
EXTENSION = pg_wkhtmltopdf
MODULE_big = $(EXTENSION)
OBJS = $(EXTENSION).o
PG_CONFIG = pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
SHLIB_LINK = -lwkhtmltox
include $(PGXS)
