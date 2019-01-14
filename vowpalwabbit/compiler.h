#pragma once

#ifdef __GNUC__
#define G_GNUC_UNUSED __attribute__((__unused__))
#else
#define G_GNUC_UNUSED
#endif

#define UNUSED_VAR(x) ((void)x)