import random

#STILL A WORK IN PROGRESS (11TH JAN, ITS EXPECTED TO BE UPDATED BY FEB AS COURSES AND OTHER STUFF IS KEEPING ME OCCUPIED)

FILENAME = "test.input"
GRID_SIZE = 12    
BLOCK_SIZE = 4.0  
ROAD_WIDTH = 1.5  

# FINAL PALETTE
COLORS = [
    "0x050510", 
    "0x050515", # Water
    "0xFF0080", # Hot Pink
    "0x00FFFF", # Cyan
    "0xFF9900", # Gold
]

def generate_final_city():
    cubes = []
    spheres = []

    # Water Floor
    spheres.append("0.0 -1003.0 -30.0 1000.0 1")

    start_offset = -((GRID_SIZE * (BLOCK_SIZE + ROAD_WIDTH)) / 2)

    for row in range(GRID_SIZE):
        if row % 4 == 0: continue 

        for col in range(GRID_SIZE):
            if col % 4 == 0: continue

            x_base = start_offset + col * (BLOCK_SIZE + ROAD_WIDTH)
            z_base = -10.0 - row * (BLOCK_SIZE + ROAD_WIDTH)

            color_idx = random.choice([2, 3, 4]) 
            
            # FIX: Massive Height Variation
            # Some buildings are small (5), some are TITANS (24)
            base_height = random.randint(5, 24) + (row * 0.2)
            y_min = -3.0

            building_type = random.randint(0, 3) # Added Type 3

            if building_type == 0: # Monolith
                w = BLOCK_SIZE * 0.9
                h = base_height * 1.5
                cubes.append(f"{x_base:.1f} {y_min:.1f} {z_base:.1f} {x_base+w:.1f} {y_min+h:.1f} {z_base+w:.1f} {color_idx}")
            
            elif building_type == 1: # Twin Towers
                w = BLOCK_SIZE * 0.4
                h = base_height * 1.8
                cubes.append(f"{x_base:.1f} {y_min:.1f} {z_base:.1f} {x_base+w:.1f} {y_min+h:.1f} {z_base+w:.1f} {color_idx}")
                cubes.append(f"{x_base+w+0.5:.1f} {y_min:.1f} {z_base+0.5:.1f} {x_base+w+0.5+w:.1f} {y_min+h*0.9:.1f} {z_base+0.5+w:.1f} {color_idx}")
            
            elif building_type == 2: # Step Tower
                current_y = y_min
                current_w = BLOCK_SIZE
                h = base_height * 0.5
                for stack in range(4):
                    cubes.append(f"{x_base:.1f} {current_y:.1f} {z_base:.1f} {x_base+current_w:.1f} {current_y+h:.1f} {z_base+current_w:.1f} {color_idx}")
                    current_y += h
                    current_w *= 0.8
                    x_base += 0.4
                    z_base += 0.4
            
            elif building_type == 3: # Needle (Super Tall, Thin)
                w = BLOCK_SIZE * 0.6
                h = base_height * 2.0 # Huge
                offset = (BLOCK_SIZE - w) / 2
                cubes.append(f"{x_base+offset:.1f} {y_min:.1f} {z_base+offset:.1f} {x_base+offset+w:.1f} {y_min+h:.1f} {z_base+offset+w:.1f} {color_idx}")


    with open(FILENAME, "w") as f:
        f.write("640 480\n2.0\n2.0\n") 
        f.write("30.0 60.0 10.0 4000.0\n") 
        f.write(f"{len(COLORS)}\n")
        for c in COLORS: f.write(f"{c}\n")
        f.write("0\n") 
        f.write(f"{len(spheres)}\n")
        for s in spheres: f.write(s + "\n")
        f.write(f"{len(cubes)}\n")
        for c in cubes: f.write(c + "\n")

    print(f"Generated {FILENAME} with HERO TOWERS.")

if __name__ == "__main__":
    generate_final_city()