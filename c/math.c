/*
# math.h

    Mathematical functions.

    C99 made many improvements to it. It seems that the C community is trying to replace FORTRAN by C
    for numerical computations, which would be a blessing as it would mean that the system programming
    croud (C) would be closer to the numerical programming one (FORTRAN).

# Redundant mathematical functions

    Many functions are redundant, but are furnished because of possible speedups and better precision.

    For exapmle, `sqrt` and `pow` are redundant since in theoryin theory `sqrt(x) == pow(x,0.5)`.

    However, many hardware platforms such as x86 implement a `sqrt` as a single instruction,
    if you use `sqrt` it will be simpler for the compiler to use the x86 sqrt instruction
    if it is available.

    Using the hardware instruction may be faster.

    It will also possibly be more precise since it is likelly that sqrt
    would need several floating point operations to implement, each one meaning a loss of precision,
    while the hardware can do them a single operation.

    I guess however that good compilers will optimize `pow(x, 0.5)` to `sqrt(x)`.

    Anyways, it is better to play on the safe side, and always use the most specific operation possible.
*/

#include "common.h"

const double ERR = 10e-6;

int main() {
    /*
    # Math errors

    # Errors

        The following errors exist for the standard library functions.

        # Domain error

            Value outside of function domain. E.g.: `sqrt(-1.0)`.

            TODO undefined or implementation defined?

            Detection: if `math_errhandling & MATH_ERRNO != 0`, `errno = ERANGE`
            and a "divide-by-zero" floating point exception is raised.

        # Pole error

            Function has a pole at a point. Ex: `log(0.0)`, `tgamma(-1.0)`.

            Return value: HUGE_VAL.

            Detection if `math_errhandling & MATH_ERRNO != 0`, `errno = ERANGE`
            and a "divide-by-zero" floating point exception is raised.

        # Range errors

            Occur if the result is too large or too small to fint into the return type.

            There are two types of range errors overflow and underflow.

            In both cases, if `math_errhandling & MATH_ERRNO != 0`,
            `errno = ERANGE` and a "divide-by-zero" floating point exception is raised.

            # Overflow

                For exapmle, around poles, functions can have arbitrarily large values,
                so it is possible that for a given input close enough to the pole that the output is too large to reprent.

                Return value: HUGE_VAL, HUGE_VALF, or HUGE_VALL, acording to function's return type.

            # Underflow

                The output is too small to represent

                Return value: an implementation-defined value whose magnitude is no greater than the smallest
                normalized positive number in the specified type;
    */
    printf("math_errhandling & MATH_ERRNO = %d\n", math_errhandling & MATH_ERRNO);

    /*
    # abs

    # fabs
    */
    {
        /* Absolute values, integer version: */
        assert(abs(-1.1) == 1);

        /* Absolute values, float version: */
        assert(fabsl(-1.1) == 1.1);
    }

#if __STDC_VERSION__ >= 199901L
    /*
    # fminl

    # fmaxl

        Max and min for floats.
    */
    {
        assert(fminl(0.1, 0.2) == 0.1);
        assert(fmaxl(0.1, 0.2) == 0.2);
    }
#endif

    /*
    # Rounding
    */
    {
        /*
        # floor

        # ceil
        */
        {
            assert(fabs(floor(0.5) - 0.0) < ERR);
            assert(fabs(ceil(0.5)  - 1.0) < ERR);
        }

        /*
        # trunc

            Never raises any errors because the new result always fits in the data type (magnitide decresases).
        */
        {
            assert(fabs(trunc(1.5)  -  1.0) < ERR);
            assert(fabs(trunc(-1.5) - -1.0) < ERR);
        }

        /*
        # round

            Away from 0 on mid case.
        */
        {
            assert(fabs(round( 1.25) -  1.0) < ERR);
            assert(fabs(round( 1.5 ) -  2.0) < ERR);
            assert(fabs(round( 1.75) -  2.0) < ERR);
            assert(fabs(round(-1.5 ) - -2.0) < ERR);
        }

        /*
        # modf

            Decompose into fraction and integer parts.
        */
        {
            double d;
            assert(fabs(modf(3.25, &d) - 0.25) < ERR);
            assert(fabs(d              - 3.00) < ERR);
        }
    }

    /*
    # fma

        Fused multiple add or floating point multiply and add.

        Does addition and multiplication on one operation,
        with a single rounding, reduncing rounding errors.

        Has hardware implementations on certain platforms.
    */
    {
        assert(fabs(fma(2.0, 3.0, 4.0) - (2.0 * 3.0 + 4.0)) < ERR);
    }

    /* # Exponential functions */
    {
        /* # exp */
        {
            assert(fabs(exp(1.0) - 2.71) < 0.01);
        }

        /*
        # ln

            See log.

        # log

            Calculates the ln.
        */
        {
            assert(fabs(log(exp(1.0)) - 1.0) < ERR);
            assert(fabs(log2(8.0)     - 3.0) < ERR);
            assert(fabs(log10(100.0)  - 2.0) < ERR);
        }

        /*
        # sqrt

            Range is positive or zero. Negatives are a range error.

            To get complex on negative values, use `csqrt`.
        */
        {
            assert(fabs(sqrt(4.0) - 2.0) < ERR);

            /* GCC 4.7 -O3 is smart enough to see that this is bad: */
            {
                float f = -4.0;
                /*printf("sqrt(-4.0) = %f\n", sqrt(f));*/
            }

            {
                float f;
                volatile float g;

                f = -4.0;
                errno = 0;
                g = sqrt(f);
                if (math_errhandling & MATH_ERRNO)
                    assert(errno == EDOM);
                printf("sqrt(-4.0) = %f\n", f);
            }
        }

#if __STDC_VERSION__ >= 199901L

        /*
        # hypot

            Hypotenuse: sqrt(x^2 + y^2)
        */
        {
            assert(fabs(hypot(3.0, 4.0) - 5.0) < ERR);
        }
#endif

#if __STDC_VERSION__ >= 199901L
        /*
        # cbrt

            CuBe RooT
        */
        {
            assert(fabs(cbrt(8.0 ) -  2.0) < ERR);
            assert(fabs(cbrt(-8.0) - -2.0) < ERR);
        }
#endif

        /* # pow */
        {
            assert(fabs(pow(2.0, 3.0) - 8.0 ) < ERR);
        }
    }

    /* # trig */
    {
        float f = sin(0.2);
        assert(fabs(sin(0.0) - 0.0) < ERR);
        assert(fabs(cos(0.0) - 1.0) < ERR);

        /*
        # PI

            There is no predefined macro for PI. TODO0 why not? so convenient...

            This is a standard way to get PI.

            The only problem is a possible slight calculation overhead.
            But don't worry much about it. For example in gcc 4.7, even with `gcc -O0` trigonometric functions
            are calculated at compile time and stored in the program text.
        */
        {
            assert(fabs(acos(-1.0) - 3.14)    < 0.01);
        }
    }

    /* # erf: TODO0 understand */

    /*
    # factorial

        There seems to be no integer factorial function,
        but `gamma(n+1)` coincides with the factorials of `n` on the positive integers,
        and may be faster to compute via analytic approximations that can be done to gamma
        and/or via a hardware implementation, so just use gamma.

    # gamma

        Wiki link: <http://en.wikipedia.org/wiki/Gamma_function>

        Extension of the factorials to the real numbers because:

        - on the positive integergs:

            gamma(n+1) == n!

        - on the positive reals:

            gamma(x+1) == x * gamma(x)

        Has a holomorphic continuation to all imaginary plane, with poles on 0 and negative integers.

        Implemented in C as `tgamma`.

    # tgamma

        True Gamma function. TODO0 Why True?

        Computes the gamma function.

        ANSI C says that it gives either domain or pole error on the negative integers.

    # lgamma

        lgamma = ln tgamma
    */
    {
        assert(fabs(tgamma(5.0) - 4*3*2) < ERR);
        assert(fabs(tgamma(3.5) - 2.5*tgamma(2.5)) < ERR);

        errno = 0;
        volatile double d = tgamma(-1.0);
        if (math_errhandling & MATH_ERRNO) {
            if (errno == ERANGE)
                assert(d == HUGE_VAL);
            else
                assert(errno == EDOM);
        }

        assert(fabs(lgamma(3.5) - log(tgamma(3.5))) < ERR);
    }

    /* Floating point manipulation functions. */
    {
        /*
        # ldexp

            ldexp(x, y) = x * (2^y)
        */
        {
            assert(fabs(ldexp(1.5, 2.0) - 6.0) < ERR);
        }

        /*
        # nextafter

            Return the next representable value in a direction.

            If both arguments equal, return them.

        # nexttowards

            TODO0 diff from nextafter
        */
        {
            printf("nexttowards(0.0, 1.0) = %a\n", nexttoward(0.0, 1.0));
            assert(nexttoward(0.0, 0.0) == 0.0);
            printf("nextafter  (0.0, 1.0) = %a\n", nextafter(0.0, 1.0));
        }
    }

    /*
    # random

    # srand

        Seed the random number generator.

        It is very common to seed with `time(NULL)` for simple applications.

    # rand

        Get a uniformly random `int` between 0 and RAND_MAX.

        For cryptographic applications, use a library:
        http://crypto.stackexchange.com/questions/15662/how-vulnerable-is-the-c-rand-in-public-cryptography-protocols

        On Linux, `/dev/random` is the way to go.

        Intel introduced a RdRand in 2011, but as of 2015 it is not widely used,
        and at some point was used as part of the entropy of `/dev/random`.
    */
    {
        srand(time(NULL));

        /* Integer between 0 and RAND_MAX: */
        {
            int i = rand();
        }

        /* int between 0 and 99: */
        {
            int i = rand() % 99;
        }

        /* float between 0 and 1: */
        float f = rand()/(float)RAND_MAX;
    }

    /*
    # IEEE-754

        IEC 60559 has the same contents as the IEEE 754-2008,
        Outside of the C standard it is commonly known by the IEEE name, or simply as IEEE floating point.

        IEEE dates from 1985.

    # __STDC_IEC_559__

    # IEC 60599

        Standard on which floating point formats and operations should be available
        on an implementation, and how they should work.

        Good overview wiki article: <http://en.wikipedia.org/wiki/IEEE_floating_point>

        Many CUPs implement large parts of IEC 60599, which C implementations can use if available.

        The C standard specifies that implementing the IEC 60599 is not mandatory.

        If the macro `__STDC_IEC_559__` is defined this means that the implementation is compliant
        to the interface specified in Annex F of the C11 standard.

        C99 introduced many features which allow greater conformance to IEC 60599.
    */
    {
#ifdef __STDC_IEC_559__
    puts("__STDC_IEC_559__");

    /*
    C float is 32 bits, double 64 bits.

    long double extende precision, and could be one of the format not spceified by IEC.

    IEC explicitly allows for extended formats, and makes basic restrictions such that
    its exponent should have more bits than the preceding type.

    This is probably the case to accomodate x86's 80 bit representation.
    */
    {
        assert(sizeof(float) == 4);
        assert(sizeof(double) == 8);
    }
#endif
    }

    /*
    # division by 0

        Time to have some fun and do naughty things.

        The outcome of a division by zero depends on wether it is an integer of floating operation.

    # isfinite
    # isinf
    # isnan
    # isnormal
    # fpclassify

        FP_INFINITE, FP_NAN, FP_NORMAL, FP_SUBNORMAL, FP_ZERO
    */
    {
        /*
        # floating point exception

            In x86, the following generates a floating point exception,
            which is handled by a floating point exception handler function.

            In Linux the default handler is implemented by the OS and sends a signal to our application,
            which if we don't catch will kill us.
        */
        if (0) {
            /* gcc 4.7 is smart enough to warn on literal division by 0: */
            {
                /*int i = 1 / 0;*/
            }

            /* gcc 4.7 is not smart enough to warn here: */
            {
                volatile int i = 0;
                printf("int 1/0 = %d\n", 1 / i);

                /* On gcc 4.7 with `-O3` this may not generate an exception, */
                /* as the compiler replaces 0 / X by 0. */
                printf("int 0/0 = %d\n", 0 / i);
            }
        }

        /*
        # HUGE_VAL

            Returned on overflow.

            Can equal `INFINITY`.
        */
        {
            /* double */
            printf("HUGE_VAL = %f\n", HUGE_VAL);
            /* float */
            printf("HUGE_VALF = %f\n", HUGE_VALF);
            /* long double */
            printf("HUGE_VALL = %Lf\n", HUGE_VALL);
        }

        /*
        # INFINITY

            Result of operations such as:

                1.0 / 0.0

            Type: float.

            There are two infinities: positive and negative.

            It is possible that `INFINITY == HUGE_VALF`.
        */
        {
            printf("INFINITY = %f\n", INFINITY);
            printf("-INFINITY = %f\n", -INFINITY);

            volatile float f = 0;
            assert(1 / f == INFINITY);
            assert(isinf(INFINITY));
            assert(! isnan(INFINITY));

            assert(INFINITY + INFINITY == INFINITY);
            assert(INFINITY + 1.0      == INFINITY);
            assert(INFINITY - 1.0      == INFINITY);
            assert(isnan(INFINITY - INFINITY));

            assert(INFINITY *  INFINITY ==  INFINITY);
            assert(INFINITY * -INFINITY == -INFINITY);
            assert(INFINITY *  2.0      ==  INFINITY);
            assert(INFINITY * -1.0      == -INFINITY);
            assert(isnan(INFINITY * 0.0));

            assert(1.0 / INFINITY == 0.0);
            assert(isnan(INFINITY / INFINITY));

            /* Comparisons with INFINITY all work as expected. */
            assert(INFINITY == INFINITY);
            assert(INFINITY != - INFINITY);
            assert(-INFINITY < - 1e100);
            assert(1e100 < INFINITY);
        }

        /*
        # NAN

            Not a number.

            Result of operations such as:

                0.0 / 0.0
                INFINITY - INFINITY
                INFINITY * 0.o
                INFINITY / INFINITY

            And any operation involving NAN.

            The sign of NAN has no meaning.
        */
        {
            printf("NAN = %f\n", NAN);
            printf("-NAN = %f\n", -NAN);

            /* TODO why do both fail? */
            /*assert(0 / f == -NAN);*/
            /*assert(0 / f == NAN);*/

            volatile float f = 0;
            assert(isnan(0 / f));
            assert(isnan(NAN));
            assert(!isinf(NAN));

            assert(isnan(NAN));
            assert(isnan(NAN + 1.0));
            assert(isnan(NAN + INFINITY));
            assert(isnan(NAN + NAN));
            assert(isnan(NAN - 1.0));
            assert(isnan(NAN * 2.0));
            assert(isnan(NAN / 1.0));
            assert(isnan(INFINITY - INFINITY));
            assert(isnan(INFINITY * 0.0));
            assert(isnan(INFINITY / INFINITY));

            /*
            NAN is not ordered. any compairison to it yields false!

            This is logical since 0 is neither smaller, larger or equal to NAN.
            */
            {
                assert(!(0.0 < NAN));
                assert(!(0.0 > NAN));
                assert(!(0.0 == NAN));
            }
        }
    }

    return EXIT_SUCCESS;
}
