#ifndef PRIVATE_PDSN_H
#define PRIVATE_PDSN_H

#define _XOPEN_SOURCE 500 // for strdup

/** Public Includes */
#include <pdsn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

/** PDSN - Definitions */

#define ARRAY_SIZE(x) ((int)(sizeof (x) / sizeof (x)[0]))

#define _PDSN_MEMBER_COUNT 7
#define _PDSN_MEMBER_SIZE sizeof(void*)
#define _PDSN_SIZE (_PDSN_MEMBER_COUNT * _PDSN_MEMBER_SIZE)

typedef struct {
    char *schema;
    char *username;
    char *password;
    char *hostname;
    char *port;
    char *path;
    char *query;
} _dsn_t;


typedef enum {
    GROUP_SCHEMA    =  1,
    GROUP_USERNAME  =  3,
    GROUP_PASSWORD  =  5,
    GROUP_HOSTNAME  =  6,
    GROUP_PORT      =  8,
    GROUP_PATH      = 10,
    GROUP_QUERY     = 12,
} pdsn_regex_group_t;

typedef struct {
    int group; // regex group
    const char *name; // entry name (schema, username, password, hostname, port, path)
} pdsn_match_t;

extern pdsn_match_t pdsn_matches[_PDSN_MEMBER_COUNT];

/** Format - Definitions */

/**
 * @brief Format - Callback Definition
 * 
 * @param dsn dsn object
 * @param buf output buffer
 * @param bufsz output buffer size
 * @return typedef 
 */
typedef size_t (pdsn_fmt_cb)(_dsn_t *dsn, char *buf, size_t bufsz);

/**
 * @brief holds const predefined format infos
 * 
 */
typedef struct {
    pdsn_format_no_t no;
    const char *name;
    pdsn_fmt_cb *callback;
} pdsn_format_t;

/** Format - Function definitions */

/**
 * @brief Format - Bash Callback Function
 * 
 * @param dsn private dsn object  
 * @param buf output buffer
 * @param bufsz output buffer size
 * @return int 
 */
size_t pdsn_bash_cb(_dsn_t *dsn, char *buf, size_t bufsz);

/**
 * @brief Format - POSIX Export Callback Function
 * 
 * @param dsn private dsn object
 * @param buf output buffer
 * @param bufsz output buffer size
 * @return size_t 
 */
size_t pdsn_export_cb(_dsn_t *dsn, char *buf, size_t bufsz);

/**
 * @brief Format - YAML Callback Function
 * 
 * @param dsn private dsn object
 * @param buf output buffer
 * @param bufsz output buffer size
 * @return size_t 
 */
size_t pdsn_yaml_cb(_dsn_t *dsn, char *buf, size_t bufsz);

extern pdsn_format_t pdsn_format_list[];

/** Regex - Definitions */



/**
 * @brief 
 * 
 */

/**
 * @brief 
 * 
 * @param input 
 * @return int 
 */
int dsn_regex_find(const char *input, regmatch_t *matches, int count);

/**
 * @brief Fills 
 * 
 * @param in_dsn input dsn string
 * @param dsn private dsn object
 * @param idx 
 * @param matches 
 * @param count 
 * @return int 
 */
int dsn_fill_member(const char *in_dsn, _dsn_t *dsn, int idx, regmatch_t *matches, int count);
#endif