                ----------------------------------
                     PATTERN TEMPLATE LIBRARY
                ----------------------------------
                         directory lib

This is the key directory which contains all the essential files
of the library. If you don't use the Template Manager, this is all you
need to work with the library.

Each file *.h contains templates for one pattern. Any of these files can be
used either as a pure template or, when #define TEMPLATE_MANAGER is used,
in conjunction with the Template Manager.

Each file contains both the interface (equivalent of *.h) and the
implementation (equivalent of *.cpp) as usual for templates.

File mgr.h is automatically used by all templates. It controls
error handling and the use of the Template Manager.

-----------------------------------------------------------------------------
Currently, this directory includes the following patterns:

(A) Basic data structures useful for constructing patterns:

    collecti.h = list-based collection, parent object (class P) pointing
                 to a circular list of child objects (class C).
    aggregat.h = list based aggregate, just like the collection, but
                 each child contains a pointer to the parent.
    array.h    = dynamically growing array of objects.
    ptrarray.h = dynamically growing array of pointers.
    arraycon.h = dynamic array as a container, unattached

(B) True patterns:

    composit.h = pattern composite.
    flyweigh.h = pattern flyweight.
    fsm.h      = finite state machine

(C) Auxilliary files:

    mgr.h      = required for the operation of the Template Manager.
-----------------------------------------------------------------------------
The purpose of this libray is not only to supply some ready-to-use
patterns, but also to provide an environment in which you can easily
add new patterns to the library. For more details, see the Users Guide
in directory DOC.
