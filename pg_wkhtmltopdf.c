#include <postgres.h>

#include <fmgr.h>
#include <utils/builtins.h>
#include <wkhtmltox/pdf.h>

#define EXTENSION(function) Datum (function)(PG_FUNCTION_ARGS); PG_FUNCTION_INFO_V1(function); Datum (function)(PG_FUNCTION_ARGS)

PG_MODULE_MAGIC;

static bool cleanup = false;
#if PG_VERSION_NUM >= 90500
static struct MemoryContextCallback objectMemoryContextCallback = {0};
#endif
static wkhtmltopdf_global_settings *global_settings = NULL;
static wkhtmltopdf_object_settings *object_settings = NULL;

void _PG_init(void); void _PG_init(void) {
    if (!wkhtmltopdf_init(0)) ereport(ERROR, (errmsg("!wkhtmltopdf_init")));
    if (!(global_settings = wkhtmltopdf_create_global_settings())) ereport(ERROR, (errmsg("!wkhtmltopdf_create_global_settings")));
}

void _PG_fini(void); void _PG_fini(void) {
    wkhtmltopdf_destroy_global_settings(global_settings);
    if (!wkhtmltopdf_deinit()) ereport(ERROR, (errmsg("!wkhtmltopdf_deinit")));
}

#if PG_VERSION_NUM >= 90500
static void objectMemoryContextCallbackFunction(void *arg) {
    if (!cleanup) return;
    wkhtmltopdf_destroy_object_settings(object_settings);
    object_settings = NULL;
    cleanup = false;
}
#endif

static void progress_changed_callback(wkhtmltopdf_converter *converter, int p) {
    ereport(DEBUG1, (errmsg("%3d%%", p)));
}

static void phase_changed_callback(wkhtmltopdf_converter *converter) {
    ereport(DEBUG1, (errmsg("%s", wkhtmltopdf_phase_description(converter, wkhtmltopdf_current_phase(converter)))));
}

static void error_callback(wkhtmltopdf_converter *converter, const char *msg) {
    ereport(ERROR, (errmsg("%s", msg)));
}

static void warning_callback(wkhtmltopdf_converter *converter, const char *msg) {
    ereport(WARNING, (errmsg("%s", msg)));
}

static void finished_callback(wkhtmltopdf_converter *converter, int p) {
    ereport(DEBUG1, (errmsg("%3d%%", p)));
}

EXTENSION(pg_wkhtmltopdf_convert) {
    bytea *pdf;
    char *data = NULL;
    long len;
    wkhtmltopdf_converter *converter;
    cleanup = true;
    if (!object_settings) ereport(ERROR, (errmsg("!object_settings")));
    if (!(converter = wkhtmltopdf_create_converter(global_settings))) ereport(ERROR, (errmsg("!wkhtmltopdf_create_converter")));
    wkhtmltopdf_set_progress_changed_callback(converter, progress_changed_callback);
    wkhtmltopdf_set_phase_changed_callback(converter, phase_changed_callback);
    wkhtmltopdf_set_error_callback(converter, error_callback);
    wkhtmltopdf_set_warning_callback(converter, warning_callback);
    wkhtmltopdf_set_finished_callback(converter, finished_callback);
    wkhtmltopdf_add_object(converter, object_settings, (const char *)NULL);
    if (!wkhtmltopdf_convert(converter)) ereport(ERROR, (errmsg("!wkhtmltopdf_convert")));
    if (!(len = wkhtmltopdf_get_output(converter, (const unsigned char **)&data))) ereport(ERROR, (errmsg("!wkhtmltopdf_get_output")));
    pdf = cstring_to_text_with_len(data, len);
    wkhtmltopdf_destroy_converter(converter);
    PG_RETURN_BYTEA_P(pdf);
}

EXTENSION(pg_wkhtmltopdf_set_global_setting) {
    char *name, *value;
    cleanup = true;
    if (PG_ARGISNULL(0)) ereport(ERROR, (errmsg("name is null!")));
    if (PG_ARGISNULL(1)) ereport(ERROR, (errmsg("value is null!")));
    name = TextDatumGetCString(PG_GETARG_DATUM(0));
    value = TextDatumGetCString(PG_GETARG_DATUM(1));
    if (!object_settings && !(object_settings = wkhtmltopdf_create_object_settings())) ereport(ERROR, (errmsg("!wkhtmltopdf_create_object_settings")));
#if PG_VERSION_NUM >= 90500
    if (!objectMemoryContextCallback.func) {
        objectMemoryContextCallback.func = objectMemoryContextCallbackFunction;
        MemoryContextRegisterResetCallback(CurrentMemoryContext, &objectMemoryContextCallback);
    }
#endif
    if (!wkhtmltopdf_set_global_setting(global_settings, (const char *)name, (const char *)value)) ereport(ERROR, (errmsg("!wkhtmltopdf_set_global_setting")));
    pfree(name);
    pfree(value);
    cleanup = false;
    PG_RETURN_BOOL(true);
}

EXTENSION(pg_wkhtmltopdf_set_object_setting) {
    char *name, *value;
    cleanup = true;
    if (PG_ARGISNULL(0)) ereport(ERROR, (errmsg("name is null!")));
    if (PG_ARGISNULL(1)) ereport(ERROR, (errmsg("value is null!")));
    name = TextDatumGetCString(PG_GETARG_DATUM(0));
    value = TextDatumGetCString(PG_GETARG_DATUM(1));
    if (!object_settings && !(object_settings = wkhtmltopdf_create_object_settings())) ereport(ERROR, (errmsg("!wkhtmltopdf_create_object_settings")));
#if PG_VERSION_NUM >= 90500
    if (!objectMemoryContextCallback.func) {
        objectMemoryContextCallback.func = objectMemoryContextCallbackFunction;
        MemoryContextRegisterResetCallback(CurrentMemoryContext, &objectMemoryContextCallback);
    }
#endif
    if (!wkhtmltopdf_set_object_setting(object_settings, (const char *)name, (const char *)value)) ereport(ERROR, (errmsg("!wkhtmltopdf_set_object_setting")));
    pfree(name);
    pfree(value);
    cleanup = false;
    PG_RETURN_BOOL(true);
}
