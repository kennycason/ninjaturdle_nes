#pragma once

#include <cstdint>
#include <array>

// Represents a single sprite tile with its properties
struct SpriteTile {
    int8_t x_offset;
    int8_t y_offset;
    uint8_t tile_number;
    uint8_t attributes;
};

// Represents a complete sprite composed of multiple tiles
class Sprite {
public:
    Sprite(const std::array<SpriteTile, 16>& tiles, size_t tile_count)
        : tiles_(tiles), tile_count_(tile_count) {}

    const SpriteTile* get_tiles() const { return tiles_.data(); }
    size_t get_tile_count() const { return tile_count_; }

private:
    std::array<SpriteTile, 16> tiles_;
    size_t tile_count_;
};

// Manages all sprites in the game
class SpriteSystem {
public:
    static SpriteSystem& get_instance() {
        static SpriteSystem instance;
        return instance;
    }

    // Player sprites
    const Sprite& get_ninja_left() const { return ninja_left_; }
    const Sprite& get_ninja_right() const { return ninja_right_; }

    // Collectible sprites
    const Sprite& get_coin() const { return coin_; }
    const Sprite& get_corn_selected() const { return corn_selected_; }
    const Sprite& get_big_coin() const { return big_coin_; }
    const Sprite& get_coin_hud() const { return coin_hud_; }

    // Enemy sprites
    const Sprite& get_wasp_left() const { return wasp_left_; }
    const Sprite& get_wasp_right() const { return wasp_right_; }
    const Sprite& get_bounce1() const { return bounce1_; }
    const Sprite& get_bounce2() const { return bounce2_; }
    const Sprite& get_worm_left1() const { return worm_left1_; }
    const Sprite& get_worm_left2() const { return worm_left2_; }
    const Sprite& get_worm_right1() const { return worm_right1_; }
    const Sprite& get_worm_right2() const { return worm_right2_; }

    // Boss sprites
    const Sprite& get_boss1_left() const { return boss1_left_; }
    const Sprite& get_boss1_right() const { return boss1_right_; }
    const Sprite& get_mother_worm_left() const { return mother_worm_left_; }

    // Projectile sprites
    const Sprite& get_turd() const { return turd_; }
    const Sprite& get_turd_life() const { return turd_life_; }

private:
    SpriteSystem(); // Private constructor for singleton

    // Player sprites
    Sprite ninja_left_;
    Sprite ninja_right_;

    // Collectible sprites
    Sprite coin_;
    Sprite corn_selected_;
    Sprite big_coin_;
    Sprite coin_hud_;

    // Enemy sprites
    Sprite wasp_left_;
    Sprite wasp_right_;
    Sprite bounce1_;
    Sprite bounce2_;
    Sprite worm_left1_;
    Sprite worm_left2_;
    Sprite worm_right1_;
    Sprite worm_right2_;

    // Boss sprites
    Sprite boss1_left_;
    Sprite boss1_right_;
    Sprite mother_worm_left_;

    // Projectile sprites
    Sprite turd_;
    Sprite turd_life_;
}; 