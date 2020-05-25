
#include "mpeg_video_stream_parser.h"
#include "../groupsock/groupsock_helper.h"

/*****************************************************************
 * TimeCode (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_time_code_t *__livemedia_time_code__alloc(void)
{
	livemedia_time_code_t *time_code;
	time_code = (livemedia_time_code_t *)calloc(1, sizeof(livemedia_time_code_t));
	if (time_code == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_time_code_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return time_code;
}
void __livemedia_time_code__init(livemedia_time_code_t *time_code)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	time_code->delete = livemedia_time_code__delete__impl;
}
void __livemedia_time_code__deinit(livemedia_time_code_t *time_code)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_time_code__free(livemedia_time_code_t *time_code)
{
	if (time_code) {
		free(time_code);
		time_code = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_time_code_t *livemedia__new__time_code(void)
{
	livemedia_time_code_t *time_code;

	time_code = __livemedia_time_code__alloc();
	if (time_code)
		__livemedia_time_code__init(time_code);

	return time_code;
}
void livemedia__delete__time_code(livemedia_time_code_t *time_code)
{
	if (time_code && time_code->delete)
		time_code->delete(time_code);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_time_code__delete__impl(livemedia_time_code_t *time_code)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (time_code) {
		__livemedia_time_code__deinit(time_code);
		__livemedia_time_code__free(time_code);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * MPEGVideoStreamFramer:FramedFilter (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mpeg_video_stream_framer_t *__livemedia_mpeg_video_stream_framer__alloc(void)
{
	livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer;
	mpeg_video_stream_framer = (livemedia_mpeg_video_stream_framer_t *)calloc(1, sizeof(livemedia_mpeg_video_stream_framer_t));
	if (mpeg_video_stream_framer == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_mpeg_video_stream_framer_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return mpeg_video_stream_framer;
}
void __livemedia_mpeg_video_stream_framer__init(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		livemedia_framed_source_t *input_source)
{
	__livemedia_framed_filter__init(&mpeg_video_stream_framer->framed_filter,
			input_source);

	mpeg_video_stream_framer->frame_rate = 0.0; /* until we learn otherwise */
	mpeg_video_stream_framer->parser = NULL;

	livemedia_mpeg_video_stream_framer__reset(mpeg_video_stream_framer);

	((livemedia_framed_source_t *)mpeg_video_stream_framer)->do_get_next_frame =
		livemedia_mpeg_video_stream_framer__do_get_next_frame__impl;
	((livemedia_framed_source_t *)mpeg_video_stream_framer)->do_stop_getting_frames =
		livemedia_mpeg_video_stream_framer__do_stop_getting_frames__impl;

	((livemedia_medium_t *)mpeg_video_stream_framer)->delete = livemedia_mpeg_video_stream_framer__delete__impl;
}
void __livemedia_mpeg_video_stream_framer__deinit(livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
	livemedia__delete__stream_parser(
			(livemedia_stream_parser_t *)mpeg_video_stream_framer->parser);

	__livemedia_framed_filter__deinit(&mpeg_video_stream_framer->framed_filter);

}
void __livemedia_mpeg_video_stream_framer__free(livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
	if (mpeg_video_stream_framer) {
		free(mpeg_video_stream_framer);
		mpeg_video_stream_framer = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_mpeg_video_stream_framer_t *livemedia__new__mpeg_video_stream_framer(
		livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer;

	mpeg_video_stream_framer = __livemedia_mpeg_video_stream_framer__alloc();
	if (mpeg_video_stream_framer)
		__livemedia_mpeg_video_stream_framer__init(mpeg_video_stream_framer, input_source);

	return mpeg_video_stream_framer;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mpeg_video_stream_framer__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer;
	mpeg_video_stream_framer = (livemedia_mpeg_video_stream_framer_t *)framed_source;

	livemedia_mpeg_video_stream_parser__register_read_interest(
			mpeg_video_stream_framer->parser, framed_source->to, framed_source->max_size);

	livemedia_mpeg_video_stream_framer__continue_read_processing(
			mpeg_video_stream_framer);
}

void livemedia_mpeg_video_stream_framer__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer;

	mpeg_video_stream_framer = (livemedia_mpeg_video_stream_framer_t *)framed_source;
	livemedia_mpeg_video_stream_framer__flush_input(mpeg_video_stream_framer);
	livemedia_framed_filter__do_stop_getting_frames__impl(framed_source);
}

void livemedia_mpeg_video_stream_framer__delete__impl(
		livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer;

	mpeg_video_stream_framer = (livemedia_mpeg_video_stream_framer_t *)medium;
	if (mpeg_video_stream_framer) {
		__livemedia_mpeg_video_stream_framer__deinit(mpeg_video_stream_framer);
		__livemedia_mpeg_video_stream_framer__free(mpeg_video_stream_framer);
	}
}

/*
 * Functions in header file
 */
bool *livemedia_mpeg_video_stream_framer__picture_end_marker(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
      /* a hack for implementing the RTP 'M' bit */
	return &mpeg_video_stream_framer->picture_end_marker;

}

/*
 * Normal functions
 */
void livemedia_mpeg_video_stream_framer__flush_input(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer__reset(mpeg_video_stream_framer);
	if (mpeg_video_stream_framer->parser != NULL)
		livemedia_stream_parser__flush_input(
				(livemedia_stream_parser_t *)mpeg_video_stream_framer->parser);

}

void livemedia_mpeg_video_stream_framer__compute_presentation_time(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		unsigned num_additional_pictures)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/* sets "fPresentationTime */
void livemedia_mpeg_video_stream_framer__set_time_code(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		unsigned int hours, unsigned int minutes, unsigned int seconds,
		unsigned int pictures, unsigned int pictures_since_last_gop)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_mpeg_video_stream_framer__reset(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	mpeg_video_stream_framer->picture_count = 0;
	mpeg_video_stream_framer->picture_end_marker = true;
	/* So that we start looking as if we'd just ended an 'acces unit' */
	mpeg_video_stream_framer->pictures_adjustment = 0;
	mpeg_video_stream_framer->picture_time_base = 0.0;
	mpeg_video_stream_framer->tc_secs_base = 0;
	mpeg_video_stream_framer->have_seen_first_time_code = false;

	/* Clear the 'presentation time base', as a signal for subclasses
	 * to reset it (to our current time) when we start (or resume) streaming: */
	mpeg_video_stream_framer->presentation_time_base.tv_sec = 0;
	mpeg_video_stream_framer->presentation_time_base.tv_usec = 0;
}

void livemedia_mpeg_video_stream_framer__continue_read_processing__static(
		void* client_data, unsigned char *ptr, unsigned int size,
		struct timeval presentation_time)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_framer_t *framer;

	framer = (livemedia_mpeg_video_stream_framer_t *)client_data;
	livemedia_mpeg_video_stream_framer__continue_read_processing(framer);
}

