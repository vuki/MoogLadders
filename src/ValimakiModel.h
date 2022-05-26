/*
 * Moog ladder filter from:
 * Vesa Välimäki, Antti Huovilainen
 * Oscillator and Filter Algorithms for Virtual Analog Synthesis
 * Computer Music Journal 30(2), pp. 19-31, 2006.
 * DOI: 10.1162/comj.2006.30.2.19
 */

#pragma once

#ifndef VALIMAKI_MOOG_H
#define VALIMAKI_MOOG_H

#include "LadderFilterBase.h"
#include "util.h"

const float max_drive = 2.f;

class ValimakiMoog : public LadderFilterBase {

public:
    ValimakiMoog(float sampleRate)
        : LadderFilterBase(sampleRate)
    {
        gcomp = 0; // 0.5f: not working correctly
        reset();
        SetCutoff(1000.0f);
        SetResonance(0.10f);
        SetDrive(0);
    }

    virtual ~ValimakiMoog() { }

    virtual void Process(float* samples, uint32_t n) override
    {
        for (uint32_t s = 0; s < n; ++s) {
            const float x = scaler * samples[s];
            const float y0 = fast_tanhf(x - 4.f * res * gres * (yp[4] - gcomp * x));
            const float y1 = g1 * y0 + g2 * yp[0] + g3 * yp[1];
            const float y2 = g1 * y1 + g2 * yp[1] + g3 * yp[2];
            const float y3 = g1 * y2 + g2 * yp[2] + g3 * yp[3];
            const float y4 = g1 * y3 + g2 * yp[3] + g3 * yp[4];

            yp[0] = y0;
            yp[1] = y1;
            yp[2] = y2;
            yp[3] = y3;
            yp[4] = y4;
            samples[s] = out_scaler * y4;
        }
    }

    virtual void SetResonance(float r) override
    {
        resonance = r;
        res = r;
    }

    virtual void SetCutoff(float c) override
    {
        cutoff = c;
        const float wc = 2.f * static_cast<float>(MOOG_PI) * c / sampleRate;
        const float wc2 = wc * wc;
        gres = 1.0029f + 0.0526f * wc - 0.0926f * wc2 + 0.0218f * wc2 * wc;
        const float g = 0.9892f * wc - 0.4342f * wc2 + 0.1381f * wc * wc2 - 0.0202f * wc2 * wc2;
        g1 = g / 1.3f;
        g2 = g * 0.3f / 1.3f;
        g3 = 1.f - g;
    }

    virtual void SetDrive(float d) override
    {
        // d is in range 0 to 1
        drive = d;
        scaler = (drive * max_drive * 64.f + 1.f) / 64.f;
        out_scaler = 1.f / fast_tanhf(scaler);
    }

    void reset()
    {
        yp[0] = yp[1] = yp[2] = yp[3] = yp[4] = 0;
    }

private:
    float yp[5];
    float g1, g2, g3, gres, gcomp;
    float res;
    float scaler, out_scaler;
};

#endif
