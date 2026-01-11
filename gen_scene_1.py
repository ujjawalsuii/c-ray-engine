filename = "test.input"

# Raw scene data from your request
scene_data = """
640 480
2.0
5.0
0.0 20.0 0.0 500.0
5
0x97C4EB 0x88CC66 0xEB6E0A 0xEBBE0A 0xEB3EDA
0
4
0.0 -41.0 -18.0 40.0 1
0.4 1.0 -10.0 0.5 3
0.6 -0.3 -10.2 0.3 4
-0.4 -0.1 -10.5 0.7 2
"""

if __name__ == "__main__":
    with open(filename, "w") as f:
        f.write(scene_data.strip() + "\n")
        # Append "0" for the cube count (needed for the new engine)
        f.write("0\n")
    print(f"Successfully generated {filename} (Scene 1)")