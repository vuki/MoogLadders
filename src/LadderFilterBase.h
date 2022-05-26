#pragma once

#ifndef LADDER_FILTER_BASE_H
#define LADDER_FILTER_BASE_H

#include "Util.h"

class LadderFilterBase
{
public:
	
	LadderFilterBase(float sampleRate) : sampleRate(sampleRate) {}
	virtual ~LadderFilterBase() {}
	
	virtual void Process(float * samples, uint32_t n) = 0;
	virtual void SetResonance(float r) = 0;
	virtual void SetCutoff(float c) = 0;
	virtual void SetDrive(float d) {drive = d;}

	float GetResonance() const { return resonance; }
	float GetCutoff() const { return cutoff; }
	float GetDrive() const { return drive; }

protected:
	
	float cutoff;
	float resonance;
	float drive;
	float sampleRate;
};

#endif
