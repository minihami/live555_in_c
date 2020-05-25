#include "rtp_source.h"
#include "groupsock_helper.h"

/*****************************************************************
 * RTPSource:FramedSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_source_t *__livemedia_rtp_source__alloc(void)
{
	livemedia_rtp_source_t *rtp_source;
	rtp_source = (livemedia_rtp_source_t *)calloc(1, sizeof(livemedia_rtp_source_t));
	if (rtp_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_source;
}

void __livemedia_rtp_source__init(livemedia_rtp_source_t *rtp_source,
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int32_t rtp_timestamp_frequency)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_source->delete = livemedia_rtp_source__delete__impl;
}

void __livemedia_rtp_source__deinit(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}

void __livemedia_rtp_source__free(livemedia_rtp_source_t *rtp_source)
{
	if (rtp_source) {
		free(rtp_source);
		rtp_source = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_rtp_source_t *livemedia__new__rtp_source(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int32_t rtp_timestamp_frequency)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_source_t *rtp_source;

	rtp_source = __livemedia_rtp_source__alloc();
	if (rtp_source)
		__livemedia_rtp_source__init(rtp_source, rtp_gs, rtp_payload_format,
				rtp_timestamp_frequency);

	return rtp_source;
}

/*
 * Functions for virtual functions
 */
