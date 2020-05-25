#include "byte_stream_file_source.h"
#include "input_file.h"
#include "groupsock_helper.h"

/*****************************************************************
 * ByteStreamFileSource:FramedFileSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_byte_stream_file_source_t *__livemedia_byte_stream_file_source__alloc(void)
{
	livemedia_byte_stream_file_source_t *byte_stream_file_source;
	byte_stream_file_source = (livemedia_byte_stream_file_source_t *)calloc(1, sizeof(livemedia_byte_stream_file_source_t));
	if (byte_stream_file_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_byte_stream_file_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return byte_stream_file_source;
}
void __livemedia_byte_stream_file_source__init(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		uv_fs_t *fid,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame)
{
	FILE *seek_fid;

	__livemedia_framed_file_source__init(&byte_stream_file_source->framed_file_source, fid);

	byte_stream_file_source->file_size = 0;
	byte_stream_file_source->preferred_frame_size = preferred_frame_size;
	byte_stream_file_source->play_time_per_frame = play_time_per_frame;
	byte_stream_file_source->last_play_time = 0;
	byte_stream_file_source->have_started_reading = false;
	byte_stream_file_source->limit_num_bytes_to_stream = false;
	byte_stream_file_source->num_bytes_to_stream = 0;
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
	groupsock__make_socket_non_blocking((((livemedia_framed_file_source_t *)byte_stream_file_source)->fid)->result);
#endif
	
	/* Test whether the file is seekable */
	seek_fid = fdopen(
				((livemedia_framed_file_source_t *)byte_stream_file_source)->fid->result,
				"rb");
	byte_stream_file_source->fid_is_seekable =
		livemedia__file_is_seekable(seek_fid);
				//((livemedia_framed_file_source_t *)byte_stream_file_source)->fid);

	((livemedia_framed_source_t *)byte_stream_file_source)->do_get_next_frame = 
		livemedia_byte_stream_file_source__do_get_next_frame__impl;
	((livemedia_framed_source_t *)byte_stream_file_source)->do_stop_getting_frames = 
		livemedia_byte_stream_file_source__do_stop_getting_frames__impl;
	((livemedia_medium_t *)byte_stream_file_source)->delete = livemedia_byte_stream_file_source__delete__impl;
}
void __livemedia_byte_stream_file_source__deinit(livemedia_byte_stream_file_source_t *byte_stream_file_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	FILE *file_fp;
	uv_idle_t *idler;

	if (((livemedia_framed_file_source_t *)byte_stream_file_source)->fid == NULL)
		return;
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
	/* turnOnBackgroundReadHandling not call -  
	envir().taskScheduler().turnOffBackgroundReadHandling(fileno(fFid));*/
	idler = (uv_idle_t *)((livemedia_medium_t *)byte_stream_file_source)->data;
	if (idler) {
		uv_idle_stop(idler);
		free(idler);
		((livemedia_medium_t *)byte_stream_file_source)->data = NULL;
	}
#endif

	file_fp = fdopen(((livemedia_framed_file_source_t *)byte_stream_file_source)->fid->result, "rb");
	livemedia__close_input_file(file_fp);

	__livemedia_framed_file_source__deinit(&byte_stream_file_source->framed_file_source);
}
void __livemedia_byte_stream_file_source__free(livemedia_byte_stream_file_source_t *byte_stream_file_source)
{
	if (byte_stream_file_source) {
		free(byte_stream_file_source);
		byte_stream_file_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_byte_stream_file_source_t *livemedia__new__byte_stream_file_source(
		uv_fs_t *fid,
		unsigned int preferred_frame_size,
		unsigned int play_time_per_frame)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_byte_stream_file_source_t *byte_stream_file_source;

	byte_stream_file_source = __livemedia_byte_stream_file_source__alloc();
	if (byte_stream_file_source)
		__livemedia_byte_stream_file_source__init(byte_stream_file_source,
				fid, preferred_frame_size, play_time_per_frame);

	return byte_stream_file_source;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */

static void do_get_next_frame_idle_cb(uv_idle_t *handle)
{
	livemedia_byte_stream_file_source_t *byte_stream_file_source;

	byte_stream_file_source = (livemedia_byte_stream_file_source_t *)handle->data;

	livemedia_byte_stream_file_source__file_readable_handler__static(
			byte_stream_file_source, 0);
}
void livemedia_byte_stream_file_source__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	uv_idle_t *idler;

	livemedia_framed_file_source_t *framed_file_source;
	livemedia_byte_stream_file_source_t *byte_stream_file_source;
	FILE *file_source_fp;


	framed_file_source = (livemedia_framed_file_source_t *)framed_source;
	byte_stream_file_source = (livemedia_byte_stream_file_source_t *)framed_source;

	file_source_fp =
		fdopen(framed_file_source->fid->result, "rb");

	if (feof(file_source_fp) || ferror(file_source_fp) ||
			(byte_stream_file_source->limit_num_bytes_to_stream &&
			 byte_stream_file_source->num_bytes_to_stream == 0)) {
		livemedia_framed_source__handle_closure(framed_source);
		return;
	}

#ifdef READ_FROM_FILES_SYNCHRONOUSLY
	livemedia_byte_stream_file_source__do_read_from_file(byte_stream_file_source);
#else
	if (!byte_stream_file_source->have_started_reading) {
		/* Await readable data from the file: */

		if (framed_file_source->fid->result > 0) {
			idler = (uv_idle_t *)malloc(sizeof(uv_idle_t));
			uv_idle_init(uv_default_loop(), idler);
			idler->data = (void *)byte_stream_file_source;
			((livemedia_medium_t *)byte_stream_file_source)->data = (void *)idler;
			uv_idle_start(idler, do_get_next_frame_idle_cb);
			byte_stream_file_source->have_started_reading = true;
		}
	}
#endif
}

void livemedia_byte_stream_file_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	uv_idle_t *idler;

	uv_timer_stop(livemedia_medium__next_task((livemedia_medium_t *)framed_source));
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
	idler = (uv_idle_t *)((livemedia_medium_t *)framed_source)->data;
	if (idler) {
		uv_idle_stop(idler);
		free(idler);
		((livemedia_medium_t *)framed_source)->data = NULL;
	}
	((livemedia_byte_stream_file_source_t *)framed_source)->have_started_reading = false;
#endif
}

