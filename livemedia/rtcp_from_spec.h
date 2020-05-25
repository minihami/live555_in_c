/* RTCP code taken directly from the most recent RTP specification:
 *     draft-ietf-avt-rtp-new-11.txt
 * C header
 */

#ifndef __LIVEMEDIA_RTCP_FROM_SPEC_H__
#define __LIVEMEDIA_RTCP_FROM_SPEC_H__

#include <stdlib.h>

/* Definitions of _ANSI_ARGS and EXTERN that will work in either
   C or C++ code:
 */
#undef _ANSI_ARGS_
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _ANSI_ARGS_(x)	x
#else
#   define _ANSI_ARGS_(x)	()
#endif
#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/* The code from the spec assumes a type "event"; make this a void*: */
typedef void* event;

#define EVENT_UNKNOWN 0
#define EVENT_REPORT 1
#define EVENT_BYE 2

/* The code from the spec assumes a type "time_tp"; make this a double: */
typedef double time_tp;

/* The code from the spec assumes a type "packet"; make this a void*: */
typedef void* packet;

#define PACKET_UNKNOWN_TYPE 0
#define PACKET_RTP 1
#define PACKET_RTCP_REPORT 2
#define PACKET_BYE 3
#define PACKET_RTCP_APP 4

/* The code from the spec calls drand48(), but we have drand30() instead */
#define drand48 livemedia__drand30

/* The code calls "exit()", but we don't want to exit, so make it a noop: */
#define exit(n) do {} while (0)

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* EXPORTS: */

EXTERN void livemedia__on_expire _ANSI_ARGS_((event, int, int, double, int, double*, int*, time_tp, time_tp*, int*));

EXTERN void livemedia__on_receive _ANSI_ARGS_((packet, event, int*, int*, int*, double*, double*, double, double));

/* IMPORTS: */

EXTERN void livemedia__schedule _ANSI_ARGS_((double,event));
EXTERN void livemedia__reschedule _ANSI_ARGS_((double,event));
EXTERN void livemedia__send_rtcp_report _ANSI_ARGS_((event));
EXTERN void livemedia__send_bye_packet _ANSI_ARGS_((event));
EXTERN int livemedia__type_of_event _ANSI_ARGS_((event));
EXTERN int livemedia__send_packet_size _ANSI_ARGS_((event));
EXTERN int livemedia__packet_type _ANSI_ARGS_((packet));
EXTERN int livemedia__received_packet_size _ANSI_ARGS_((packet));
EXTERN int livemedia__new_member _ANSI_ARGS_((packet));
EXTERN int livemedia__new_sender _ANSI_ARGS_((packet));
EXTERN void livemedia__add_member _ANSI_ARGS_((packet));
EXTERN void livemedia__add_sender _ANSI_ARGS_((packet));
EXTERN void livemedia__remove_member _ANSI_ARGS_((packet));
EXTERN void livemedia__remove_sender _ANSI_ARGS_((packet));
EXTERN double livemedia__drand30 _ANSI_ARGS_((void));

#endif
