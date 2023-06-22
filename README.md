# A "These Instructions Can Be Reordered" Hint for C

If you find yourself in a situation where you've turned
up your C compiler's optimizations, looked at the machine
code, and are now wishing you could just somehow tell it
that it's free to re-order or parallelize some operations,
I'd give this a quick try.

Otherwise, this is mostly a toy example of how we can
express our intent and expose optimization opportunities
in a way that a compiler might automatically understand
per the rules of the programming language it implements.


## How It Works

In C, certain things are "unsequenced". This means
they can happen in any order, or even *in parallel*
(even within a single thread, the compiler and CPU
are free to interleave the machine code of those
operations, or do them simultaneously - for
example with SIMD instructions).

For example, in the call `foo(a+*b, c+d[i])`, a C
compiler is allowed to generate code which does
`*b` in parallel with `d[i]`, and then does the
two adds in parallel. It's just a question of if
the CPU supports it, if the optimizer passes know
how to do it, and if it would actually be better
per our optimization criteria. This is possible
because the different arguments in a function call
argument list are unsequenced.

So we just need a function call which does nothing,
which can take any number of arguments of any type,
and has an intuitive and self-descriptive name.

So we define a variadic function like this:

```c
static void unordered(int unused, ...) {}
```

And now we can make a group of explicitly not-ordered
expressions by just putting each expression as a
separate argument in an `unordered(...)` call.

Since C requires variadic functions to take at least
one initial argument with a hard-coded type, that
first argument is unavoidable, but luckily, in all C
versions starting with C99, (and pre-C99 compilers
with the right extensions), we can also define a
variadic macro to fill in that leading `int` for us:

```c
#define unordered(...) unordered(0, __VA_ARGS__)
```

That way, unless our code must be compatible with C89,
we do not need to ensure that the first expression
in an `unordered` group evaluates to an `int`.


## Limitations

Sadly, since each expression in an `unordered` group
is evaluated as a function argument, function calls
with `void` return type cannot occur directly within
an `unordered` expression group.

Furthermore, even in unsequenced expressions,
function calls within those expressions are only
"indeterminately sequenced" as of C11: therefore,
a strictly conforming implementation can re-order
two function calls inside `unordered(...)`, but
not interleave or parallelize them.


## Example

```c
#include <unodered.h>
```

Now let's say we want to zero out some memory containing
a 256-bit secret cryptographic key before freeing it.
This is a notorious example of a problem created by
modern optimizations: the compiler detects that nothing
within the program will see the result of those writes,
and "optimizes" by just not doing those writes. In C,
the way to say "something outside the program might see
this memory" is to mark that memory as `volatile`. That
works to ensure the writes all happen, but the downside
is that the compiler must now preserve the order of the
writes as they occur in the source... *unless* we use a
construct like `unordered` to make all or some of the
writes unsequenced relative to each other:

```c
volatile unsigned char * ptr = secret_key_bytes;
unordered(
    ptr[0] = 0,
    ptr[1] = 0,
    ptr[2] = 0,
    /* ... */
    ptr[29] = 0,
    ptr[30] = 0,
    ptr[31] = 0  /* 256 bits == 32 bytes */
);
```

## C Name Collisions

Besides the public `unordered`, the `unordered.h` header
only defines identifiers that start with `UNORDERED_`.

`unordered.h` does not include any other headers.