void livemedia_byte_stream_file_source__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_byte_stream_file_source_t *byte_stream_file_source;
	byte_stream_file_source = (livemedia_byte_stream_file_source_t *)medium;

	if (byte_stream_file_source) {
		__livemedia_byte_stream_file_source__deinit(byte_stream_file_source);
		__livemedia_byte_stream_file_source__free(byte_stream_file_source);
	}
}

/*
 * Functions in header file
 */
u_int64_t livemedia_byte_stream_file_source__file_size(
		livemedia_byte_stream_file_source_t *byte_stream_file_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* 0 means zero-length, unbounded, or unknown */
	return byte_stream_file_source->file_size;
}

/*
 * Normal functions
 */

livemedia_byte_stream_file_source_t *livemedia_byte_stream_file_source__create_new_by_file_name__static(
		char const *file_name,
		unsigned int preferred_frame_size/*defalut: 0 */,
		unsigned int play_time_per_frame/*defalut: 0 */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	uv_fs_t *fid;
	livemedia_byte_stream_file_source_t *new_source;
	FILE *file_fp;
	
	fid = livemedia__open_input_file(file_name);
	if (fid == NULL)
		return NULL;

	file_fp = fdopen(fid->result, "rb");
	new_source = livemedia__new__byte_stream_file_source(fid, preferred_frame_size,
			play_time_per_frame);
	new_source->file_size = livemedia__get_file_size(file_name, file_fp);

	return new_source;
}

livemedia_byte_stream_file_source_t *livemedia_byte_stream_file_source__create_new_by_file_ptr__static(
		uv_fs_t *fid,
		unsigned int preferred_frame_size/*defalut: 0 */,
		unsigned int play_time_per_frame/*defalut: 0 */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_byte_stream_file_source_t *new_source;
	FILE *file_fp;

	if (fid == NULL)
		return NULL;

	file_fp = fdopen(fid->result, "rb");
	new_source = livemedia__new__byte_stream_file_source(fid, preferred_frame_size,
			play_time_per_frame);
	new_source->file_size = livemedia__get_file_size(NULL, file_fp);

	return new_source;
}

