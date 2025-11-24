#include <time.h>
#include <magick/common.h>

MagickPassFail
string_add_tail(const char *fromstring, const char *addstring, char **tostring);

double diff_timespec(struct timespec *ts1, struct timespec *ts0);
