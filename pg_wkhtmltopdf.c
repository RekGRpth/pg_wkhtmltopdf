#include <postgres.h>
#include <fmgr.h>

#include <utils/builtins.h>
#include <wkhtmltox/pdf.h>

#define EXTENSION(function) Datum (function)(PG_FUNCTION_ARGS); PG_FUNCTION_INFO_V1(function); Datum (function)(PG_FUNCTION_ARGS)

PG_MODULE_MAGIC;

/*static void progress_changed_callback(wkhtmltopdf_converter *converter, int p) {
    printf("progress_changed_callback: %3d%%\n", p);
    fflush(stdout);
}*/

/*static void phase_changed_callback(wkhtmltopdf_converter *converter) {
    int phase = wkhtmltopdf_current_phase(converter);
    printf("phase_changed_callback: %s\n", wkhtmltopdf_phase_description(converter, phase));
    fflush(stdout);
}*/

static void error_callback(wkhtmltopdf_converter *converter, const char *msg) {
//    fprintf(stderr, "error_callback: %s\n", msg);
//    fflush(stderr);
    ereport(WARNING, (errmsg("%s", msg)));
}

static void warning_callback(wkhtmltopdf_converter *converter, const char *msg) {
//    fprintf(stderr, "warning_callback: %s\n", msg);
//    fflush(stderr);
    ereport(WARNING, (errmsg("%s", msg)));
}

/*static void finished_callback(wkhtmltopdf_converter *converter, int p) {
    printf("finished_callback: %3d%%\n", p);
    fflush(stdout);
}*/

EXTENSION(wkhtmltopdf) {
    char *html;
    wkhtmltopdf_global_settings *global_settings;
    wkhtmltopdf_object_settings *object_settings;
    wkhtmltopdf_converter *converter;
    const unsigned char *data;
    long len;
    if (PG_ARGISNULL(0)) ereport(ERROR, (errmsg("html is null!")));
    html = TextDatumGetCString(PG_GETARG_DATUM(0));
    if (!wkhtmltopdf_init(0)) ereport(ERROR, (errmsg("!wkhtmltopdf_init")));
    if (!(global_settings = wkhtmltopdf_create_global_settings())) ereport(ERROR, (errmsg("!global_settings")));
    if (!(object_settings = wkhtmltopdf_create_object_settings())) ereport(ERROR, (errmsg("!object_settings")));
    if (!wkhtmltopdf_set_object_setting(object_settings, "page", (const char *)html)) ereport(ERROR, (errmsg("!wkhtmltopdf_set_object_setting")));
    if (!(converter = wkhtmltopdf_create_converter(global_settings))) ereport(ERROR, (errmsg("!converter")));
//    (void)wkhtmltopdf_set_progress_changed_callback(converter, progress_changed_callback);
//    (void)wkhtmltopdf_set_phase_changed_callback(converter, phase_changed_callback);
    (void)wkhtmltopdf_set_error_callback(converter, error_callback);
    (void)wkhtmltopdf_set_warning_callback(converter, warning_callback);
//    (void)wkhtmltopdf_set_finished_callback(converter, finished_callback);
    (void)wkhtmltopdf_add_object(converter, object_settings, (const char *)NULL);
    if (!wkhtmltopdf_convert(converter)) ereport(ERROR, (errmsg("!wkhtmltopdf_convert")));
    if (!(len = wkhtmltopdf_get_output(converter, &data))) ereport(ERROR, (errmsg("!len")));
    (void)wkhtmltopdf_destroy_converter(converter);
    (void)wkhtmltopdf_destroy_object_settings(object_settings);
    (void)wkhtmltopdf_destroy_global_settings(global_settings);
    if (!wkhtmltopdf_deinit()) ereport(ERROR, (errmsg("!wkhtmltopdf_deinit")));
    (void)pfree(html);
    PG_RETURN_TEXT_P(cstring_to_text_with_len((const char *)data, len));
}
