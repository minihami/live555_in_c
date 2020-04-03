#ifndef __LIVEMEDIA_RTCP_H__
#define __LIVEMEDIA_RTCP_H__

#include "rtp_sink.h"
#if 0
TODO: 
#include "rtp_source.h"
#include "srtp_cryptographic_context.h"
#endif

/*****************************************************************
 * SDEItem (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_sdes_item_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

} livemedia_sdes_item_t;

/*****************************************************************
 * RTCPInstance:Medium (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtcp_instance_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

} livemedia_rtcp_instance_t;

/*****************************************************************
 * SDEItem (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_sdes_item_t *__livemedia_sdes_item__alloc(void);
void __livemedia_sdes_item__init(livemedia_sdes_item_t *sdes_item);
void __livemedia_sdes_item__deinit(livemedia_sdes_item_t *sdes_item);
void __livemedia_sdes_item__free(livemedia_sdes_item_t *sdes_item);
/*
 * new() and delete() functions
 */
livemedia_sdes_item_t *livemedia__new__sdes_item(void);
void livemedia__delete__sdes_item(livemedia_sdes_item_t *sdes_item);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_sdes_item__delete__impl(livemedia_sdes_item_t *sdes_item);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * RTCPInstance:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtcp_instance_t *__livemedia_rtcp_instance__alloc(void);
void __livemedia_rtcp_instance__init(livemedia_rtcp_instance_t *rtcp_instance);
void __livemedia_rtcp_instance__deinit(livemedia_rtcp_instance_t *rtcp_instance);
void __livemedia_rtcp_instance__free(livemedia_rtcp_instance_t *rtcp_instance);
/*
 * new() and delete() functions
 */
livemedia_rtcp_instance_t *livemedia__new__rtcp_instance(void);
void livemedia__delete__rtcp_instance(livemedia_rtcp_instance_t *rtcp_instance);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtcp_instance__delete__impl(livemedia_rtcp_instance_t *rtcp_instance);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
