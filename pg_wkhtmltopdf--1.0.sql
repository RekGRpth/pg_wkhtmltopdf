-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_wkhtmltopdf" to load this file. \quit

CREATE FUNCTION wkhtmltopdf_convert() RETURNS BYTEA AS 'MODULE_PATHNAME', 'pg_wkhtmltopdf_convert' LANGUAGE 'c';
CREATE FUNCTION wkhtmltopdf_set_global_setting(name TEXT, value TEXT) RETURNS BOOL AS 'MODULE_PATHNAME', 'pg_wkhtmltopdf_set_global_setting' LANGUAGE 'c';
CREATE FUNCTION wkhtmltopdf_set_object_setting(name TEXT, value TEXT) RETURNS BOOL AS 'MODULE_PATHNAME', 'pg_wkhtmltopdf_set_object_setting' LANGUAGE 'c';
