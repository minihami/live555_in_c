#ifndef __LIVEMEDIA_FRAMED_FILE_SOURCE_H__
#define __LIVEMEDIA_FRAMED_FILE_SOURCE_H__

#include "framed_source.h"
/*****************************************************************
 * FramedFileSource:FramedSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_framed_file_source_t {
	livemedia_framed_source_t framed_source;

	/* member variables */
	uv_fs_t *fid;

} livemedia_framed_file_source_t;

/*****************************************************************
 * FramedFileSource:FramedSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_file_source_t *__livemedia_framed_file_source__alloc(void);

void __livemedia_framed_file_source__init(
		livemedia_framed_file_source_t *framed_file_source, uv_fs_t *fid);

void __livemedia_framed_file_source__deinit(
		livemedia_framed_file_source_t *framed_file_source);

void __livemedia_framed_file_source__free(
		livemedia_framed_file_source_t *framed_file_source);
/*
 * new() and delete() functions
 */
livemedia_framed_file_source_t *livemedia__new__framed_file_source(uv_fs_t *fid);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_framed_file_source__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
