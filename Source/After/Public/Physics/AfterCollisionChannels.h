#pragma once

// Keep track of the actors/components that provide interaction.
#define After_TraceChannel_Interaction ECC_GameTraceChannel1

// The trail used by the weapon will hit physical objects, not capsules.
#define After_TraceChannel_Weapon ECC_GameTraceChannel2

// The trail used by the weapon will hit pawn capsules instead of physical assets.
#define After_TraceChannel_Weapon_Capsule ECC_GameTraceChannel3

// The trail used by the weapon will go through several pawns, rather than stopping at the first hit.
#define After_TraceChannel_Weapon_Multi ECC_GameTraceChannel4