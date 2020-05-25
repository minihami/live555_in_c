
#ifndef __LIVEMEDIA_BYTE_STREAM_FILE_SOURCE_H__
#define __LIVEMEDIA_BYTE_STREAM_FILE_SOURCE_H__
#include "framed_file_source.h"

/*****************************************************************
 * ByteStreamFileSource:FramedFileSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_byte_stream_file_source_t {
	livemedia_framed_file_source_t framed_file_source;

	/* member variables */
	u_int64_t file_size;
	unsigned int preferred_frame_size;
	unsigned int play_time_per_frame;
	bool fid_is_seekable;
	unsigned int last_play_time;
	bool have_started_reading;
	bool limit_num_bytes_to_stream;
	u_int64_t num_bytes_to_stream;	/* used iff "fLimitNumBytesToStream" is True */

} livemedia_byte_stream_file_source_t;

/*****************************************************************
 * ByteStreamFileSource:FramedFileSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_byte_stream_file_source_t *__livemedia_byte_stream_file_source__alloc(void);

void __livemedia_byte_stream_file_source__init(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		uv_fs_t *fid,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame);

void __livemedia_byte_stream_file_source__deinit(
		livemedia_byte_stream_file_source_t *byte_stream_file_source);

void __livemedia_byte_stream_file_source__free(
		livemedia_byte_stream_file_source_t *byte_stream_file_source);
/*
 * new() and delete() functions
 */
livemedia_byte_stream_file_source_t *livemedia__new__byte_stream_file_source(
		uv_fs_t *fid,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame);
	/* called only by createNew() */

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_byte_stream_file_source__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source);

void livemedia_byte_stream_file_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source);

void livemedia_byte_stream_file_source__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
u_int64_t livemedia_byte_stream_file_source__file_size(
		livemedia_byte_stream_file_source_t *byte_stream_file_source);
      /* 0 means zero-length, unbounded, or unknown */

/*
 * Normal functions
 */

livemedia_byte_stream_file_source_t *livemedia_byte_stream_file_source__create_new_by_file_name__static(
		char const *file_name,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame);
  /* "preferredFrameSize" == 0 means 'no preference'
   * "playTimePerFrame" is in microseconds */

livemedia_byte_stream_file_source_t *livemedia_byte_stream_file_source__create_new_by_file_ptr__static(
		uv_fs_t *fid,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame);
      /* an alternative version of "createNew()" that's used if you already have
       * an open file. */

void livemedia_byte_stream_file_source__seek_to_byte_absolute(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		u_int64_t byte_number, u_int64_t num_bytes_to_stream);
    /* if "numBytesToStream" is >0, then we limit the stream to that number of bytes, before treating it as EOF */

void livemedia_byte_stream_file_source__seek_to_byte_ralative(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		int64_t offset, u_int64_t num_bytes_to_stream);

void livemedia_byte_stream_file_source__seek_to_end(
		livemedia_byte_stream_file_source_t *byte_stream_file_source);
/* to force EOF handling on the next read */

void livemedia_byte_stream_file_source__file_readable_handler__static(
		livemedia_byte_stream_file_source_t *source, int mask);

void livemedia_byte_stream_file_source__do_read_from_file(
		livemedia_byte_stream_file_source_t *byte_stream_file_source);

#endif

