/* -*- mode: C; c-basic-offset: 4; intent-tabs-mode: nil -*-
 *
 * STIR -- Sifteo Tiled Image Reducer
 * M. Elizabeth Scott <beth@sifteo.com>
 *
 * Copyright <c> 2011 Sifteo, Inc. All rights reserved.
 */

#ifndef _ENCODER_H
#define _ENCODER_H

#include <stdio.h>
#include <vector>
#include <string>
#include <speex/speex.h>


class AudioEncoder {
public:
    virtual ~AudioEncoder() {};
    virtual void encodeFile(const std::string &path, std::vector<uint8_t> &out) = 0;
    virtual const char *getTypeSymbol() = 0;
    
    static AudioEncoder *create(std::string name, int quality);
};


class SpeexEncoder : public AudioEncoder {
public:
    SpeexEncoder(int quality);
    virtual ~SpeexEncoder();
    virtual void encodeFile(const std::string &path, std::vector<uint8_t> &out);

    virtual const char *getTypeSymbol() {
        return "_SYS_Speex";
    }

private:
    // max sizes from speexenc
    static const unsigned MAX_FRAME_SIZE     = 2000;
    static const unsigned MAX_FRAME_BYTES    = 2000;
    
    SpeexBits bits;
    void *encoderState;
    int frameSize;
};


class PCMEncoder : public AudioEncoder {
public:
    virtual void encodeFile(const std::string &path, std::vector<uint8_t> &out);

    virtual const char *getTypeSymbol() {
        return "_SYS_PCM";
    }
};

#endif
