//
//  LFO.h
//  lo-fi-juicer
//
//  Created by Howard  on 5/3/21.
//  Copyright © 2021 HowRad. All rights reserved.
//

#ifndef _LFO_H_
#define _LFO_H_

#define pi                     3.14159265358979323846

class Lfo
{
public:
    // constructor and destructor
    Lfo(void);
    ~Lfo(void);
    
    // member variables:
    float f0;
    int fs;
    float phase;
    float phase_inc;
    float output;
    float* output_array;
    
    // member functions:
    void initLFO(float freq, int sampleRate);
};

#endif /* _LFO_H_ */
