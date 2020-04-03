
#ifndef __LIVEMEDIA_CLASS_H__
#define __LIVEMEDIA_CLASS_H__
/*****************************************************************
 * Class:Superclass (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_class_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

} livemedia_class_t;

/*****************************************************************
 * Class:Superclass (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_class_t *__livemedia_class__alloc(void);
void __livemedia_class__init(livemedia_class_t *class);
void __livemedia_class__deinit(livemedia_class_t *class);
void __livemedia_class__free(livemedia_class_t *class);
/*
 * new() and delete() functions
 */
livemedia_class_t *livemedia__new__class(void);
void livemedia__delete__class(livemedia_class_t *class);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_class__delete__impl(livemedia_class_t *class);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
