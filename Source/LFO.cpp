//
//  LFO.cpp
//  lo-fi-juicer - VST3
//
//  Created by Howard  on 5/3/21.
//  Copyright © 2021 HowRad. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "LFO.h"

Lfo::Lfo(void)
{
    ;
}

Lfo::~Lfo(void)
{
    ;
}

void Lfo::initLFO(float freq, int sampleRate)
{
    this->f0 = freq;
    this->fs = sampleRate;
    this->phase = 0.0;
    this->phase_inc = 2 * pi * this->f0 / this->fs;
    this->output = sin(phase);
}

