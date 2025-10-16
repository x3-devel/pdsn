#include <stddef.h>
#include <stdio.h>

#include "pdsn.h"

/** Format registration */
pdsn_format_t pdsn_format_list[] = {
    {PDSN_FMT_BASH, "bash", pdsn_bash_cb},
    {PDSN_FMT_EXPORT, "export", pdsn_export_cb},
    {PDSN_FMT_JSON, "json", NULL},
    {PDSN_FMT_XML, "xml", NULL},
    {PDSN_FMT_YAML, "yaml", pdsn_yaml_cb},  
    {PDSN_FMT_TOML, "toml", NULL},
};

const char *pdsn_matches[_PDSN_MEMBER_COUNT] = {
    "schema",
    "username",
    "password",
    "hostname",
    "port",
    "path",
    "query",
};

/**
 * @brief pdsn print callback for bash
 * 
 * @param dsn 
 * @return int 
 */
size_t pdsn_bash_cb(_dsn_t *dsn, char *buf, size_t bufsz)
{
    char **dsn_hack = (char **)dsn;
    int total = 0;

    for (int i = 0; i < _PDSN_MEMBER_COUNT; i++) {
        total = total + snprintf(buf + total, bufsz, "%s=\"%s\"\n", pdsn_matches[i], dsn_hack[i] == NULL ? "" : dsn_hack[i]);
    }

    return 0;
}

/**
 * @brief pdsn print callback for POSIX 'export'
 * 
 * @param dsn 
 * @return int 
 */
size_t pdsn_export_cb(_dsn_t *dsn, char *buf, size_t bufsz)
{
    char **dsn_hack = (char **)dsn;
    int total = 0;

    for (int i = 0; i < _PDSN_MEMBER_COUNT; i++) {
        total = total + snprintf(buf + total, bufsz, "export %s=\"%s\";\n", pdsn_matches[i], dsn_hack[i] == NULL ? "" : dsn_hack[i]);
    }

    return 0;
}

/**
 * @brief pdsn print callback for POSIX 'yaml'
 * 
 * @param dsn 
 * @return int 
 */
size_t pdsn_yaml_cb(_dsn_t *dsn, char *buf, size_t bufsz)
{
    char **dsn_hack = (char **)dsn;
    int total = 0;

    for (int i = 0; i < _PDSN_MEMBER_COUNT; i++) {
        total = total + snprintf(buf + total, bufsz, "%s: %s\n", pdsn_matches[i], dsn_hack[i] == NULL ? "null" : dsn_hack[i]);
    }

    return 0;
}


