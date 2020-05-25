#include "Locale.h"
#include "str_dup.h"

/*****************************************************************
 * Locale: (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_locale_t *__livemedia_locale__alloc(void)
{
	livemedia_locale_t *locale;
	locale = (livemedia_locale_t *)calloc(1, sizeof(livemedia_locale_t));
	if (locale == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_locale_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return locale;
}
void __livemedia_locale__init(livemedia_locale_t *locale, 
		char *new_locale, locale_category_e category/*default: all */)
{
#ifndef LOCALE_NOT_USED
#ifndef NEWLOCALE_NOT_USED
	int category_mask;
	/* constructor */
	switch (category) {
		case all:
			category_mask = LC_ALL_MASK;
			break;
		case numeric:
			category_mask = LC_NUMERIC_MASK;
			break;
	}
	locale->locale = newlocale(category_mask, new_locale, NULL);
	locale->prev_locale = uselocale(locale->locale);
#else
	switch (category) {
		case all:
			locale->category_num = LC_ALL;
			break;
		case numeric:
			locale->category_num = LC_NUMERIC;
			break;
	}
	locale->prev_locale = livemedia__str_dup(setlocale(locale->category_num, NULL));
	setlocale(locale->category_num, new_locale);
#endif
#endif
	/* virtual functions configuration */ 
	locale->delete = livemedia_locale__delete__impl;
}
void __livemedia_locale__deinit(livemedia_locale_t *locale)
{
	/* destructor */
#ifndef LOCALE_NOT_USED
#ifndef NEWLOCALE_NOT_USED
	if (locale->locale != (locale_t)0) {
		uselocale(locale->prev_locale);
		freelocale(locale->locale);
	}
#else
	if (locale->prev_locale != NULL) {
		setlocale(locale->category_num, locale->prev_locale);
		if (locale->prev_locale) {
			free(locale->prev_locale);
			prev_locale = NULL;
		}
	}
#endif
#endif

}
void __livemedia_locale__free(livemedia_locale_t *locale)
{
	if (locale) {
		free(locale);
		locale = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_locale_t *livemedia__new__locale(
		char *new_locale, locale_category_e category/*default: all */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_locale_t *locale;

	locale = __livemedia_locale__alloc();
	if (locale)
		__livemedia_locale__init(locale, new_locale, category);

	return locale;
}
void livemedia__delete__locale(livemedia_locale_t *locale)
{
	if (locale && locale->delete)
		locale->delete(locale);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_locale__delete__impl(livemedia_locale_t *locale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (locale) {
		__livemedia_locale__deinit(locale);
		__livemedia_locale__free(locale);
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
