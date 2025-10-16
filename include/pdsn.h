#ifndef PUBLIC_PDSN_H
#define PUBLIC_PDSN_H

#include <stddef.h>

typedef enum {
    PDSN_ENOFMT = -1, // format not found
    PDSN_EFORMAT = -2, // formatting failed
    PDSN_EPARSE = -3, // parsing failed
    PDSN_EGRPOOI = -4, // group out of index
    PDSN_EOOI = -4, // pdsn_hack out of index
} pdsn_error_t;

/** Formats */
typedef enum {
    PDSN_FMT_BASH   = 0,
    PDSN_FMT_EXPORT = 1,
    PDSN_FMT_JSON   = 2,
    PDSN_FMT_XML    = 3,
    PDSN_FMT_YAML   = 4,
    PDSN_FMT_TOML   = 5,
    PDSN_FMT_MAX
} pdsn_format_no_t;

typedef struct {
    unsigned char _private[56];
} dsn_t;

/**
 * @brief Parse a given Data Source Name (DSN) 
 * 
 * @param input input dsn
 * @param dsn dsn object
 * @return 0 (succes), -1 (failure, sets 'errno')
 */
int parse_dsn(const char *input, dsn_t *dsn);

/**
 * @brief Convert output format name to idx
 * 
 * @param format output format name
 * @return int 
 */
int pdsn_get_format(const char *format);

/**
 * @brief Format DSN object
 * 
 * @param format format number (0(bash), 1(export), 2(json), 3(xml), 4(yaml), 5(toml))
 * @param format_name format name (bash, export, json, xml, yaml, toml)
 * @param dsn dsn object
 * @param buf buffer
 * @param bufsz buffer size
 * @return 0 (success), ENOFMT or EFORMAT (failure)
 */
int dsn_format(pdsn_format_no_t format, const char **format_name, dsn_t *dsn, char *buf, size_t bufsz);

/**
 * @brief Destroy DSN object
 * 
 * @param dsn dsn object
 */
void dsn_destroy(dsn_t *dsn);

#endif