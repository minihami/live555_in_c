#include "media_sink.h"

/*****************************************************************
 * MediaSink:Medium (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_media_sink_t *__livemedia_media_sink__alloc(void)
{
	livemedia_media_sink_t *media_sink;
	media_sink = (livemedia_media_sink_t *)calloc(1, sizeof(livemedia_media_sink_t));
	if (media_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_media_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return media_sink;
}
void __livemedia_media_sink__init(livemedia_media_sink_t *media_sink)
{
	livemedia_medium_t *medium;

	__livemedia_medium__init(&media_sink->medium);

	media_sink->source = NULL;
	
	/* virtual functions configuration */ 
	medium = (livemedia_medium_t *)media_sink;

	media_sink->stop_playing = livemedia_media_sink__stop_playing__impl;
	media_sink->is_rtp_sink = livemedia_media_sink__is_rtp_sink__impl;
	media_sink->source_is_compatible_with_us = livemedia_media_sink__source_is_compatible_with_us__impl;
	media_sink->continue_playing = NULL;
	medium->is_sink = livemedia_media_sink__is_sink__impl;
	medium->delete = livemedia_media_sink__delete__impl;
}
void __livemedia_media_sink__deinit(livemedia_media_sink_t *media_sink)
{
	livemedia_media_sink__stop_playing(media_sink);

	__livemedia_medium__deinit(&media_sink->medium);
}
void __livemedia_media_sink__free(livemedia_media_sink_t *media_sink)
{
	if (media_sink) {
		free(media_sink);
		media_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_media_sink_t *livemedia__new__media_sink(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_media_sink_t *media_sink;

	media_sink = __livemedia_media_sink__alloc();
	if (media_sink)
		__livemedia_media_sink__init(media_sink);

	return media_sink;
}
/*
 * Functions for virtual functions
 */
void livemedia_media_sink__stop_playing(livemedia_media_sink_t *media_sink)
{
	if (media_sink && media_sink->stop_playing)
		media_sink->stop_playing(media_sink);
}

bool livemedia_media_sink__is_rtp_sink(livemedia_media_sink_t *media_sink)
{
	if (media_sink && media_sink->is_rtp_sink)
		return media_sink->is_rtp_sink(media_sink);
	return false;
}

bool livemedia_media_sink__source_is_compatible_with_us(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *media_source)
{
	if (media_sink && media_sink->is_rtp_sink)
		return media_sink->source_is_compatible_with_us(media_sink, media_source);
	return false;
}

bool livemedia_media_sink__continue_playing(livemedia_media_sink_t *media_sink)
{
	if (media_sink && media_sink->continue_playing)
		return media_sink->continue_playing(media_sink);
	return false;
}

/*
 * Override functions
 */
void livemedia_media_sink__stop_playing__impl(livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_medium_t *medium;

	/* First, tell the source that we're no longer interested: */
	if (media_sink->source != NULL)
		livemedia_framed_source__stop_getting_frames(media_sink->source);

	/* Cancel any pending tasks: */
	medium = (livemedia_medium_t *)media_sink;
	uv_timer_stop(livemedia_medium__next_task(medium));

	media_sink->source = NULL;		/* indicates that we can be played again */
	media_sink->after_func = NULL;
}

bool livemedia_media_sink__is_rtp_sink__impl(livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false;	/* defalut implementation */
}

bool livemedia_media_sink__source_is_compatible_with_us__impl(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* We currently support only framed sources. */
	return livemedia_media_source__is_framed_source(source);
}

bool livemedia_media_sink__is_sink__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}

void livemedia_media_sink__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_media_sink_t *media_sink;

	media_sink = (livemedia_media_sink_t *)medium;
	if (media_sink) {
		__livemedia_media_sink__deinit(media_sink);
		__livemedia_media_sink__free(media_sink);
	}
}

/*
 * Functions in header file
 */

livemedia_framed_source_t *livemedia_media_sink__source(livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return media_sink->source;
}

/*
 * Normal functions
 */
