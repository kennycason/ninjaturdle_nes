#include "sprite_system.h"

SpriteSystem::SpriteSystem()
    : ninja_left_({
        SpriteTile{-4, -4, 0x02, 1},
        SpriteTile{7, -4, 0x03, 1},
        SpriteTile{-4, 4, 0x12, 1},
        SpriteTile{7, 4, 0x13, 1}
    }, 4)
    , ninja_right_({
        SpriteTile{-4, -4, 0x00, 1},
        SpriteTile{7, -4, 0x01, 1},
        SpriteTile{-4, 4, 0x10, 1},
        SpriteTile{7, 4, 0x11, 1}
    }, 4)
    , coin_({
        SpriteTile{-1, -1, 0x20, 1}
    }, 1)
    , corn_selected_({
        SpriteTile{-1, -1, 0x20, 2}
    }, 1)
    , big_coin_({
        SpriteTile{-1, -1, 0x21, 0},
        SpriteTile{-1, 7, 0x31, 1},
        SpriteTile{7, -1, 0x22, 2},
        SpriteTile{7, 7, 0x32, 3}
    }, 4)
    , coin_hud_({
        SpriteTile{0, 0, 0x20, 1}
    }, 1)
    , wasp_left_({
        SpriteTile{-1, -1, 0x08, 2},
        SpriteTile{7, -1, 0x09, 2},
        SpriteTile{-1, 7, 0x18, 2},
        SpriteTile{7, 7, 0x19, 2}
    }, 4)
    , wasp_right_({
        SpriteTile{-1, -1, 0x0A, 2},
        SpriteTile{7, -1, 0x0B, 2},
        SpriteTile{-1, 7, 0x1A, 2},
        SpriteTile{7, 7, 0x1B, 2}
    }, 4)
    , bounce1_({
        SpriteTile{-1, -1, 0x06, 3},
        SpriteTile{7, -1, 0x07, 3},
        SpriteTile{-1, 7, 0x16, 3},
        SpriteTile{7, 7, 0x17, 3}
    }, 4)
    , bounce2_({
        SpriteTile{-1, -1, 0x04, 3},
        SpriteTile{7, -1, 0x05, 3},
        SpriteTile{-1, 7, 0x14, 3},
        SpriteTile{7, 7, 0x15, 3}
    }, 4)
    , boss1_left_({
        // Top-left 16x16 block
        SpriteTile{-1, -1, 0x2C, 1},
        SpriteTile{7, -1, 0x2D, 1},
        SpriteTile{-1, 7, 0x3C, 1},
        SpriteTile{7, 7, 0x3D, 1},

        // Top-right 16x16 block
        SpriteTile{15, -1, 0x2E, 1},
        SpriteTile{23, -1, 0x2F, 1},
        SpriteTile{15, 7, 0x3E, 1},
        SpriteTile{23, 7, 0x3F, 1},

        // Bottom-left 16x16 block
        SpriteTile{-1, 15, 0x4C, 1},
        SpriteTile{7, 15, 0x4D, 1},
        SpriteTile{-1, 23, 0x5C, 1},
        SpriteTile{7, 23, 0x5D, 1},

        // Bottom-right 16x16 block
        SpriteTile{15, 15, 0x4E, 1},
        SpriteTile{23, 15, 0x4F, 1},
        SpriteTile{15, 23, 0x5E, 1},
        SpriteTile{23, 23, 0x5F, 1}
    }, 16)
    , boss1_right_({
        // Top-left 16x16 block
        SpriteTile{-1, -1, 0x6C, 1},
        SpriteTile{7, -1, 0x6D, 1},
        SpriteTile{-1, 7, 0x7C, 1},
        SpriteTile{7, 7, 0x7D, 1},

        // Top-right 16x16 block
        SpriteTile{15, -1, 0x6E, 1},
        SpriteTile{23, -1, 0x6F, 1},
        SpriteTile{15, 7, 0x7E, 1},
        SpriteTile{23, 7, 0x7F, 1},

        // Bottom-left 16x16 block
        SpriteTile{-1, 15, 0x8C, 1},
        SpriteTile{7, 15, 0x8D, 1},
        SpriteTile{-1, 23, 0x9C, 1},
        SpriteTile{7, 23, 0x9D, 1},

        // Bottom-right 16x16 block
        SpriteTile{15, 15, 0x8E, 1},
        SpriteTile{23, 15, 0x8F, 1},
        SpriteTile{15, 23, 0x9E, 1},
        SpriteTile{23, 23, 0x9F, 1}
    }, 16)
    , mother_worm_left_({
        // Top row
        SpriteTile{0, 0, 0xAA, 1},
        SpriteTile{8, 0, 0xAB, 1},
        SpriteTile{16, 0, 0xAC, 1},
        SpriteTile{24, 0, 0xAD, 1},
        SpriteTile{32, 0, 0xAE, 1},
        SpriteTile{40, 0, 0xAF, 1},

        // Middle row
        SpriteTile{0, 8, 0xBA, 1},
        SpriteTile{8, 8, 0xBB, 1},
        SpriteTile{16, 8, 0xBC, 1},
        SpriteTile{24, 8, 0xBD, 1},
        SpriteTile{32, 8, 0xBE, 1},
        SpriteTile{40, 8, 0xBF, 1},

        // Bottom row
        SpriteTile{0, 16, 0xCA, 1},
        SpriteTile{8, 16, 0xCB, 1},
        SpriteTile{16, 16, 0xCC, 1},
        SpriteTile{24, 16, 0xCD, 1},
        SpriteTile{32, 16, 0xCE, 1},
        SpriteTile{40, 16, 0xCF, 1}
    }, 18)
    , worm_left1_({
        SpriteTile{-1, -1, 0x26, 3},
        SpriteTile{7, -1, 0x27, 3},
        SpriteTile{-1, 7, 0x36, 3},
        SpriteTile{7, 7, 0x37, 3}
    }, 4)
    , worm_left2_({
        SpriteTile{-1, -1, 0x28, 3},
        SpriteTile{7, -1, 0x29, 3},
        SpriteTile{-1, 7, 0x38, 3},
        SpriteTile{7, 7, 0x39, 3}
    }, 4)
    , worm_right1_({
        SpriteTile{-1, -1, 0x46, 3},
        SpriteTile{7, -1, 0x47, 3},
        SpriteTile{-1, 7, 0x56, 3},
        SpriteTile{7, 7, 0x57, 3}
    }, 4)
    , worm_right2_({
        SpriteTile{-1, -1, 0x48, 3},
        SpriteTile{7, -1, 0x49, 3},
        SpriteTile{-1, 7, 0x58, 3},
        SpriteTile{7, 7, 0x59, 3}
    }, 4)
    , turd_({
        SpriteTile{0, 0, 0x10, 1}
    }, 1)
    , turd_life_({
        SpriteTile{0, 0, 0x30, 1}
    }, 1) {
} 