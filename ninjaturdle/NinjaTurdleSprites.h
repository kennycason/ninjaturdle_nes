// Ninja Turdle Sprites
// Contains metasprite definitions for custom sprites

// Ninja Walking sprites (72x32, 3 frames)
const unsigned char NinjaWalkL1[]={
    0xff, 0xff, 0x40, 0,  // top left
    0x07, 0xff, 0x41, 0,  // top right
    0xff, 0x07, 0x50, 0,  // bottom left
    0x07, 0x07, 0x51, 0,  // bottom right
    128
};

const unsigned char NinjaWalkL2[]={
    0xff, 0xff, 0x42, 0,
    0x07, 0xff, 0x43, 0,
    0xff, 0x07, 0x52, 0,
    0x07, 0x07, 0x53, 0,
    128
};

const unsigned char NinjaWalkL3[]={
    0xff, 0xff, 0x44, 0,
    0x07, 0xff, 0x45, 0,
    0xff, 0x07, 0x54, 0,
    0x07, 0x07, 0x55, 0,
    128
};

const unsigned char NinjaWalkR1[]={
    0xff, 0xff, 0x46, 0,
    0x07, 0xff, 0x47, 0,
    0xff, 0x07, 0x56, 0,
    0x07, 0x07, 0x57, 0,
    128
};

const unsigned char NinjaWalkR2[]={
    0xff, 0xff, 0x48, 0,
    0x07, 0xff, 0x49, 0,
    0xff, 0x07, 0x58, 0,
    0x07, 0x07, 0x59, 0,
    128
};

const unsigned char NinjaWalkR3[]={
    0xff, 0xff, 0x4A, 0,
    0x07, 0xff, 0x4B, 0,
    0xff, 0x07, 0x5A, 0,
    0x07, 0x07, 0x5B, 0,
    128
};

// Rolling sprite (20x20, single sprite)
const unsigned char NinjaRolling[]={
    0xff, 0xff, 0x60, 0,
    0x07, 0xff, 0x61, 0,
    0xff, 0x07, 0x70, 0,
    0x07, 0x07, 0x71, 0,
    128
};

// Masks (23x72, 8 sprites)
// These would need to be adapted to the NES 8x8 tile system
// Here's an example for the first mask
const unsigned char MaskSprite1[]={
    0xff, 0xff, 0x80, 1,
    0x07, 0xff, 0x81, 1,
    0xff, 0x07, 0x90, 1,
    0x07, 0x07, 0x91, 1,
    128
};

// Mouth sprites (7x12, 3 sprites)
const unsigned char MouthSprite1[]={
    0xff, 0xff, 0xA0, 2,
    128
};

const unsigned char MouthSprite2[]={
    0xff, 0xff, 0xA1, 2,
    128
};

const unsigned char MouthSprite3[]={
    0xff, 0xff, 0xA2, 2,
    128
};

// Animation sequences
enum {
    ANIM_WALK_L1, ANIM_WALK_L2, ANIM_WALK_L3,
    ANIM_WALK_R1, ANIM_WALK_R2, ANIM_WALK_R3,
    ANIM_ROLLING,
    ANIM_MASK1,
    ANIM_MOUTH1, ANIM_MOUTH2, ANIM_MOUTH3
};

const unsigned char * const ninja_anim_list[]={
    NinjaWalkL1, NinjaWalkL2, NinjaWalkL3,
    NinjaWalkR1, NinjaWalkR2, NinjaWalkR3,
    NinjaRolling,
    MaskSprite1,
    MouthSprite1, MouthSprite2, MouthSprite3
};