bool livemedia_media_sink__lookup_by_name__static(char const *sink_name, 
		livemedia_media_sink_t **result_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_medium_t *medium;

	*result_sink = NULL; /* unless we succeed */

	if(!livemedia_medium__lookup_by_name__static(sink_name, &medium))
		return false;

	*result_sink = (livemedia_media_sink_t *)medium;
	return true;
	
}

bool livemedia_media_sink__start_playing(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *source, livemedia_after_plying_func *after_func,
		void *after_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Make sure we're not already being played: */
	if (media_sink->source != NULL) {
		fprintf(stderr, "This sink is already being played in %s()\n", __func__);
		return false;
	}

  /* Make sure our source is compatible: */
	if (!livemedia_media_sink__source_is_compatible_with_us(media_sink, source)) {
		fprintf(stderr, "MediaSink::startPlaying(): source is not compatible! in %s()\n", __func__);
		return false;
	}
	media_sink->source = (livemedia_framed_source_t *)source;

	media_sink->after_func = after_func;
	media_sink->after_client_data = after_client_data;
	return livemedia_media_sink__continue_playing(media_sink);
}

void livemedia_media_sink__on_source_closure__static(void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_media_sink_t *sink;
	sink = (livemedia_media_sink_t *)client_data;
	livemedia_media_sink__on_source_closure(sink);
}

void livemedia_media_sink__on_source_closure(livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_medium_t *medium;

  /* Cancel any pending tasks: */
	medium = (livemedia_medium_t *)media_sink;
	uv_timer_stop(livemedia_medium__next_task(medium));

	media_sink->source = NULL; /* indicates that we can be played again */
	if (media_sink->after_func != NULL)
		media_sink->after_func(media_sink->after_client_data);
}

