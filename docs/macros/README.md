# Macros
CClogger provides a number of convenience macros. To use these macros you need to use
```c
#include <cclog_macros.h>
```
The macros are defined in different header file to not clutter the main API and to be optitional for usage.

## Usage
There are a lot of macros defined and the best way to learn everything about them is to look into the [header file](https://github.com/CrusaderSVK287/CCLog/blob/main/src/cclog_macros.h) itself since every macro has a comment above it saying shortly what it does.\
However, a short introduction is also found in this documentation. We have 3 kinds of macros:
* on condition jump
* on condition make a log and then jump
* on condition just make a log 

Here, jumping is meant using `goto`. Most people don't like to use gotos, hence why the 3rd kind of macro was added. Lets look at some details.

## Common traits
If we take a look on one of each "group" of macros we can see some similiarities
```c
/* Fires if val is 0 */
#define if_success(val, label) {if (val == 0) {goto label;}}

/* Fires if val is greater than zero */
#define if_failed_logp(val, label, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

/* Fires if val is null, doesnt jump to label  */
#define if_null_log_ng(val, log_level, priv, msg, ...) {\
    if (val == (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
```
First, we can see that the naming scheme of the macros looks like this: `if_(success|failed)[_log][_(p|n|pe|ne)][_ng]`. This is the basic naming scheme. The first part, `if_(success|failed)`
if_success or if_failed doesnt have any effect on functionality, it's just to enable denotig that the jump is supposed to be performed if the operation was successfull. \
Note: The only difference between success/failed is that basic(that is without suffix) if_success makes a val == 0 comparison and if_failed make a val != 0 comparison.\
The second part `[_log]`, just indicates whether the macro produces a log or not.\
The third part `[_(p|n|pe|ne)][_ng]` are the suffixes. These are explained in the next chapter

## Suffixes
You may have noticed that there are a lot of macros that almost the same but have some suffix to them. These suffixes indicates how the macro compares value.
For all the macro goups, these suffixes are used.
* p - comparison is (val > 0)
* n - comparison is (val < 0)
* pe - comparison is (val >= 0)
* ne - comparison is (val <= 0)

The third macro group (on condition just make a log) has its own suffix since it does exactly the same thing as the second group (on condition make a log and jump) but without jumping to any label. All macros in this group also have a suffix `ng` (abreviation of "no goto").

### Suffix Examples

`if_failedn` : suffix is n, meaning will jump if val is **n**egative\
`if_success_log_p` : suffix is p, meaning will jump if val is **p**ositive\
`if_failed_log_pe_ng` : suffix is p, so it will fire if value is more or equal to zero, **but** it also has a `ng` suffix meaning it won't jump to label
