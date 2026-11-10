//------------------------------------------------------------------------
// Copyright(c) 2026 matsu-cmd.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace matsucmd {
//------------------------------------------------------------------------
static const Steinberg::FUID kDelayUniProcessorUID (0x17E9695C, 0xE0E75467, 0x875D23F2, 0x93CA69DB);
static const Steinberg::FUID kDelayUniControllerUID (0xE53050B6, 0xFE1255D6, 0xB54BFC19, 0xBCD48AE8);

#define DelayUniVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace matsucmd
