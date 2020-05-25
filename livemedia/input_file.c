#include "input_file.h"
#include <string.h>


uv_fs_t *livemedia__open_input_file(char const *file_name) {
	uv_fs_t *fid;
	fid = (uv_fs_t *)malloc(sizeof(uv_fs_t));


	/* Chek for a special case file name: "stdin" */
	if (strcmp(file_name, "stdin") == 0) {
		fid = stdin;
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
		_setmode(_fileno(stdin), _O_BINARY); /* convert to binary mode */
#endif
	} else {
		uv_fs_open(uv_default_loop(), fid, file_name, O_RDONLY, O_NONBLOCK, NULL);
		if (fid == NULL) {
			fprintf(stderr, "unable to open file \"%s\" in %s()\n",
					file_name, __func__);
		}
	}

	return fid;
}

void livemedia__close_input_file(FILE* fid) {
  /* Don't close 'stdin', in case we want to use it again later. */
  if (fid != NULL && fid != stdin)
	  fclose(fid);
}

u_int64_t livemedia__get_file_size(char const* file_name, FILE* fid) {
  u_int64_t file_size;
  struct stat sb;

  file_size = 0; /* by default */

  if (fid != stdin) {
#if !defined(_WIN32_WCE)
    if (file_name == NULL) {
#endif
      if (fid != NULL && livemedia__seek_file64(fid, 0, SEEK_END) >= 0) {
	file_size = (u_int64_t)livemedia__tell_file64(fid);
	if (file_size == (u_int64_t)-1) file_size = 0; /* TellFile64() failed */
	livemedia__seek_file64(fid, 0, SEEK_SET);
      }
#if !defined(_WIN32_WCE)
    } else {
      if (stat(file_name, &sb) == 0) {
	file_size = sb.st_size;
      }
    }
#endif
  }

  return file_size;
}

int64_t livemedia__seek_file64(FILE *fid, int64_t offset, int whence) {
  if (fid == NULL)
	  return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _lseeki64(_fileno(fid), offset, whence) == (int64_t)-1 ? -1 : 0;
#else
#if defined(_WIN32_WCE)
  return fseek(fid, (long)(offset), whence);
#else
  return fseeko(fid, (off_t)(offset), whence);
#endif
#endif
}

int64_t livemedia__tell_file64(FILE *fid) {
  if (fid == NULL) return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _telli64(_fileno(fid));
#else
#if defined(_WIN32_WCE)
  return ftell(fid);
#else
  return ftello(fid);
#endif
#endif
}

bool livemedia__file_is_seekable(FILE *fid) {
  if (livemedia__seek_file64(fid, 1, SEEK_CUR) < 0) {
    return false;
  }

  livemedia__seek_file64(fid, -1, SEEK_CUR); // seek back to where we were
  return true;
}
