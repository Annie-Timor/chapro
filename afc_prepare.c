// afc_prepare.c - adaptive-feedback-cancelation preparation functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "chapro.h"
#include "cha_ff.h"
#include "ite_fb.h"

/***********************************************************/

FUNC(int)
cha_afc_prepare(CHA_PTR cp, double mu, double rho, double eps, int afl, 
                int wfl, int pfl, int hdel, double fbg, int sqm)
{
    double fbm = 0;
    float *sfbp, *wfrp, *ffrp;
    int i, cs, fbl, mxl = 0, nqm = 0, rsz = 32;

    cha_prepare(cp);
    // allocate HA-output ring buffer
    cs = CHA_IVAR[_cs];
    fbl = (fbg > 0) ? FBSZ : 0;
    mxl = (fbl > afl) ? fbl : (afl > wfl) ? afl : (wfl > pfl) ? wfl : pfl;
    while (rsz < (mxl + hdel + cs))  rsz *= 2;
    CHA_IVAR[_rsz] = rsz;
    CHA_IVAR[_mxl] = mxl;
    cha_allocate(cp, rsz, sizeof(float), _rng0);
    // allocate AFC-filter buffer
    if (afl > 0) {
        cha_allocate(cp, afl, sizeof(float), _efbp);
    }
    CHA_DVAR[_mu]  = mu;
    CHA_DVAR[_rho] = rho;
    CHA_DVAR[_eps] = eps;
    CHA_IVAR[_afl] = afl;
    // initialize signal-whitening filter
    if (wfl > 0) {
        cha_allocate(cp, rsz, sizeof(float), _rng3); // ee -> rng3
        cha_allocate(cp, rsz, sizeof(float), _rng2); // uf -> rng2
        wfrp = cha_allocate(cp, wfl, sizeof(float), _wfrp);
        wfrp[0] = 1;
    }
    CHA_IVAR[_wfl] = wfl;
    // initialize persistent-feedback filter
    if (pfl > 0) {
        cha_allocate(cp, rsz, sizeof(float), _rng1); // uu -> rng1
        ffrp = cha_allocate(cp, pfl, sizeof(float), _ffrp);
        ffrp[0] = 1;
    }
    CHA_IVAR[_pfl] = pfl;
    // initialize simulated feedback path
    if (fbl > 0) {
        cha_allocate(cp, fbl, sizeof(float), _sfbp);
        sfbp = (float *) cp[_sfbp];
        fbm = 0;
        for (i = 0; i < fbl; i++) {
            sfbp[i] = (float) (ite_fbp[i] * fbg);
            fbm += sfbp[i] * sfbp[i];
        }
    }
    CHA_IVAR[_fbl] = fbl;
    CHA_DVAR[_fbm] = fbm;
    // initialize quality metrics
    if (sqm && (afl > 0) && (fbl > 0)) {
        nqm = (fbl < afl) ? fbl : afl;
        cha_allocate(cp,  cs, sizeof(float), _merr);
    }
    CHA_IVAR[_nqm] = nqm;
    // initialize hardware delay
    CHA_IVAR[_hdel] = hdel; // should this be 38 ???

    return (0);
}
