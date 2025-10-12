#include "pdsn.h"

#include <netdb.h>

const char *regex_string_list[] = {
    // first default dsn regex 
    // "^([A-Za-z][A-Za-z0-9+.-]*)://" // schema()
    // "(([^:@/[:space:]]+)(:([^@/[:space:]]*))?@)?" // username:password, username, password
    // "(([A-Za-z0-9_.-]+)?(:([0-9]+))?)?" // hostname, port
    // "(/([^[:space:]?#]*))?$", // path
    "^([A-Za-z][A-Za-z0-9+.-]*)://"            // schema
    "(([^:@/[:space:]]+)(:([^@/[:space:]]*))?@)?" // user:pass@
    "([^:/[:space:]?#]+)"                      // host
    "(:([0-9]+))?"                             // :port
    "(/([^[:space:]?#]*))?"                    // /path
    "(\\?([^#[:space:]]*))?$"                  // ?query
};

regex_t regex_list[ARRAY_SIZE(regex_string_list)];

/**
 * @brief Initializes all available regexes
 * 
 */
__attribute__((constructor)) void compile_dsn_regex_list(void)
{
    char errbuf[256];

    for (int i = 0; (i < ARRAY_SIZE(regex_list) && i < ARRAY_SIZE(regex_string_list)); i++) {
        int ret = regcomp(&regex_list[i], regex_string_list[i], REG_EXTENDED);
        if (ret) {
            regerror(ret, &regex_list[i], errbuf, sizeof(errbuf));
            fprintf(stderr, "Regex compile failed: %s\n", errbuf);
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Cleanup all compiled regexes
 * 
 */
__attribute__((destructor)) void destroy_dsn_regex_list(void)
{
    for (int i = 0; (i < ARRAY_SIZE(regex_list) && i < ARRAY_SIZE(regex_string_list)); i++) {
        regfree(&regex_list[i]);
    }
}

int dsn_regex_find(const char *input, regmatch_t *matches, int count)
{
    char errbuf[256];

    int ret = -1;
    for (int i = 0; i < ARRAY_SIZE(regex_list) && (ret = regexec(&regex_list[i], input, count, matches, 0)); i++) {
        regerror(ret, &regex_list[i], errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex exec failed: %s in %s:%d\n", errbuf, __FILE__, __LINE__);
    }

    return ret;
}

/**
 * @brief fills dsn member based on index
 * 
 * @param idx 
 * @param matches 
 * @param count 
 * @return 0 or ENOMEM
 */
int dsn_fill_member(const char *in_dsn, _dsn_t *dsn, int idx, regmatch_t *matches, int count)
{   
    char **dsn_hack=(char **)dsn;
    int n = pdsn_matches[idx].group;

    if (n >= count) {
        return PDSN_EGRPOOI;
    }

    if (matches[n].rm_so != -1) {
        int len = matches[n].rm_eo - matches[n].rm_so;

        char buf[1024];
        snprintf(buf, sizeof(buf), "%.*s", len, in_dsn + matches[n].rm_so);
        dsn_hack[idx] = strdup(buf);
        if (dsn_hack[idx] == NULL) {
            errno = ENOMEM;
            return -1; // mem error
        }
    }
    else {
        dsn_hack[idx] = NULL; // this is not an error
    }

    if (n == GROUP_PORT && ! dsn->port) {
        struct servent *ent = getservbyname(dsn->schema, NULL);

        char buf[6];
        snprintf(buf, sizeof buf, "%d", ntohs(ent->s_port));

        dsn_hack[idx] = strdup(buf);
        if (dsn_hack[idx] == NULL) {
            errno = ENOMEM;
            return -1; // mem error
        }
    }

    return 0;
}