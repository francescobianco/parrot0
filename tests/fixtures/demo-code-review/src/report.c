/* Builds the report. One function does far too much. */
#include <stdio.h>

int load_rows(void);
int filter_rows(void);
int sort_rows(void);
int format_header(void);
int format_body(void);
int format_footer(void);
int write_output(void);
int flush_output(void);
int log_progress(void);
int check_quota(void);

int build_report(void) {
    load_rows();
    filter_rows();
    sort_rows();
    format_header();
    format_body();
    format_footer();
    write_output();
    flush_output();
    log_progress();
    check_quota();
    return 0;
}

int render_summary(void) {
    format_header();
    return 0;
}
