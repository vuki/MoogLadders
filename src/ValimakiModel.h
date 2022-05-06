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

class ValimakiMoog : public LadderFilterBase {

public:
    ValimakiMoog(float sampleRate)
        : LadderFilterBase(sampleRate)
    {
        b0 = 1.0f / 1.3f;
        b1 = 0.3f / 1.3f;
        g_comp = 0.5f;
        SetCutoff(1000.0f);
        SetResonance(0.10f);
    }

    virtual ~ValimakiMoog() { }

    virtual void Process(float* samples, uint32_t n) override
    {
        for (uint32_t s = 0; s < n; ++s) {
            const float x = fast_tanhf(samples[s] - 4.0f * mg_res * mg_gres * (mg_y4 - g_comp * samples[s]));
            mg_y1 = mg_g * (b0 * x + b1 * mg_prev_x - mg_prev_y1) + mg_prev_y1;
            mg_y2 = mg_g * (b0 * mg_y1 + b1 * mg_prev_y1 - mg_prev_y2) + mg_prev_y2;
            mg_y3 = mg_g * (b0 * mg_y2 + b1 * mg_prev_y2 - mg_prev_y3) + mg_prev_y3;
            mg_y4 = mg_g * (b0 * mg_y3 + b1 * mg_prev_y3 - mg_y4) + mg_y4;

            mg_prev_x = x;
            mg_prev_y1 = mg_y1;
            mg_prev_y2 = mg_y2;
            mg_prev_y3 = mg_y3;

            samples[s] = mg_y4;
        }
    }

    virtual void SetResonance(float r) override
    {
        resonance = r;
        mg_res = r;
    }

    virtual void SetCutoff(float c) override
    {
        cutoff = c;
        const float wc = 2.f * MOOG_PI * c / sampleRate;
        const float wc2 = wc * wc;
        mg_g = 0.9892f * wc - 0.4342f * wc2 + 0.1381f * wc * wc2 - 0.0202f * wc2 * wc2;
        mg_gres = 1.0029f + 0.0526f * wc - 0.0926f * wc2 + 0.0218f * wc2 * wc;
    }

private:
    float mg_y1, mg_y2, mg_y3, mg_y4;
    float mg_prev_y1, mg_prev_y2, mg_prev_y3, mg_prev_x;
    float mg_res, mg_g, mg_gres;
    float b0, b1, g_comp;
};

#endif
