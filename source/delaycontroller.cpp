// source/delaycontroller.cpp
//------------------------------------------------------------------------
// Copyright(c) 2026 matsu-cmd.
//------------------------------------------------------------------------

#include "delaycontroller.h"
#include "delaycids.h"
#include <string>
#include <cstring>

using namespace Steinberg;

namespace matsucmd {

    // Helper function to convert ASCII to UTF-16
    void setString128(Vst::String128 dest, const char* src)
    {
        // Clear destination
        for (int i = 0; i < 128; i++) {
            dest[i] = 0;
        }

        // Convert ASCII to UTF-16 (simple mapping, ASCII chars are same in UTF-16)
        size_t len = strlen(src);
        for (size_t i = 0; i < len && i < 127; i++) {
            dest[i] = src[i];
        }
        dest[len] = 0;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniController::initialize(FUnknown* context)
    {
        tresult result = EditControllerEx1::initialize(context);
        if (result != kResultOk)
            return result;

        Vst::ParameterInfo info;

        // Clear the whole structure first
        memset(&info, 0, sizeof(info));

        // Delay Time
        info.id = 0;
        info.stepCount = 0;
        info.defaultNormalizedValue = 0.246;
        info.flags = Vst::ParameterInfo::kCanAutomate;
        setString128(info.title, "Delay Time");
        setString128(info.units, "ms");
        parameters.addParameter(info);

        // Feedback
        memset(&info, 0, sizeof(info));
        info.id = 1;
        info.defaultNormalizedValue = 0.5;
        info.stepCount = 100;
        info.flags = Vst::ParameterInfo::kCanAutomate;
        setString128(info.title, "Feedback");
        setString128(info.units, "%");
        parameters.addParameter(info);

        // Mix
        memset(&info, 0, sizeof(info));
        info.id = 2;
        info.defaultNormalizedValue = 0.3;
        info.stepCount = 100;
        info.flags = Vst::ParameterInfo::kCanAutomate;
        setString128(info.title, "Mix");
        setString128(info.units, "%");
        parameters.addParameter(info);

        // Bypass
        memset(&info, 0, sizeof(info));
        info.id = 3;
        info.defaultNormalizedValue = 0.0;
        info.stepCount = 1;
        info.flags = Vst::ParameterInfo::kCanAutomate;
        setString128(info.title, "Bypass");
        info.units[0] = 0;
        parameters.addParameter(info);

        return kResultOk;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniController::terminate()
    {
        return EditControllerEx1::terminate();
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniController::setComponentState(IBStream* state)
    {
        return kResultOk;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniController::setState(IBStream* state)
    {
        return kResultTrue;
    }

    //------------------------------------------------------------------------
    tresult PLUGIN_API DelayUniController::getState(IBStream* state)
    {
        return kResultTrue;
    }

    //------------------------------------------------------------------------
    IPlugView* PLUGIN_API DelayUniController::createView(FIDString name)
    {
        return nullptr;
    }

    //------------------------------------------------------------------------
} // namespace matsucmd