/*****************************************************************
 * OutPacketBuffer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_out_packet_buffer_t *__livemedia_out_packet_buffer__alloc(void)
{
	livemedia_out_packet_buffer_t *out_packet_buffer;
	out_packet_buffer = (livemedia_out_packet_buffer_t *)calloc(1, sizeof(livemedia_out_packet_buffer_t));
	if (out_packet_buffer == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_out_packet_buffer_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return out_packet_buffer;
}

void __livemedia_out_packet_buffer__init(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int preferred_packet_size,
		unsigned int max_packet_size, unsigned int max_buffer_size)
{

	unsigned int max_num_packets;

	out_packet_buffer->preferred = preferred_packet_size;
	out_packet_buffer->max = max_packet_size;
	out_packet_buffer->overflow_data_size = 0;
	out_packet_buffer->max_size = &out_packet_buffer__max_size;

	if(max_buffer_size == 0)
		max_buffer_size = out_packet_buffer__max_size;
	max_num_packets = (max_buffer_size + (max_packet_size - 1))/max_packet_size;
	out_packet_buffer->limit = max_num_packets * max_packet_size;
	out_packet_buffer->buf = (unsigned char *)malloc(
			sizeof(unsigned char)*out_packet_buffer->limit);
	if (out_packet_buffer->buf == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char)*out_packet_buffer->limit)"
				"failed in %s()\n", __func__);
		return ;
	}
	livemedia_out_packet_buffer__reset_packet_start(out_packet_buffer);
	livemedia_out_packet_buffer__reset_offset(out_packet_buffer);
	livemedia_out_packet_buffer__reset_overflow_data(out_packet_buffer);

	/* virtual functions configuration */ 
	out_packet_buffer->delete = livemedia_out_packet_buffer__delete__impl;
}
void __livemedia_out_packet_buffer__deinit(livemedia_out_packet_buffer_t *out_packet_buffer)
{
	if (out_packet_buffer->buf) {
		free(out_packet_buffer->buf);
		out_packet_buffer->buf = NULL;
	}
}
void __livemedia_out_packet_buffer__free(livemedia_out_packet_buffer_t *out_packet_buffer)
{
	if (out_packet_buffer) {
		free(out_packet_buffer);
		out_packet_buffer = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_out_packet_buffer_t *livemedia__new__out_packet_buffer(
		unsigned int preferred_packet_size,
		unsigned int max_packet_size, unsigned int max_buffer_size/* defalut :0 */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_out_packet_buffer_t *out_packet_buffer;

	out_packet_buffer = __livemedia_out_packet_buffer__alloc();
	if (out_packet_buffer)
		__livemedia_out_packet_buffer__init(out_packet_buffer,
				preferred_packet_size, max_packet_size, max_buffer_size);

	return out_packet_buffer;
}

void livemedia__delete__out_packet_buffer(livemedia_out_packet_buffer_t *out_packet_buffer)
{
	if (out_packet_buffer && out_packet_buffer->delete)
		out_packet_buffer->delete(out_packet_buffer);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_out_packet_buffer__delete__impl(livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (out_packet_buffer) {
		__livemedia_out_packet_buffer__deinit(out_packet_buffer);
		__livemedia_out_packet_buffer__free(out_packet_buffer);
	}
}

/*
 * Functions in header file
 */
void livemedia_out_packet_buffer__increase_max_size_to__static(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int new_max_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (new_max_size > out_packet_buffer__max_size)
		out_packet_buffer__max_size = new_max_size;
}

unsigned char *livemedia_out_packet_buffer__cur_ptr(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &out_packet_buffer->buf[out_packet_buffer->packet_start +
		out_packet_buffer->cur_offset];
}

unsigned int livemedia_out_packet_buffer__total_bytes_available(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (out_packet_buffer->limit -
		(out_packet_buffer->packet_start + out_packet_buffer->cur_offset));
}

unsigned int livemedia_out_packet_buffer__total_buffer_size(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->limit;
}

unsigned char *livemedia_out_packet_buffer__packet(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &out_packet_buffer->buf[out_packet_buffer->packet_start];
}

unsigned int livemedia_out_packet_buffer__cur_packet_size(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->cur_offset;
}

void livemedia_out_packet_buffer__increment(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	out_packet_buffer->cur_offset += num_bytes;
}

bool livemedia_out_packet_buffer__is_preferred_size(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->cur_offset >= out_packet_buffer->preferred;
}

bool livemedia_out_packet_buffer__would_overflow(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (out_packet_buffer->cur_offset + num_bytes) > out_packet_buffer->max;
}

unsigned int livemedia_out_packet_buffer__num_overflow_bytes(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (out_packet_buffer->cur_offset + num_bytes) - out_packet_buffer->max;
}

bool livemedia_out_packet_buffer__is_too_big_for_a_packet(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return num_bytes > out_packet_buffer->max;
}

unsigned int livemedia_out_packet_buffer__overflow_data_size(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->overflow_data_size;
}

struct timeval livemedia_out_packet_buffer__overflow_presentation_time(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->overflow_presentation_time;
}

unsigned int livemedia_out_packet_buffer__overflow_duration_in_microseconds(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->overflow_duration_in_microseconds;
}

bool livemedia_out_packet_buffer__have_overflow_data(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return out_packet_buffer->overflow_data_size > 0 ;
}

void livemedia_out_packet_buffer__reset_offset(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	out_packet_buffer->cur_offset = 0;
}

void livemedia_out_packet_buffer__reset_overflow_data(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	out_packet_buffer->overflow_data_offset = 0;
	out_packet_buffer->overflow_data_size = 0;
}


/*
 * Normal functions
 */
void livemedia_out_packet_buffer__enqueue(
		livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char const *from, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (num_bytes >
			livemedia_out_packet_buffer__total_bytes_available(out_packet_buffer)) {
#ifdef DEBUG
		fprintf(stderr, "OutPacketBuffer::enqueue() warning: %d > %d\n", num_bytes,
				livemedia_out_packet_buffer__total_bytes_available(out_packet_buffer));
#endif
		num_bytes = livemedia_out_packet_buffer__total_bytes_available(out_packet_buffer);
	}
	
	if (livemedia_out_packet_buffer__cur_ptr(out_packet_buffer) != from) 
		memmove(livemedia_out_packet_buffer__cur_ptr(out_packet_buffer), from, num_bytes);
	livemedia_out_packet_buffer__increment(out_packet_buffer, num_bytes);
}

void livemedia_out_packet_buffer__enqueue_word(
		livemedia_out_packet_buffer_t *out_packet_buffer, u_int32_t word)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int32_t n_word;

	n_word = htonl(word);
	livemedia_out_packet_buffer__enqueue(out_packet_buffer, (unsigned char *)&n_word, 4);
}

void livemedia_out_packet_buffer__insert(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char const *from, unsigned int num_bytes, unsigned int to_position)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int real_to_position ;
	
	real_to_position = out_packet_buffer->packet_start + to_position;
	if (real_to_position + num_bytes > out_packet_buffer->limit) {
		if (real_to_position > out_packet_buffer->limit)
			return;		/* we can't do this */
		num_bytes = out_packet_buffer->limit - real_to_position;
	}

	memmove(&out_packet_buffer->buf[real_to_position], from, num_bytes);
	if (to_position + num_bytes > out_packet_buffer->cur_offset)
		out_packet_buffer->cur_offset = to_position + num_bytes;
}

void livemedia_out_packet_buffer__insert_word(
		livemedia_out_packet_buffer_t *out_packet_buffer,
		u_int32_t word, unsigned int to_position)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int32_t n_word;

	n_word = htonl(word);
	livemedia_out_packet_buffer__insert(out_packet_buffer, (unsigned char *)&n_word, 4,
			to_position);

}

