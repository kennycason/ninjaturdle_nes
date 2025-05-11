# Ninja Turdle Code Cleanup Plan

## Priority 1: Low-Risk, High-Impact Changes

### Header Organization (`ninjaturdle.h`)
- [ ] Group related constants and defines together
- [ ] Convert direction constants to enum:
  ```c
  typedef enum {
      DIR_LEFT = 0,
      DIR_RIGHT = 1
  } Direction;
  ```
- [ ] Convert game modes to enum:
  ```c
  typedef enum {
      MODE_TITLE,
      MODE_GAME,
      MODE_PAUSE,
      MODE_SWITCH,
      MODE_END,
      MODE_GAME_OVER,
      MODE_RESET
  } GameMode;
  ```
- [ ] Group related variables into structs (e.g., BossData, PlayerData)
- [ ] Add descriptive comments for each struct and enum

### Magic Numbers (`ninjaturdle.h`, `ninjaturdle.c`)
- [ ] Replace common magic numbers with named constants:
  ```c
  #define SCREEN_WIDTH 256
  #define SCREEN_HEIGHT 240
  #define SPRITE_SIZE 8
  #define TITLE_FADE_SPEED 8
  ```
- [ ] Group related constants together (e.g., all sprite sizes, all timing values)

### Dead Code Removal
- [ ] Remove commented-out code in `sprites.h`:
  - Enemy animation list
  - Unused sprite definitions
- [ ] Clean up debug code (wrap in `#ifdef DEBUG` if needed)

## Priority 2: Moderate Changes

### Function Organization (`ninjaturdle.c`)
- [ ] Mark internal functions as `static`
- [ ] Group related functions together (e.g., all collision functions)
- [ ] Add brief function documentation headers
- [ ] Split large functions into smaller, focused ones

### Variable Scope & Naming
- [ ] Rename ambiguous variables:
  - `temp1` → `frameCount`
  - `temp2` → `collisionResult`
  - `temp3` → `entityIndex`
  - etc.
- [ ] Make helper variables `static` where possible
- [ ] Group related global variables into appropriate structs

### Include File Cleanup
- [ ] Audit and remove unused includes
- [ ] Order includes consistently:
  1. Standard library headers
  2. NES-specific headers
  3. Project-specific headers

## Priority 3: Structural Improvements

### Data Organization
- [ ] Move level data into separate files
- [ ] Create dedicated headers for:
  - Collision system
  - Enemy system
  - Player system
- [ ] Organize sprite data more systematically

### Build System
- [ ] Add debug build configuration
- [ ] Improve makefile organization
- [ ] Add build documentation

## Guidelines for Implementation

1. **Make Atomic Changes**
   - Each change should be small and self-contained
   - Test after each modification
   - Document any unexpected behavior

2. **Maintain Compatibility**
   - Preserve existing function signatures
   - Keep binary compatibility where possible
   - Document any breaking changes

3. **Documentation**
   - Add/update comments for non-obvious code
   - Document hardware-specific behavior
   - Add section headers for major code blocks

## Testing Strategy

1. **For Each Change**
   - Compile and verify no new warnings
   - Test affected functionality
   - Verify no performance regression

2. **Final Verification**
   - Full playthrough testing
   - Performance testing
   - Memory usage verification

## Notes

- NES-specific optimizations should be preserved
- Comments should explain "why" not "what"
- Keep 8-bit system limitations in mind 