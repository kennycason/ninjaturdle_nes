def print_indices():
    print("x,y -> index")
    for y in range(0, 0x100, 0x20):  # 0 to 0xe0 step 0x20
        for x in range(0, 0x100, 0x20):  # 0 to 0xe0 step 0x20
            # Current formula
            index = (y >> 4) * 16 + (x >> 4)
            print(f"0x{x:02x},0x{y:02x} -> {index}")
        print("---")  # Row separator

print("Current formula ((y & 0xe0) >> 5) * 8 + ((x & 0xe0) >> 5):")
print_indices()

# Let's also print what we get with the original working first row formula
print("\nOriginal formula (y & 0xf0) + (x >> 4):")
def print_indices_original():
    print("x,y -> index")
    for y in range(0, 0x100, 0x20):
        for x in range(0, 0x100, 0x20):
            index = (y & 0xf0) + (x >> 4)
            print(f"0x{x:02x},0x{y:02x} -> {index}")
        print("---")