#include "class.h"

/*****************************************************************
 * Class:Superclass (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_class_t *__livemedia_class__alloc(void)
{
	livemedia_class_t *class;
	class = (livemedia_class_t *)calloc(1, sizeof(livemedia_class_t));
	if (class == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_class_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return class;
}
void __livemedia_class__init(livemedia_class_t *class)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	class->delete = livemedia_class__delete__impl;
}
void __livemedia_class__deinit(livemedia_class_t *class)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_class__free(livemedia_class_t *class)
{
	if (class) {
		free(class);
		class = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_class_t *livemedia__new__class(void)
{
	livemedia_class_t *class;

	class = __livemedia_class__alloc();
	if (class)
		__livemedia_class__init(class);

	return class;
}
void livemedia__delete__class(livemedia_class_t *class)
{
	if (class && class->delete)
		class->delete(class);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_class__delete__impl(livemedia_class_t *class)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (class) {
		__livemedia_class__deinit(class);
		__livemedia_class__free(class);
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
