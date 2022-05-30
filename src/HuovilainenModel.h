/*
Antti Huovilainen's Moog ladder filter.
Non-Linear Digital Implementation of the Moog Ladder Filter
Antti Huovilainen
DAFx-2004 - Naples
https://www.dafx.de/paper-archive/2004/P_061.PDF
*/

#pragma once

#ifndef HUOVILAINEN_LADDER_H
#define HUOVILAINEN_LADDER_H

#include <cmath>
#include "LadderFilterBase.h"

class HuovilainenMoog : public LadderFilterBase {
public:
    HuovilainenMoog(float sampleRate)
        : LadderFilterBase(sampleRate)
		, i2vt(1.f / 64.f)
		, g(1.f)
        , maxDrive(2.f)
		, dcGainComp(0)
    {
        stage[0] = stage[1] = stage[2] = stage[3] = 0;
        stageTanh[0] = stageTanh[1] = stageTanh[2] = 0;
        SetCutoff(1000.0f);
        SetResonance(0.10f);
        SetDrive(0);
    }

    virtual ~HuovilainenMoog() { }

    virtual void Process(float* samples, uint32_t n) override
    {
        for (uint32_t s = 0; s < n; ++s) {
            const float x = samples[s] - 4.f * resonance * acr * fb;
            stage[0] += g * (std::tanh(x * i2vt) - stageTanh[0]);
            stageTanh[0] = std::tanh(stage[0] * i2vt);
            stage[1] += g * (stageTanh[0] - stageTanh[1]);
            stageTanh[1] = std::tanh(stage[1] * i2vt);
            stage[2] += g * (stageTanh[1] - stageTanh[2]);
            stageTanh[2] = std::tanh(stage[2] * i2vt);
            fb = stage[3];
            stage[3] += g * (stageTanh[2] - tanh(stage[3] * i2vt));
            fb = (stage[3] + fb) * 0.5f;
            samples[s] = gComp * stage[3];
        }
    }

    virtual void SetResonance(float r) override
    {
        resonance = r;
		gComp = 1.f + 4.f * dcGainComp * resonance;
    }

    virtual void SetCutoff(float c) override
    {
        cutoff = c;
        const double fc = 2.f * cutoff / sampleRate;
        const double fc2 = fc * fc;
        const double fcr = 1.8730 * fc2 * fc + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
        acr = static_cast<float>(-3.9364 * fc2 + 1.8409 * fc + 0.9968);
        tune = static_cast<float>((1.0 - std::exp(-MOOG_PI * fc * fcr)));
		g = tune / i2vt;
    }

    virtual void SetDrive(float d) override
    {
        // d is in range 0 to 1
        drive = d;
        i2vt = (drive * maxDrive * 64.f + 1.f) / 64.f;
        g = tune / i2vt;
    }

private:
    float stage[4];
    float stageTanh[3];
    float fb;
    float tune;
    float acr;
	float i2vt;
	float g;
    float maxDrive;
	float dcGainComp;
	float gComp;
};

#endif
