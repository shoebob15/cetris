# Cetris
### Yet another Tetris clone witten in C utilizing the [Raylib](https://github.com/raysan5/raylib) library

# Screenshots
![Screenshot 2024-08-01 at 12 11 50 AM](https://github.com/user-attachments/assets/27341919-5276-4f67-87a9-e23db1b02fc2)

# Building
1. Clone the repository

```$ git clone https://github.com/shoebob15/cetris```

2. Move into the Cetris directory
```$ cd cetris```

4. Generate CMake build files

```$ cmake -S . -B build```

4. Build the project

```$ cmake --build build```

5. Run the output file

```$ ./build/cetris```

Prebuilt binaries for most platforms are in the Github releases tab (TODO)

# Controls
↑ -> Rotate clockwise

← -> Move left

→ -> Move right

↓ -> Slow drop

space -> Fast drop (will implement when there is a drop preview)

z -> Rotate counter-clockwise

enter -> swap the tetromino


# TODO
- [ ] Finish controls
- [ ] High score
- [ ] Pause menu
- [x] Levels and speed increase
- [ ] Next drop preview
- [ ] Different game modes
- [ ] Package assets
