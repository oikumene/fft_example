#include <stdlib.h>
#include <string.h>
#include "utility.h"

MagickPassFail
string_add_tail(const char *fromstring, const char *addstring, char **tostring)
{
    size_t flen, alen, slen;
    char *head, *suffix, *tmpstr;

    flen = strlen(fromstring);
    alen = strlen(addstring);
    head = malloc(flen + alen + 1);
    if (head == NULL) {
        return (MagickFail);
    }
    strlcpy(head, fromstring, flen + alen + 1);
    suffix = strrchr(fromstring, '.');
    tmpstr = strrchr(head, '.');
    slen = strlen(suffix);
    strlcpy(tmpstr, addstring, alen + 1);
    tmpstr += alen;
    strlcpy(tmpstr, suffix, slen + 1);

    *tostring = head;
    return (MagickPass);
}

double
diff_timespec(struct timespec *ts1, struct timespec *ts0)
{
    double diff = (ts1->tv_sec - ts0->tv_sec) +
        (ts1->tv_nsec - ts0->tv_nsec) * 1.0e-9;
    return (diff);
}
