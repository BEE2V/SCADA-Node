#include "fusion.h"
#include <math.h>

// ---------------------------------------------------------------
// Weighted-average sensor fusion
// w1 + w2 = 1
// Sensor 2 is assumed more accurate -> higher weight
// Replace these weights once you have your characterization data.
// ---------------------------------------------------------------
static const float W1 = 0.35f;
static const float W2 = 0.65f;

// Exponential Moving Average smoothing factor
// 0 < ALPHA <= 1 ; smaller -> smoother but slower
static const float ALPHA = 0.30f;

static bool ema_initialized = false;
static float ema_value = 0.0f;

float fuse_temperature(const SensorReading &s)
{
    // Instantaneous weighted average
    float wsum = W1 + W2;
    float instant = (W1 * s.t1 + W2 * s.t2) / wsum;

    // Apply EMA to reduce noise
    if (!ema_initialized)
    {
        ema_value = instant;
        ema_initialized = true;
    }
    else
    {
        ema_value = ALPHA * instant + (1.0f - ALPHA) * ema_value;
    }
    return ema_value;
}