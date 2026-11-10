// source/paramids.h
//------------------------------------------------------------------------
// Copyright(c) 2026 matsu-cmd.
//------------------------------------------------------------------------

#pragma once

namespace matsucmd {

    // Parameter IDs
    enum ParamIDs {
        kParamDelayTime = 0,    // Delay time (10-2000ms)
        kParamFeedback = 1,     // Feedback (0-100%)
        kParamMix = 2,          // Dry/Wet mix (0-100%)
        kParamBypass = 3,       // Bypass switch
        kNumParams = 4
    };

} // namespace matsucmd