#include "pdsn.h"

_Static_assert(sizeof(dsn_t) == sizeof(_dsn_t),
               "struct size mismatch between public and internal definitions");

pdsn_match_t pdsn_matches[_PDSN_MEMBER_COUNT] = {
    { GROUP_SCHEMA,     "schema"},
    { GROUP_USERNAME,   "username"},
    { GROUP_PASSWORD,   "password"},
    { GROUP_HOSTNAME,   "hostname"},
    { GROUP_PORT,       "port"},
    { GROUP_PATH,       "path"},
    { GROUP_QUERY,      "query"},
};

int parse_dsn(const char *input, dsn_t *dsn)
{
    regmatch_t matches[20];
    int ret = 0;

    // try all available regex, maybe I can 
    ret = dsn_regex_find(input, matches, ARRAY_SIZE(matches));
    if (ret != 0) {
        return PDSN_EPARSE; // regex failed
    }

    for (int i = 0; i < _PDSN_MEMBER_COUNT; i++) {

        _dsn_t *_dsn = (_dsn_t*) dsn;
        ret = dsn_fill_member(input, _dsn, i, matches, ARRAY_SIZE(matches));
        if (ret) {
            errno = ret;
            return -1;
        }
        
    }

    return 0;
}

int dsn_format(pdsn_format_no_t format, const char **format_name, dsn_t *dsn, char *buf, size_t bufsz)
{
    _dsn_t *_dsn = (_dsn_t*)dsn;

    // check for format range
    if (format < 0 && format > (PDSN_FMT_MAX-1)) {
        return PDSN_ENOFMT;
    }

    // check if format is available (index)
    pdsn_fmt_cb *callback = pdsn_format_list[format].callback;
    if (callback == NULL) {
        return PDSN_ENOFMT;
    }

    // call it
    int ret = callback(_dsn, buf, bufsz);
    if (ret != 0) {
        return PDSN_EFORMAT;
    }
    
    // if given, set format name argument
    if (format_name != NULL)
        *format_name = pdsn_format_list[format].name;
                
    return 0;
}

void dsn_destroy(dsn_t *dsn)
{
    char **dsn_hack=(char **)dsn;
    for (int i = 0; i < ARRAY_SIZE(pdsn_matches); i++) {
        free(dsn_hack[i]);
    }
}

