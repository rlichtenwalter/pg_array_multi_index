MODULES = pg_array_multi_index
EXTENSION = pg_array_multi_index
DATA = pg_array_multi_index--1.0.sql
DOCS = README.pg_array_multi_index

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
