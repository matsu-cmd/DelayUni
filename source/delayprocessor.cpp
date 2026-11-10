// source/delayprocessor.cpp
//------------------------------------------------------------------------
// Copyright(c) 2026 matsu-cmd.
//------------------------------------------------------------------------

#include "delayprocessor.h"
#include "delaycids.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include <cstring>
#include <cmath>

using namespace Steinberg;

namespace matsucmd {

    //------------------------------------------------------------------------
    DelayUniProcessor::DelayUniProcessor()
        : delayBufferL(nullptr)
        , delayBufferR(nullptr)
        , delayBufferSize(0)
        , writePosL(0)
        , writePosR(0)
        , sampleRate(44100.0)
        , delayTimeMs(500.0f)
        , feedback(0.5f)
        , mix(0.3f)
        , bypass(false)
    {
        setControllerClass(kDelayUniControllerUID);
    }

    //------------------------------------------------------------------------
    DelayUniProcessor::~DelayUniProcessor()
    {
        terminate();
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::initialize(FUnknown* context)
    {
        tresult result = AudioEffect::initialize(context);
        if (result != kResultOk)
            return result;

        addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
        addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);

        delayBufferSize = 88200; // 2 seconds at 44.1kHz
        delayBufferL = new float[delayBufferSize];
        delayBufferR = new float[delayBufferSize];

        clearBuffers();

        return kResultOk;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::terminate()
    {
        if (delayBufferL) {
            delete[] delayBufferL;
            delayBufferL = nullptr;
        }
        if (delayBufferR) {
            delete[] delayBufferR;
            delayBufferR = nullptr;
        }
        return AudioEffect::terminate();
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::setActive(TBool state)
    {
        if (state) {
            clearBuffers();
        }
        return AudioEffect::setActive(state);
    }

    //------------------------------------------------------------------------
    void DelayUniProcessor::clearBuffers()
    {
        if (delayBufferL) {
            memset(delayBufferL, 0, sizeof(float) * delayBufferSize);
        }
        if (delayBufferR) {
            memset(delayBufferR, 0, sizeof(float) * delayBufferSize);
        }
        writePosL = 0;
        writePosR = 0;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
    {
        sampleRate = newSetup.sampleRate;

        // Reallocate buffers if needed
        int newBufferSize = (int)(sampleRate * 2.0);
        if (newBufferSize != delayBufferSize && newBufferSize > 0) {
            delete[] delayBufferL;
            delete[] delayBufferR;

            delayBufferSize = newBufferSize;
            delayBufferL = new float[delayBufferSize];
            delayBufferR = new float[delayBufferSize];
            clearBuffers();
        }

        return AudioEffect::setupProcessing(newSetup);
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::canProcessSampleSize(int32 symbolicSampleSize)
    {
        return (symbolicSampleSize == Vst::kSample32) ? kResultTrue : kResultFalse;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::process(Vst::ProcessData& data)
    {
        if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples == 0)
            return kResultOk;

        if (delayBufferSize == 0)
            return kResultOk;

        // Read parameters from host
        if (data.inputParameterChanges) {
            int32 numParams = data.inputParameterChanges->getParameterCount();
            for (int32 i = 0; i < numParams; i++) {
                Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(i);
                if (paramQueue) {
                    int32 numPoints = paramQueue->getPointCount();
                    if (numPoints > 0) {
                        int32 offset;
                        Vst::ParamValue value;
                        paramQueue->getPoint(numPoints - 1, offset, value);

                        switch (paramQueue->getParameterId()) {
                        case 0: // Delay Time
                            delayTimeMs = 10.0f + (float)(value * 1990.0);
                            break;
                        case 1: // Feedback
                            feedback = (float)value;
                            break;
                        case 2: // Mix
                            mix = (float)value;
                            break;
                        case 3: // Bypass
                            bypass = (value > 0.5);
                            break;
                        }
                    }
                }
            }
        }

        Vst::AudioBusBuffers& inputs = data.inputs[0];
        Vst::AudioBusBuffers& outputs = data.outputs[0];

        int32 numChannels = (inputs.numChannels < 2) ? inputs.numChannels : 2;
        int32 numSamples = data.numSamples;

        // Process each sample
        for (int32 sample = 0; sample < numSamples; sample++) {
            // Calculate delay in samples
            float delaySamplesFloat = (delayTimeMs / 1000.0f) * (float)sampleRate;
            int delaySamples = (int)delaySamplesFloat;

            if (delaySamples < 1) delaySamples = 1;
            if (delaySamples >= delayBufferSize) delaySamples = delayBufferSize - 1;

            // Left channel
            float inputL = inputs.channelBuffers32[0][sample];
            int readPosL = writePosL - delaySamples;
            if (readPosL < 0) readPosL += delayBufferSize;
            float delayedL = delayBufferL[readPosL];

            // Right channel
            float inputR = inputL;
            float delayedR = delayedL;
            if (numChannels > 1) {
                inputR = inputs.channelBuffers32[1][sample];
                int readPosR = writePosR - delaySamples;
                if (readPosR < 0) readPosR += delayBufferSize;
                delayedR = delayBufferR[readPosR];
            }

            if (!bypass) {
                // Apply delay
                float outL = inputL * (1.0f - mix) + delayedL * mix;
                float outR = inputR * (1.0f - mix) + delayedR * mix;

                outputs.channelBuffers32[0][sample] = outL;
                if (outputs.numChannels > 1) {
                    outputs.channelBuffers32[1][sample] = outR;
                }

                // Write to buffer with feedback
                delayBufferL[writePosL] = inputL + delayedL * feedback;
                if (numChannels > 1) {
                    delayBufferR[writePosR] = inputR + delayedR * feedback;
                }
                else {
                    delayBufferR[writePosR] = inputL + delayedL * feedback;
                }
            }
            else {
                // Bypass
                outputs.channelBuffers32[0][sample] = inputL;
                if (outputs.numChannels > 1) {
                    outputs.channelBuffers32[1][sample] = inputR;
                }

                delayBufferL[writePosL] = inputL;
                delayBufferR[writePosR] = (numChannels > 1) ? inputR : inputL;
            }

            // Update write positions
            writePosL++;
            if (writePosL >= delayBufferSize) writePosL = 0;

            writePosR++;
            if (writePosR >= delayBufferSize) writePosR = 0;
        }

        return kResultOk;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::setState(IBStream* state)
    {
        if (!state) return kResultFalse;

        float savedDelayTimeMs = delayTimeMs;
        float savedFeedback = feedback;
        float savedMix = mix;

        if (state->read(&savedDelayTimeMs, sizeof(float)) != kResultOk)
            return kResultFalse;
        if (state->read(&savedFeedback, sizeof(float)) != kResultOk)
            return kResultFalse;
        if (state->read(&savedMix, sizeof(float)) != kResultOk)
            return kResultFalse;

        delayTimeMs = savedDelayTimeMs;
        feedback = savedFeedback;
        mix = savedMix;

        return kResultOk;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniProcessor::getState(IBStream* state)
    {
        if (!state) return kResultFalse;

        if (state->write(&delayTimeMs, sizeof(float)) != kResultOk)
            return kResultFalse;
        if (state->write(&feedback, sizeof(float)) != kResultOk)
            return kResultFalse;
        if (state->write(&mix, sizeof(float)) != kResultOk)
            return kResultFalse;

        return kResultOk;
    }

    //------------------------------------------------------------------------
} // namespace matsucmd