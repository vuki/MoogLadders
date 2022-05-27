/*
Moog ladder filter from:
Generalized Moog Ladder Filter: Part II
Explicit Nonlinear Model through a NovelDelay-Free Loop Implementation Method
S. D’Angelo and V. Välimäki,
IEEE/ACM Trans. Audio, Speech, and Lang. Process.,
vol. 22, no. 12, pp. 1873–1883, December 2014. DOI: 10.1109/TASLP.2014.2352556.
https://www.dangelo.audio/#research

Copyright (C) 2014 Stefano D'Angelo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#ifndef DANGELO_MOOG_H
#define DANGELO_MOOG_H

#include <cmath>
#include "LadderFilterBase.h"

class DAngeloMoog : public LadderFilterBase {

public:
    DAngeloMoog(float sampleRate)
        : LadderFilterBase(sampleRate)
        , gainComp(0)
        , alpha(1.f)
    {
        VT = 32.f; // empirical, to use linear section of tanh
        VT2 = 2 * VT;
        VT2i = 1.f / VT2;
        reset();
        SetCutoff(1000.0f);
        SetResonance(0);
    }

    virtual ~DAngeloMoog() { }

    virtual void Process(float* samples, uint32_t n) override
    {
        for (uint32_t s = 0; s < n; ++s) {
            float yo = std::tanh(k0g * (samples[s] + sg[0]));
            float yn;
            for (int n = 0; n < 4; n++) {
                const float yi = yo;
                const float yd = k0s * (yi + sf[n]);
                yn = yd + si[n];
                yo = std::tanh(VT2i * yn);
                si[n] = yd + yn;
                sf[n] = r1s * yi - q0s * yo;
                out[n] = yn;
            }
            const float yf = k * yn;
            sg[0] = rg[0] * samples[s] + qg[0] * yf + sg[1];
            sg[1] = rg[1] * samples[s] + qg[1] * yf + sg[2];
            sg[2] = rg[2] * samples[s] + qg[2] * yf + sg[3];
            sg[3] = rg[3] * samples[s] + qg[3] * yf;

            samples[s] = out[3] * (1.f + gainComp * k);
        }
    }

    virtual void SetResonance(float r) override
    {
        resonance = r;
        k = 4.f * r;
        alpha = std::sqrt(1.f + std::pow(k, 0.5f) - 2.f * std::pow(k, 0.25f) * 0.7071067811865476f);
        computeFilter();
    }

    virtual void SetCutoff(float c) override
    {
        cutoff = c;
        computeFilter();
    }

private:
    float gainComp; // DC gain compensation, 0 to 1
    float k, alpha;
    float VT, VT2, VT2i;
    float q0s, r1s, k0s, k0g;
    float si[4];
    float sf[4];
    float sg[4];
    float rg[4];
    float qg[4];
    float out[4];

    void reset()
    {
        for (int i = 0; i < 4; i++) {
            si[i] = sf[i] = sg[i] = out[i] = 0;
        }
    }

    void computeFilter()
    {
        const float g = std::tan(MOOG_PI * cutoff / sampleRate) / alpha;
        const float p0s = 1.f / (1.f + g);
        q0s = 1.f - g;
        r1s = -g;
        k0s = VT2 * g * p0s;
        const float kgN = k * std::pow(1.f - p0s, 4.f);
        const float p0g = 1.f / (1.f + kgN);
        const float binom[4] = { 4.f, 6.f, 4.f, 1.f };
        k0g = -VT2i * p0g;
        for (int i = 0; i < 4; i++) {
            rg[i] = -binom[i] * kgN;
            qg[i] = rg[i] - binom[i] * std::pow((g - 1.f) * p0s, i + 1.f);
        }
    }
};

#endif
