filename = "test.input"

scene_data = """
640 480
2.0
1.0
20.0 20.0 10.0 1000.0
4
0x1188EE 0xDD2266 0xDD7700 0x11CC66
0
3
2.0 0.0 -5.0 2.0 2
-2.0 0.0 -5.0 2.0 3
0.0 -102.0 -5.0 100.0 1
"""

if __name__ == "__main__":
    with open(filename, "w") as f:
        f.write(scene_data.strip() + "\n")
        f.write("0\n") # Zero cubes
    print(f"Successfully generated {filename} (Scene 2)")