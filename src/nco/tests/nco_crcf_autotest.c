/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

// forward declaration of internal method to constrain phase
uint32_t nco_crcf_constrain(float _theta);

// test phase constraint
void autotest_nco_crcf_constrain()
{
    float delta = (float)0x00001fff;

    // phase: 0 mod 2 pi
    CONTEND_DELTA( nco_crcf_constrain(    0.0f), (float)         0, delta );
    CONTEND_DELTA( nco_crcf_constrain(  2*M_PI), (float)         0, delta );
    CONTEND_DELTA( nco_crcf_constrain(  4*M_PI), (float)         0, delta );
    CONTEND_DELTA( nco_crcf_constrain(  6*M_PI), (float)         0, delta );
    CONTEND_DELTA( nco_crcf_constrain( 20*M_PI), (float)         0, delta );

    // phase: 0 mod 2 pi (negative)
    //CONTEND_DELTA( nco_crcf_constrain(   -0.0f), (float)         0, delta );
    //CONTEND_DELTA( nco_crcf_constrain( -2*M_PI), (float)         0, delta );
    //CONTEND_DELTA( nco_crcf_constrain( -4*M_PI), (float)         0, delta );
    //CONTEND_DELTA( nco_crcf_constrain( -6*M_PI), (float)         0, delta );
    //CONTEND_DELTA( nco_crcf_constrain(-20*M_PI), (float)         0, delta );

    // phase: pi mod 2 pi
    CONTEND_DELTA( nco_crcf_constrain(    M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain(  3*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain(  5*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain(  7*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain( 27*M_PI), (float)0x80000000, delta );

    // phase: pi mod 2 pi (negative)
    CONTEND_DELTA( nco_crcf_constrain(   -M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain( -3*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain( -5*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain( -7*M_PI), (float)0x80000000, delta );
    CONTEND_DELTA( nco_crcf_constrain(-27*M_PI), (float)0x80000000, delta );
}

// test copying object
void autotest_nco_crcf_copy()
{
    // create and initialize object
    nco_crcf nco_0 = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase        (nco_0, 1.23456f);
    nco_crcf_set_frequency    (nco_0, 5.67890f);
    nco_crcf_pll_set_bandwidth(nco_0, 0.011f);

    // copy object
    nco_crcf nco_1 = nco_crcf_copy(nco_0);

    unsigned int i, n = 240;
    float complex v0, v1;
    for (i=0; i<n; i++) {
        // received complex signal
        nco_crcf_cexpf(nco_0,&v0);
        nco_crcf_cexpf(nco_1,&v1);

        // update pll
        nco_crcf_pll_step(nco_0, cargf(v0));
        nco_crcf_pll_step(nco_1, cargf(v1));

        // update nco objects
        nco_crcf_step(nco_0);
        nco_crcf_step(nco_1);

        // check output
        CONTEND_EQUALITY(v0, v1);
    }

    // clean it up
    nco_crcf_destroy(nco_0);
    nco_crcf_destroy(nco_1);
}