void livemedia_out_packet_buffer__extract(
		livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char *to, unsigned int num_bytes, unsigned int from_position)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int real_from_position;

	real_from_position = out_packet_buffer->packet_start + from_position;
	if (real_from_position + num_bytes > out_packet_buffer->limit) {	/*sanity check*/
		if (real_from_position > out_packet_buffer->limit)
			return; /* we can't do this */
		num_bytes = out_packet_buffer->limit - real_from_position;
	}
	
	memmove(to, &out_packet_buffer->buf[real_from_position], num_bytes);

}

u_int32_t livemedia_out_packet_buffer__extract_word(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int from_position)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int32_t n_word;

	livemedia_out_packet_buffer__extract(out_packet_buffer, (unsigned char *)&n_word, 4,
			from_position);
	return ntohl(n_word);
}

void livemedia_out_packet_buffer__skip_bytes(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (num_bytes > livemedia_out_packet_buffer__total_bytes_available(out_packet_buffer))
		num_bytes = livemedia_out_packet_buffer__total_bytes_available(out_packet_buffer);
	livemedia_out_packet_buffer__increment(out_packet_buffer, num_bytes);
}

void livemedia_out_packet_buffer__set_overflow_data(
		livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int overflow_data_offset, unsigned int overflow_data_size, 
		struct timeval const *presentation_time, 
		unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	out_packet_buffer->overflow_data_offset = overflow_data_offset;
	out_packet_buffer->overflow_data_size = overflow_data_size;
	out_packet_buffer->overflow_presentation_time = *presentation_time;
	out_packet_buffer->overflow_duration_in_microseconds = duration_in_microseconds;
}

void livemedia_out_packet_buffer__use_overflow_data(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_out_packet_buffer__enqueue(out_packet_buffer, &out_packet_buffer->buf[
			out_packet_buffer->packet_start + out_packet_buffer->overflow_data_offset],
			out_packet_buffer->overflow_data_size);
	out_packet_buffer->cur_offset -= out_packet_buffer->overflow_data_size;
	/* undoes increment performed by "enqueue" */

	livemedia_out_packet_buffer__reset_overflow_data(out_packet_buffer);
}

void livemedia_out_packet_buffer__adjust_packet_start(
		livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	out_packet_buffer->packet_start += num_bytes;
	if (out_packet_buffer->overflow_data_offset >= num_bytes) {
		out_packet_buffer->overflow_data_offset -= num_bytes;
	} else {
		out_packet_buffer->overflow_data_offset = 0;
		out_packet_buffer->overflow_data_size = 0;	/* an error otherwise */
	}
}

void livemedia_out_packet_buffer__reset_packet_start(
		livemedia_out_packet_buffer_t *out_packet_buffer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (out_packet_buffer->overflow_data_size > 0) {
		out_packet_buffer->overflow_data_offset += out_packet_buffer->packet_start;
	}
	out_packet_buffer->packet_start = 0;
}
