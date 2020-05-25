#ifndef __LIVEMEDIA_LOCALE_H__
#define __LIVEMEDIA_LOCALE_H__

/* If you're on a system that (for whatever reason) doesn't have either the "setlocale()" or the "newlocale()" function, then
 * add "-DLOCALE_NOT_USED" to your "config.*" file.

 * If you're on a system that (for whatever reason) has "setlocale()" but not "newlocale()", then
 * add "-DNEWLOCALE_NOT_USED" to your "config.*" file.
 * (Note that -DLOCALE_NOT_USED implies -DNEWLOCALE_NOT_USED; you do not need both.)
 * Also, for Windows systems, we define "NEWLOCALE_NOT_USED" by default, because at least some Windows systems
 * (or their development environments) don't have "newlocale()".  If, however, your Windows system *does* have "newlocale()",
 * then you can override this by defining "NEWLOCALE_USED" before #including this file.

 * Finally, some old development environments need a header file "xlocale.h" to use "newlocale()".
 * Should you need this header file, add "-DNEED_XLOCALE_H" to your "config.*" file. */

#ifdef NEWLOCALE_USED
#undef LOCALE_NOT_USED
#undef NEWLOCALE_NOT_USED
#else
#if defined(__WIN32__) || defined(_WIN32)
#define NEWLOCALE_NOT_USED 1
#endif
#endif

#ifndef LOCALE_NOT_USED
#include <locale.h>
#ifndef NEWLOCALE_NOT_USED
#ifdef NEED_XLOCALE_H
#include <xlocale.h>
#endif
#endif
#endif

#include <stdlib.h>
#include <stdio.h>

typedef enum _locale_category_e { all, numeric }
locale_category_e;	/* define and implement more categories later, as needed */

/*****************************************************************
 * Locale: (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_locale_t {

#ifndef LOCALE_NOT_USED
#ifndef NEWLOCALE_NOT_USED
	locale_t locale;
	locale_t prev_locale;
#else
	int category_num;
	char *prev_locale;
#endif
#endif

	void (*delete)(struct _livemedia_locale_t *locale);

} livemedia_locale_t;

/*****************************************************************
 * Locale: (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_locale_t *__livemedia_locale__alloc(void);
void __livemedia_locale__init(livemedia_locale_t *locale, 
		char *new_locale, locale_category_e category/*default: all */);
void __livemedia_locale__deinit(livemedia_locale_t *locale);
void __livemedia_locale__free(livemedia_locale_t *locale);
/*
 * new() and delete() functions
 */
livemedia_locale_t *livemedia__new__locale(
		char *new_locale, locale_category_e category/*default: all */);
void livemedia__delete__locale(livemedia_locale_t *locale);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_locale__delete__impl(livemedia_locale_t *locale);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