void livemedia_mpeg_video_stream_framer__continue_read_processing(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int acquired_frame_size;
	livemedia_framed_source_t *framed_source;

	framed_source = (livemedia_framed_source_t *)mpeg_video_stream_framer;

	acquired_frame_size = livemedia_mpeg_video_stream_parser__parse(
			mpeg_video_stream_framer->parser);
	if (acquired_frame_size > 0) {
    /* We were able to acquire a frame from the input.
     * It has already been copied to the reader's space. */
		framed_source->frame_size = acquired_frame_size;
		framed_source->num_truncated_bytes =
			livemedia_mpeg_video_stream_parser__num_truncated_bytes(
					mpeg_video_stream_framer->parser);

		/* "fPresentationTime" should have already been computed. */

		/* Compute "fDurationInMicroseconds" now: */
		framed_source->duration_in_microseconds
			= (mpeg_video_stream_framer->frame_rate == 0.0 || 
					((int)mpeg_video_stream_framer->picture_count) < 0) ? 0
			: (unsigned int)((mpeg_video_stream_framer->picture_count * 1000000) /
					mpeg_video_stream_framer->frame_rate);
#ifdef DEBUG
		fprintf(stderr, "%d bytes @%u.%06d, fDurationInMicroseconds: %d ((%d*1000000)/%f)\n", acquired_frame_size, framed_source->presentation_time.tv_sec, framed_source->presentation_time.tv_usec, framed_source->duration_in_microseconds, mpeg_video_stream_framer->picture_count, mpeg_video_stream_framer->frame_rate);
#endif
		mpeg_video_stream_framer->picture_count = 0;

    /* Call our own 'after getting' function.  Because we're not a 'leaf'
     * source, we can call this directly, without risking infinite recursion. */
		livemedia_framed_source__after_getting__static(framed_source);
	} else {
    /* We were unable to parse a complete frame from the input, because:
     * - we had to read more data from the source stream, or
     * - the source stream has ended. */
	}
}


/*
 * Static functions implementation for ancestor
 */
