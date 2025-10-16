#include "pdsn.h"

#include <netdb.h>

pdsn_regex_t pdsn_regex_list[] = {
    {
        "^([A-Za-z][A-Za-z0-9+.-]*)://" // schema
    "(([^:@/[:space:]]+)(:([^@/[:space:]]*))?@)?" // user:pass@
    "([^:/[:space:]?#]+)" // host
    "(:([0-9]+))?" // :port
    "(/([^[:space:]?#]*))?" // /path
    "(\\?([^#[:space:]]*))?$", // query 
    {1, 3, 5, 6, 8, 10, 12,}
    }
};

regex_t regex_list[ARRAY_SIZE(pdsn_regex_list)];

/**
 * @brief Initializes all available regexes
 * 
 */
__attribute__((constructor)) void compile_dsn_regex_list(void)
{
    char errbuf[256];

    for (int i = 0; (i < ARRAY_SIZE(regex_list) && i < ARRAY_SIZE(pdsn_regex_list)); i++) {
        int ret = regcomp(&regex_list[i], pdsn_regex_list[i].regstr, REG_EXTENDED);
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
    for (int i = 0; (i < ARRAY_SIZE(regex_list) && i < ARRAY_SIZE(pdsn_regex_list)); i++) {
        regfree(&regex_list[i]);
    }
}

pdsn_regex_t* dsn_regex_find(const char *input, regmatch_t *matches, int nmatch)
{
    char errbuf[256];

    int i = 0;
    for (int ret = -1; i < ARRAY_SIZE(regex_list) && (ret = regexec(&regex_list[i], input, nmatch, matches, 0)); i++) {
        regerror(ret, &regex_list[i], errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex exec failed: %s in %s:%d\n", errbuf, __FILE__, __LINE__);
        return NULL;
    }

    return &pdsn_regex_list[i];
}

/**
 * @brief fills dsn member based on index
 * 
 * @param idx 
 * @param matches 
 * @param count 
 * @return 0 or ENOMEM
 */
int dsn_fill_member(const char *in_dsn, _dsn_t *dsn, int idx, pdsn_regex_t* pdsn_regex, regmatch_t *matches, int nmatch)
{   
    char **dsn_hack=(char **)dsn;

    if (idx > (_PDSN_MEMBER_COUNT - 1)) {
        return PDSN_EOOI;
    }

    int n = pdsn_regex->groups[idx];    // grab match group index

    if (n >= nmatch) {  // must not be ge nmatch (buffer overflow)
        return PDSN_EGRPOOI;
    }

    if (matches[n].rm_so != -1) { // match index was found
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
    
    if (n == pdsn_regex->groups[GROUP_PORT] && ! dsn->port) {
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