# NES DEv


## Build cc65

```bash
cd tools/cc65/
make
```

## Build & Run Ninja Turdle

```bash
cd ninjaturdle/
make
fceux ninjaturdle.nes
```

## Reginerating NES .CHR files

```bash
../tools/png2chr ninjaturdle.png
```

This will generated a `ninjaturdle.chr` file.