bool livemedia_rtp_source__has_been_synchronized_using_rtcp(struct _livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__set_packet_reordering_threshold_time(
		struct _livemedia_rtp_source_t *rtp_source, unsigned int u_seconds)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Override functions
 */
bool livemedia_rtp_source__has_been_synchronized_using_rtcp__impl(
		livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_rtp_source__is_rtp_source__impl(livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__get_attributes__impl(livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__delete__impl(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child */
	if (rtp_source) {
		__livemedia_rtp_source__deinit(rtp_source);
		__livemedia_rtp_source__free(rtp_source);
	}
}

/*
 * Functions in header file
 */
bool livemedia_rtp_source__cur_packet_maker_bit(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned char livemedia_rtp_source__rtp_payload_format(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_groupsock_t *livemedia_rtp_source__rtp_gs(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_groupsock__set_crypto(livemedia_rtp_source_t *rtp_source,
		livemedia_srtp_cryptographic_context_t *crypto)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int32_t livemedia_rtp_source__ssrc(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__register_for_multiplexed_rtcp_packets(
		livemedia_rtp_source_t *rtp_source, struct _livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__deregister_for_multiplexed_rtcp_packets(
		livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_source__timestamp_frequency(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

struct _livemedia_rtp_reception_stats_db_t *livemedia_rtp_source__reception_stats_db(
		livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int32_t livemedia_rtp_source__last_received_ssrc(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool *livemedia_rtp_source__enable_rtcp_reports(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__set_stream_socket(livemedia_rtp_source_t *rtp_source,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_source__set_auxilliary_read_handler(livemedia_rtp_source_t *rtp_source,
		livemedia__aux_handler_func *handler_func, void *handler_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int16_t livemedia_rtp_source_cur_packet_rtp_seq_num(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int16_t livemedia_rtp_source_cur_packet_rtp_timestamp(livemedia_rtp_source_t *rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Normal functions
 */
bool livemedia_rtp_source__lookup_by_name__static(char const *source_name,
		livemedia_rtp_source_t **result_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * RTPReceptionStatsDB (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_db_t *__livemedia_rtp_reception_stats_db__alloc(void)
{
	livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db;
	rtp_reception_stats_db = (livemedia_rtp_reception_stats_db_t *)calloc(1, sizeof(livemedia_rtp_reception_stats_db_t));
	if (rtp_reception_stats_db == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_reception_stats_db_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_reception_stats_db;
}
void __livemedia_rtp_reception_stats_db__init(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_reception_stats_db->delete = livemedia_rtp_reception_stats_db__delete__impl;
}
void __livemedia_rtp_reception_stats_db__deinit(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_rtp_reception_stats_db__free(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
	if (rtp_reception_stats_db) {
		free(rtp_reception_stats_db);
		rtp_reception_stats_db = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_db_t *livemedia__new__rtp_reception_stats_db(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db;

	rtp_reception_stats_db = __livemedia_rtp_reception_stats_db__alloc();
	if (rtp_reception_stats_db)
		__livemedia_rtp_reception_stats_db__init(rtp_reception_stats_db);

	return rtp_reception_stats_db;
}
void livemedia__delete__rtp_reception_stats_db(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
	if (rtp_reception_stats_db && rtp_reception_stats_db->delete)
		rtp_reception_stats_db->delete(rtp_reception_stats_db);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats_db__delete__impl(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_reception_stats_db) {
		__livemedia_rtp_reception_stats_db__deinit(rtp_reception_stats_db);
		__livemedia_rtp_reception_stats_db__free(rtp_reception_stats_db);
	}
}

/*
 * Functions in header file
 */
unsigned int livemedia_rtp_reception_stats_db__tot_num_packets_received(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats_db__num_active_sources_since_last_reset(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Normal functions
 */
void livemedia_rtp_reception_stats_db__reset(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats_db__note_incoming_packet(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc, u_int16_t seq_num,
		u_int32_t rtp_timestamp,
		unsigned int timestamp_frequency,
		bool use_for_jitter_calculation,
		struct timeval *result_presentation_time,
		bool *result_has_been_synced_using_rtcp,
		unsigned packet_size /* payload only */)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats_db__note_incoming_sr(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ntp_timestamp_msw, u_int32_t ntp_timestamp_lsw,
		u_int32_t rtp_timestamp)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats_db__remove_record(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

struct _livemedia_rtp_reception_stats_t *livemedia_rtp_reception_stats_db__lookup(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats_db__add(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc, struct _livemedia_rtp_reception_stats_t *stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * RTPReceptionStatsDB::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_db_iterator_t *__livemedia_rtp_reception_stats_db_iterator__alloc(void)
{
	livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator;
	rtp_reception_stats_db_iterator = (livemedia_rtp_reception_stats_db_iterator_t *)calloc(1, sizeof(livemedia_rtp_reception_stats_db_iterator_t));
	if (rtp_reception_stats_db_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_reception_stats_db_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_reception_stats_db_iterator;
}
void __livemedia_rtp_reception_stats_db_iterator__init(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator,
		livemedia_rtp_reception_stats_db_t *reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_reception_stats_db_iterator->delete = livemedia_rtp_reception_stats_db_iterator__delete__impl;
}
void __livemedia_rtp_reception_stats_db_iterator__deinit(livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_rtp_reception_stats_db_iterator__free(livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator)
{
	if (rtp_reception_stats_db_iterator) {
		free(rtp_reception_stats_db_iterator);
		rtp_reception_stats_db_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_db_iterator_t *livemedia__new__rtp_reception_stats_db_iterator(
		livemedia_rtp_reception_stats_db_t *reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator;

	rtp_reception_stats_db_iterator = __livemedia_rtp_reception_stats_db_iterator__alloc();
	if (rtp_reception_stats_db_iterator)
		__livemedia_rtp_reception_stats_db_iterator__init(rtp_reception_stats_db_iterator,
				reception_stats_db);

	return rtp_reception_stats_db_iterator;
}
void livemedia__delete__rtp_reception_stats_db_iterator(livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator)
{
	if (rtp_reception_stats_db_iterator && rtp_reception_stats_db_iterator->delete)
		rtp_reception_stats_db_iterator->delete(rtp_reception_stats_db_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats_db_iterator__delete__impl(livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_reception_stats_db_iterator) {
		__livemedia_rtp_reception_stats_db_iterator__deinit(rtp_reception_stats_db_iterator);
		__livemedia_rtp_reception_stats_db_iterator__free(rtp_reception_stats_db_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
struct _livemedia_rtp_reception_stats_t *livemedia_rtp_reception_stats_db_iterator__next(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator,
		bool include_inactive_sources /*default: false */)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * RTPReceptionStats (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_t *__livemedia_rtp_reception_stats__alloc(void)
{
	livemedia_rtp_reception_stats_t *rtp_reception_stats;
	rtp_reception_stats = (livemedia_rtp_reception_stats_t *)calloc(1, sizeof(livemedia_rtp_reception_stats_t));
	if (rtp_reception_stats == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_reception_stats_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_reception_stats;
}

void __livemedia_rtp_reception_stats__init(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_reception_stats->delete = livemedia_rtp_reception_stats__delete__impl;
}

void __livemedia_rtp_reception_stats__init_by_initial_seq_num(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ssrc, u_int16_t initial_seq_num)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_reception_stats->delete = livemedia_rtp_reception_stats__delete__impl;
}

void __livemedia_rtp_reception_stats__deinit(livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}

void __livemedia_rtp_reception_stats__free(livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
	if (rtp_reception_stats) {
		free(rtp_reception_stats);
		rtp_reception_stats = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_t *livemedia__new__rtp_reception_stats(u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_reception_stats_t *rtp_reception_stats;

	rtp_reception_stats = __livemedia_rtp_reception_stats__alloc();
	if (rtp_reception_stats)
		__livemedia_rtp_reception_stats__init(rtp_reception_stats, ssrc);

	return rtp_reception_stats;
}
livemedia_rtp_reception_stats_t *livemedia__new__rtp_reception_stats_by_initial_seq_num(
		u_int32_t ssrc, u_int16_t initial_seq_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_reception_stats_t *rtp_reception_stats;

	rtp_reception_stats = __livemedia_rtp_reception_stats__alloc();
	if (rtp_reception_stats)
		__livemedia_rtp_reception_stats__init_by_initial_seq_num(rtp_reception_stats,
				ssrc, initial_seq_num);

	return rtp_reception_stats;
}
void livemedia__delete__rtp_reception_stats(livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
	if (rtp_reception_stats && rtp_reception_stats->delete)
		rtp_reception_stats->delete(rtp_reception_stats);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats__delete__impl(livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_reception_stats) {
		__livemedia_rtp_reception_stats__deinit(rtp_reception_stats);
		__livemedia_rtp_reception_stats__free(rtp_reception_stats);
	}
}

/*
 * Functions in header file
 */
u_int32_t livemedia_rtp_reception_stats__ssrc(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__num_packets_received_since_last_reset(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__tot_num_packets_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__tot_num_packets_expected(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__base_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__last_reset_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__highest_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__last_received_sr_ntp_msw(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__last_received_sr_ntp_lsw(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

struct timeval const *livemedia_rtp_reception_stats__last_received_sr_time(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__min_inter_packet_gap_us(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__max_inter_packet_gap_us(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Normal functions
 */
double livemedia_rtp_reception_stats__tot_num_bytes_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_rtp_reception_stats__jitter(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats__note_incoming_packet(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int16_t seq_num, u_int32_t rtp_timestamp,
		unsigned int timestamp_frequency,
		bool use_for_jitter_calculation,
		struct timeval *result_presentaion_time,
		bool *result_has_been_synced_using_rtcp,
		unsigned int packet_size /* payload only */)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats__note_incoming_sr(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ntp_timestamp_msw, u_int32_t ntp_timestamp_lsw,
		u_int32_t rtp_timestamp)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats__init(
		livemedia_rtp_reception_stats_t *rtp_reception_stats, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats__init_seq_num(
		livemedia_rtp_reception_stats_t *rtp_reception_stats, u_int16_t initial_seq_num)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_reception_stats__reset(
		livemedia_rtp_reception_stats_t *rtp_reception_stats)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Static functions implementation for ancestor
 */
