-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_wkhtmltopdf" to load this file. \quit

CREATE OR REPLACE FUNCTION wkhtmltopdf(url text) RETURNS text AS 'MODULE_PATHNAME', 'wkhtmltopdf' LANGUAGE 'c';
