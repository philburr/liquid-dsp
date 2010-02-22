/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Bessel filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

#define LIQUID_DEBUG_BESSEL_PRINT   0

// forward declarations

void fpoly_bessel(unsigned int _n, float * _p);

void fpoly_bessel_roots(unsigned int _n, float complex * _roots);

void fpoly_bessel_roots_orchard(unsigned int _n, float complex * _roots);

void fpoly_bessel_roots_orchard_recursion(unsigned int _n,
                                          float _x,
                                          float _y,
                                          float * _x_hat,
                                          float * _y_hat);

// ****************************************

void bessel_azpkf(unsigned int _n,
                  float complex * _za,
                  float complex * _pa,
                  float complex * _ka)
{
    // compute poles (roots to Bessel polynomial)
    fpoly_bessel_roots(_n+1,_pa);

    // analog Bessel filter prototype has no zeros

#if 0
    // compute gain using standard math log(gamma(z))
    unsigned int N = _n;
    unsigned int k = 0;
    float t0 = lgammaf((float)(2*N-k)+1);
    float t1 = lgammaf((float)(N-k)  +1);
    float t2 = lgammaf((float)(k)    +1);

    // M_LN2 = log(2) = 0.693147180559945
    float t3 = M_LN2 * (float)(N-k);    // log(2^(N-k)) = log(2)*log(N-k)

    *_ka = roundf(expf(t0 - t1 - t2 - t3));
#else
    *_ka = 1.0f;
#endif
}

void fpoly_bessel(unsigned int _n, float * _p)
{
    unsigned int k;
    unsigned int N = _n-1;
    for (k=0; k<_n; k++) {
#if 0
        // use internal log(gamma(z))
        float t0 = liquid_lngammaf((float)(2*N-k)+1);
        float t1 = liquid_lngammaf((float)(N-k)  +1);
        float t2 = liquid_lngammaf((float)(k)     +1);
#else
        // use standard math log(gamma(z))
        float t0 = lgammaf((float)(2*N-k)+1);
        float t1 = lgammaf((float)(N-k)  +1);
        float t2 = lgammaf((float)(k)    +1);
#endif

        // M_LN2 = log(2) = 0.693147180559945
        float t3 = M_LN2 * (float)(N-k);    // log(2^(N-k)) = log(2)*log(N-k)

        _p[k] = roundf(expf(t0 - t1 - t2 - t3));

#if 0
        printf("  p[%3u,%3u] = %12.4e\n", k, _n, _p[k]);
        printf("    t0 : %12.4e\n", t0);
        printf("    t1 : %12.4e\n", t1);
        printf("    t2 : %12.4e\n", t2);
        printf("    t3 : %12.4e\n", t3);
#endif
    }
}

void fpoly_bessel_roots(unsigned int _n,
                        float complex * _roots)
{
    //if (_n < 11) {
    if (0) {
        float p[_n];
        fpoly_bessel(_n,p);
        fpoly_findroots(p,_n,_roots);
    } else {
        float m0 = -0.668861023825672*_n + 0.352940768662957;
        float m1 = 1.0f / (1.6013579390149844*_n - 0.0429146801453954);

        int i;
        int r = _n%2;
        int L = (_n-r)/2;
        float ri, rq;
        for (i=0; i<_n; i++) {
            rq = (i - L - r + 0.5)*1.778f;
            ri = m0 * m1*rq*rq;

            printf("  [%3u] : %12.8f + j*%12.8f\n", i, ri, rq);
            fpoly_bessel_roots_orchard_recursion(_n,ri,rq,&ri,&rq);
            _roots[i] = ri + _Complex_I*rq;
        }
    }
}

void fpoly_bessel_roots_orchard(unsigned int _n,
                                float complex * _roots)
{
    // make initial guesses at roots

    // run recursion for each root estimate
    unsigned int i;
    for (i=0; i<_n; i++) {
        float x = 1.0f;
        float y = 0.0f;
        float x_hat, y_hat;
        fpoly_bessel_roots_orchard_recursion(_n,x,y,&x_hat,&y_hat);
    }

    // TODO : check for uniqueness
}

void fpoly_bessel_roots_orchard_recursion(unsigned int _n,
                                          float _x,
                                          float _y,
                                          float * _x_hat,
                                          float * _y_hat)
{
    if (_n < 2) {
        fprintf(stderr,"error: fpoly_bessel_roots_orchard_recursion(), n < 2\n");
        exit(1);
    }
    float u0, u1, u2=0, u2p=0;
    float v0, v1, v2=0, v2p=0;
    float x = _x;
    float y = _y;

    unsigned int k,i;
    unsigned int num_iterations = 10;
    for (k=0; k<num_iterations; k++) {
        u0 = 1.0;
        u1 = 1.0 + x;

        v0 = 0.0f;
        v1 = y;

        // compute u_r, v_r
        for (i=2; i<_n; i++) {
            u2 = (2*i-1)*u1 + (x*x - y*y)*u0 - 2*x*y*v0;
            v2 = (2*i-1)*v1 + (x*x - y*y)*v0 + 2*x*y*u0;

            // update u[r-1], v[r-1]
            u0 = u1;
            v0 = v1;

            // update u[r-2], v[r-2]
            u1 = u2;
            v1 = v2;
        }

        // compute derivatives
        u2p = u2 - x*u1 + y*v1;
        v2p = v2 - x*v1 - y*u1;

        // update roots
        float g = u2p*u2p + v2p*v2p;
        x -= (u2p*u2 + v2p*v2)/g;
        y -= (u2p*v2 - v2p*u2)/g;
    }

    *_x_hat = x;
    *_y_hat = y;
}

