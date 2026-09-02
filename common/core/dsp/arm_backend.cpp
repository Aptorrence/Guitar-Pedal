#include "arm_backend.h"

#include "arm_math.h"

namespace dsp
{
    void scale(std::span<float> block, float gain)
    {
        arm_scale_f32(block.data(), gain, block.data(), static_cast<uint32_t>(block.size()));
    }
} // namespace dsp
