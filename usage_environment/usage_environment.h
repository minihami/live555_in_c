#ifndef __USAGE_ENVIRONMENT_H__
#define __USAGE_ENVIRONMENT_H__

#include "../usage_environment/usage_environment_version.h"
#include "../groupsock/net_common.h"
#include <stdbool.h>
#include "../livemedia/str_dup.h"
#include "uv.h"

#ifdef __BORLANDC__
#define _setmode setmode
#define _O_BINARY O_BINARY
#endif

typedef void usage_environment__background_handler_proc(void* client_data, int mask /* empty parameter */);
typedef void usage_environment__task_func(void *client_data);

void *usage_environment__groupsock_priv;

#endif
