#include "fusion.h"

// Weighted average: sensor 2 is assumed more accurate → higher weight.
// Update these once your characterization data is in.
static const float W1 = 0.35f;
static const float W2 = 0.65f;
static const float ALPHA = 0.30f; // EMA smoothing (0 < α ≤ 1)

// Per-node EMA state. Keyed by node index (0 = master, 1..3 = remotes).
#define FUSION_MAX_NODES 8
static bool ema_init[FUSION_MAX_NODES] = {false};
static float ema_val[FUSION_MAX_NODES] = {0.0f};

// Each call needs to know which node it's fusing for, so the EMA
// state stays separate. We use a static "current slot" that the
// caller rotates (see note below).
static int current_slot = 0;

void fusion_set_slot(int slot)
{
    if (slot >= 0 && slot < FUSION_MAX_NODES)
        current_slot = slot;
}

float fuse_temperature(const SensorReading &s)
{
    float instant = (W1 * s.t1 + W2 * s.t2) / (W1 + W2);

    int i = current_slot;
    if (!ema_init[i])
    {
        ema_val[i] = instant;
        ema_init[i] = true;
    }
    else
    {
        ema_val[i] = ALPHA * instant + (1.0f - ALPHA) * ema_val[i];
    }
    return ema_val[i];
}