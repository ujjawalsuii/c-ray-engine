import random

FILENAME = "test.input"
GRID_SIZE = 5  
SPACING = 3.0

# The original simple palette
# 0=Black(Sky), 1=DarkBlue(Water), 2=NeonPink, 3=NeonCyan, 4=NeonLime
COLORS = ["0x000000", "0x000033", "0xFF00FF", "0x00FFFF", "0x32CD32"]

def generate_water_city():
    cubes = []
    spheres = []

    # 1. THE WATER FLOOR
    spheres.append("0.0 -1002.0 -20.0 1000.0 1")

    # 2. NEON TOWERS
    start_x = -(GRID_SIZE * SPACING) / 2
    for row in range(GRID_SIZE):
        for col in range(GRID_SIZE):
            x = start_x + (col * SPACING)
            z = -12.0 - (row * SPACING)
            
            # Random height
            height = random.randint(2, 6)
            color_idx = random.randint(2, 4) 
            
            # Stack cubes
            for h in range(height):
                y_min = -2.0 + (h * 1.0)
                y_max = y_min + 1.0
                cubes.append(f"{x:.1f} {y_min:.1f} {z:.1f} {x+0.8:.1f} {y_max:.1f} {z+0.8:.1f} {color_idx}")

    # 3. THE MOON
    spheres.append("5.0 8.0 -25.0 2.0 3") 

    with open(FILENAME, "w") as f:
        # Camera settings from the old scene
        f.write("640 480\n2.0\n2.5\n") 
        f.write("0.0 20.0 0.0 1500.0\n") 
        
        f.write(f"{len(COLORS)}\n")
        for c in COLORS: f.write(f"{c}\n")
        f.write("0\n") 
        
        f.write(f"{len(spheres)}\n")
        for s in spheres: f.write(s + "\n")
        f.write(f"{len(cubes)}\n")
        for c in cubes: f.write(c + "\n")

    print(f"Generated {FILENAME} (Original Layout)")

if __name__ == "__main__":
    generate_water_city()