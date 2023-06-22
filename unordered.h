/* SPDX-License-Identifier: 0BSD */
/* Copyright 2022 Alexander Kozhevnikov <mentalisttraceur@gmail.com> */

#ifndef UNORDERED_H
#define UNORDERED_H

/* The order of evaluation for function call arguments is
   unspecified, so we can free an optimizing compiler to
   re-order expressions by writing them in the argument
   list of a single function call.

   A variadic function can take any number of arguments
   of any type, does not have to do anything, and works
   all the way back to C89 - perfect for this trick. */
static void unordered(int unused, ...)
{
    /* C requires at least one non-variadic parameter with
       a fixed type. This is an attempt to suppress unused
       parameter warnings in *all* compilers, linters, and
       so on (if you know of a better way, please send it
       to https://github.com/mentalisttraceur/c-unused): */
    #ifdef __KEIL__
    (void)(unused=unused);
    #else
    (void)unused;
    #endif
}

/* If variadic macros are available, don't force developers to deal with
   the first argument needing to be an `int` - insert one automatically: */
#if __STDC_VERSION__ >= 199901L || _MSC_FULL_VER >= 140050727L
    #define unordered(...) unordered(0, __VA_ARGS__)
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wvariadic-macros"
    #if __GNUC__ > 2
        #define unordered(arguments...) unordered(0, arguments)
    #elif __GNUC__ == 2 && __GNUC_MINOR__ > 95
        #define unordered(arguments...) unordered(0, arguments)
    #elif __GNUC__ == 2 && __GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ >= 3
        #define unordered(arguments...) unordered(0, arguments)
    #endif
    #pragma GCC diagnostic pop
#endif

#endif /* UNORDERED_H */
