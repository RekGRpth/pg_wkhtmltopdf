-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_wkhtmltopdf" to load this file. \quit

CREATE OR REPLACE FUNCTION wkhtmltopdf(html TEXT) RETURNS TEXT AS 'MODULE_PATHNAME', 'wkhtmltopdf' LANGUAGE 'c';
