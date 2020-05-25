#!/usr/bin/bash

MACRO_NAME=LOCALE
CLASS_NAME=Locale
SUPER_CLASS_NAME=
class=locale

sed "s/CLASS/$MACRO_NAME/g" format.h > $class.h
sed -i "s/Superclass/$SUPER_CLASS_NAME/g" $class.h 
sed -i "s/Class/$CLASS_NAME/g" $class.h 
sed -i "s/class/$class/g" $class.h 


sed "s/Superclass/$SUPER_CLASS_NAME/g" format.c > $class.c
sed -i "s/Class/$CLASS_NAME/g" $class.c 
sed -i "s/class/$class/g" $class.c  