void livemedia_byte_stream_file_source__seek_to_byte_absolute(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		u_int64_t byte_number, u_int64_t num_bytes_to_stream)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_byte_stream_file_source__seek_to_byte_ralative(
		livemedia_byte_stream_file_source_t *byte_stream_file_source,
		int64_t offset, u_int64_t num_bytes_to_stream)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_byte_stream_file_source__seek_to_end(
		livemedia_byte_stream_file_source_t *byte_stream_file_source)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_byte_stream_file_source__file_readable_handler__static(
		livemedia_byte_stream_file_source_t *source, int mask)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (!livemedia_framed_source__is_currently_awaiting_data(
				(livemedia_framed_source_t *)source)) {
		livemedia_framed_source__do_stop_getting_frames(
				(livemedia_framed_source_t *)source);
		/* we're not ready for the data yet */
		return;
	}
	livemedia_byte_stream_file_source__do_read_from_file(source);
}
void do_read_from_file_timer_cb(uv_timer_t *handle)
{
	livemedia_framed_source_t *framed_source;

	framed_source = (livemedia_framed_source_t *)handle->data;
	livemedia_framed_source__after_getting__static(framed_source);

}

void livemedia_byte_stream_file_source__do_read_from_file(
		livemedia_byte_stream_file_source_t *byte_stream_file_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_source_t *framed_source;
	livemedia_framed_file_source_t *framed_file_source;
	FILE *file_fp;
	unsigned int u_seconds;
	uv_timer_t *next_task;

	framed_source = (livemedia_framed_source_t *)byte_stream_file_source;
	framed_file_source = (livemedia_framed_file_source_t *)byte_stream_file_source;
	file_fp = fdopen(framed_file_source->fid->result, "rb");


  /* Try to read as many bytes as will fit in the buffer provided
   * (or "fPreferredFrameSize" if less) */
	if (byte_stream_file_source->preferred_frame_size &&
			byte_stream_file_source->num_bytes_to_stream < (u_int64_t)framed_source->max_size) {
		framed_source->max_size =
			(unsigned int)byte_stream_file_source->num_bytes_to_stream;
	}
	if (byte_stream_file_source->preferred_frame_size > 0 &&
			byte_stream_file_source->preferred_frame_size < framed_source->max_size) {
		framed_source->max_size = byte_stream_file_source->preferred_frame_size;
	}
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
	framed_source->frame_size = fread(framed_source->to, 1, framed_source->max_size, 
			file_fp);
#else
	if (byte_stream_file_source->fid_is_seekable) {
	framed_source->frame_size = fread(framed_source->to, 1, framed_source->max_size, 
			file_fp);
	} else {
		/* For non-seekable files (e.g., pipes), call "read()" rather than "fread()", to ensure that the read doesn't block: */
		framed_source->frame_size = read(framed_file_source->fid->result, 
				framed_source->to, framed_source->max_size);
	}
#endif
	if (framed_source->frame_size == 0) {
		livemedia_framed_source__handle_closure(framed_source);
		return;
	}
	byte_stream_file_source->num_bytes_to_stream -= framed_source->frame_size;

	/* Set the 'presentation time': */
	if (byte_stream_file_source->play_time_per_frame > 0 && 
			byte_stream_file_source->preferred_frame_size > 0) {
		if (framed_source->presentation_time.tv_sec == 0 &&
				framed_source->presentation_time.tv_usec == 0) {
			/* This is the first frame, so use the current time: */
			gettimeofday(&framed_source->presentation_time, NULL);
		} else {
			/* Increment by the play time of the previous data: */
			u_seconds = framed_source->presentation_time.tv_usec + 
				byte_stream_file_source->last_play_time;
			framed_source->presentation_time.tv_sec += u_seconds/1000000;
			framed_source->presentation_time.tv_usec = u_seconds%1000000;
		}

		/* Remember the play time of this data: */
		byte_stream_file_source->last_play_time = 
			(byte_stream_file_source->play_time_per_frame * framed_source->frame_size) / 
			byte_stream_file_source->preferred_frame_size;
		framed_source->duration_in_microseconds = byte_stream_file_source->last_play_time;
	} else {
		/* We don't know a specific play time duration for this data,
		 * so just record the current time as being the 'presentation time': */
		gettimeofday(&framed_source->presentation_time, NULL);
	}

  /* Inform the reader that he has data: */
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
  /* To avoid possible infinite recursion, we need to return to the event loop to do this: */
	next_task = livemedia_medium__next_task((livemedia_medium_t *)byte_stream_file_source);
	next_task->data = (void *)byte_stream_file_source;
	uv_timer_start(next_task, do_read_from_file_timer_cb, 0, 0);
#else
  /* Because the file read was done from the event loop, we can call the
   * 'after getting' function directly, without risk of infinite recursion: */
	livemedia_framed_source__after_getting__static(framed_source);
#endif
				
}
