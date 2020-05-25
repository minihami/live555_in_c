#include "file_server_media_subsession.h"

/*****************************************************************
 * FileServerMediaSubsession:OnDemandServerMediaSubsession (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_file_server_media_subsession_t *__livemedia_file_server_media_subsession__alloc(void)
{
	livemedia_file_server_media_subsession_t *file_server_media_subsession;
	file_server_media_subsession = (livemedia_file_server_media_subsession_t *)calloc(1, sizeof(livemedia_file_server_media_subsession_t));
	if (file_server_media_subsession == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_file_server_media_subsession_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return file_server_media_subsession;
}
void __livemedia_file_server_media_subsession__init(
		livemedia_file_server_media_subsession_t *file_server_media_subsession,
		char const *file_name, bool reuse_first_source)
{
	__livemedia_on_demand_server_media_subsession__init(
			&file_server_media_subsession->on_demand_server_media_subsession, reuse_first_source,
			6970, false);

	file_server_media_subsession->file_size = 0;
	file_server_media_subsession->file_name = livemedia__str_dup(file_name);
	
	/* virtual functions configuration */ 

	((livemedia_medium_t *)file_server_media_subsession)->delete =
		livemedia_file_server_media_subsession__delete__impl;
}
void __livemedia_file_server_media_subsession__deinit(livemedia_file_server_media_subsession_t *file_server_media_subsession)
{
	/* parent deinit */
	__livemedia_on_demand_server_media_subsession__deinit(
			&file_server_media_subsession->on_demand_server_media_subsession);
			
}
void __livemedia_file_server_media_subsession__free(livemedia_file_server_media_subsession_t *file_server_media_subsession)
{
	if (file_server_media_subsession) {
		free(file_server_media_subsession);
		file_server_media_subsession = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_file_server_media_subsession_t *livemedia__new__file_server_media_subsession(
		char const *file_name, bool reuse_first_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_file_server_media_subsession_t *file_server_media_subsession;

	file_server_media_subsession = __livemedia_file_server_media_subsession__alloc();
	if (file_server_media_subsession)
		__livemedia_file_server_media_subsession__init(file_server_media_subsession,
				file_name, reuse_first_source);

	return file_server_media_subsession;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_file_server_media_subsession__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_file_server_media_subsession_t *file_server_media_subsession;
	file_server_media_subsession = (livemedia_file_server_media_subsession_t *)medium;
	/* if child, get parent as parameter and cast to child */
	if (file_server_media_subsession) {
		__livemedia_file_server_media_subsession__deinit(file_server_media_subsession);
		__livemedia_file_server_media_subsession__free(file_server_media_subsession);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */
