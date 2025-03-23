# NES Development Guidelines

## Build Commands
- `make` - Build the NES ROM
- `make clean` - Remove build artifacts
- `make debug` - Check compiler/assembler paths
- To run a specific project: `cd game_of_life && make`

## Code Style Guidelines
- Include order: <standard libs> followed by "../neslib/neslib.h" then "LIB/nesdoug.h" if needed
- Constants: ALL_CAPS with underscore separators
- Variables: snake_case for all variables
- Use uint8_t for most variables (memory-constrained environment)
- Comment logic blocks and use function docblocks to document purpose
- Limit functions to 25-50 lines when possible
- Initialize variables to prevent undefined behavior
- Organize structs at top of header files
- Keep functions small and focused on a single task
- Use 4 spaces for indentation
- Wrap at 80 characters when possible
- Use descriptive variable names
- Use bitmasks & bitshifts for efficient operations
- Add platform-specific optimizations with compiler pragmas
- Place zero page variables in appropriate segments with #pragma bss-name
- Use nesdoug.h utilities for VRAM buffer, collision, and scrolling operations

## Platform Notes
- Use neslib for hardware-dependent functions (ppu, input, sound)
- Use metatile system for efficiently rendering backgrounds
- Understand NMI and frame timing for smooth animation
- Remember 6502 performance constraints when designing game logic