# Cetris
### Tetris clone witten in C utilizing the [Raylib](https://github.com/raysan5/raylib) library

# Screenshots
![Screenshot 2024-08-02 at 8 28 47 PM](https://github.com/user-attachments/assets/c4570293-c76b-4f0b-9adb-57b7ffbae843)


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

space -> Fast drop

z -> Rotate counter-clockwise

enter -> swap the tetromino


# TODO
- [x] Finish controls
- [ ] High score
- [ ] Pause menu
- [x] Levels and speed increase
- [x] Next drop preview
- [ ] Different game modes
- [ ] Package assets
