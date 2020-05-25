#ifndef __LIVEMEDIA_INPUT_FILE_H__
#define __LIVEMEDIA_INPUT_FILE_H__

#include "../usage_environment/usage_environment.h"
#include <stdio.h>

#if (defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE))
#ifndef _WIN32_WCE
// Include header files that might be needed by Windows (in code that uses this header file): */
#include <io.h>
#include <fcntl.h>
#endif

#define READ_FROM_FILES_SYNCHRONOUSLY 1
    /* Because Windows is a silly toy operating system that doesn't (reliably) treat
    * open files as being readable sockets (which can be handled within the default
    * "BasicTaskScheduler" event loop, using "select()"), we implement file reading
    * in Windows using synchronous, rather than asynchronous, I/O.  This can severely
    * limit the scalability of servers using this code that run on Windows.
    * If this is a problem for you, then either use a better operating system,
    * or else write your own Windows-specific event loop ("TaskScheduler" subclass)
    * that can handle readable data in Windows open files as an event.*/
#endif

#ifndef _WIN32_WCE
#include <sys/stat.h>
#endif

uv_fs_t *livemedia__open_input_file(char const *file_name);

void livemedia__close_input_file(FILE* fid);

#undef GetFileSize /*because some platforms already define this as a macro */
u_int64_t livemedia__get_file_size(char const *file_name, FILE* fid);
/* 0 means zero-length, unbounded, or unknown */

int64_t livemedia__seek_file64(FILE *fid, int64_t offset, int whence);

    /* A platform-independent routine for seeking within (possibly) large files */

int64_t livemedia__tell_file64(FILE *fid);
    /* A platform-independent routine for reporting the position within
    * (possibly) large files */

bool livemedia__file_is_seekable(FILE *fid);
    /* Tests whether "fid" is seekable, by trying to seek within it. */

#endif
