#include "framed_file_source.h"

/*****************************************************************
 * FramedFileSource:FramedSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_file_source_t *__livemedia_framed_file_source__alloc(void)
{
	livemedia_framed_file_source_t *framed_file_source;
	framed_file_source = (livemedia_framed_file_source_t *)calloc(1, sizeof(livemedia_framed_file_source_t));
	if (framed_file_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_framed_file_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return framed_file_source;
}
void __livemedia_framed_file_source__init(
		livemedia_framed_file_source_t *framed_file_source, uv_fs_t *fid)
{
	__livemedia_framed_source__init(&framed_file_source->framed_source);

	framed_file_source->fid = fid;
	
	((livemedia_medium_t *)framed_file_source)->delete = 
		livemedia_framed_file_source__delete__impl;
}
void __livemedia_framed_file_source__deinit(livemedia_framed_file_source_t *framed_file_source)
{
	if (framed_file_source->fid) {
		free(framed_file_source->fid);
		framed_file_source->fid = NULL;
	}

	__livemedia_framed_source__deinit(&framed_file_source->framed_source);
}
void __livemedia_framed_file_source__free(livemedia_framed_file_source_t *framed_file_source)
{
	if (framed_file_source) {
		free(framed_file_source);
		framed_file_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_framed_file_source_t *livemedia__new__framed_file_source(uv_fs_t *fid)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_file_source_t *framed_file_source;

	framed_file_source = __livemedia_framed_file_source__alloc();
	if (framed_file_source)
		__livemedia_framed_file_source__init(framed_file_source, fid);

	return framed_file_source;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_framed_file_source__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_file_source_t *framed_file_source;
	framed_file_source = (livemedia_framed_file_source_t *)medium;

	if (framed_file_source) {
		__livemedia_framed_file_source__deinit(framed_file_source);
		__livemedia_framed_file_source__free(framed_file_source);
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
