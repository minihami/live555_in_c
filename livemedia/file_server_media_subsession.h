#ifndef __LIVEMEDIA_FILE_SERVER_MEDIA_SUBSESSION_H__
#define __LIVEMEDIA_FILE_SERVER_MEDIA_SUBSESSION_H__

#include "server_media_session.h"
#include "on_demand_server_media_subsession.h"
/*****************************************************************
 * FileServerMediaSubsession:OnDemandServerMediaSubsession (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_file_server_media_subsession_t {
	livemedia_on_demand_server_media_subsession_t on_demand_server_media_subsession;

	char const *file_name;
	u_int64_t file_size; /* if known*/

} livemedia_file_server_media_subsession_t;

/*****************************************************************
 * FileServerMediaSubsession:OnDemandServerMediaSubsession (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_file_server_media_subsession_t *__livemedia_file_server_media_subsession__alloc(
		void);

void __livemedia_file_server_media_subsession__init(
		livemedia_file_server_media_subsession_t *file_server_media_subsession,
		char const *file_name, bool reuse_first_source);
void __livemedia_file_server_media_subsession__deinit(
		livemedia_file_server_media_subsession_t *file_server_media_subsession);
void __livemedia_file_server_media_subsession__free(
		livemedia_file_server_media_subsession_t *file_server_media_subsession);
/*
 * new() and delete() functions
 */
livemedia_file_server_media_subsession_t *livemedia__new__file_server_media_subsession(
		char const *file_name, bool reuse_first_source);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_file_server_media_subsession__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
