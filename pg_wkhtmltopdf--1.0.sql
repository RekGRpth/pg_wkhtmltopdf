-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_wkhtmltopdf" to load this file. \quit

CREATE OR REPLACE FUNCTION wkhtmltopdf(html TEXT) RETURNS TEXT AS 'MODULE_PATHNAME', 'wkhtmltopdf' LANGUAGE 'c';
CREATE OR REPLACE FUNCTION wkhtmltopdf_set_global_setting(name TEXT, value TEXT) RETURNS VOID AS 'MODULE_PATHNAME', 'pg_wkhtmltopdf_set_global_setting' LANGUAGE 'c';
