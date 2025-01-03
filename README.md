# Jelly Simulator

https://github.com/user-attachments/assets/6be7c8a8-e1dc-4ff9-931d-c66ebc43b431

https://github.com/user-attachments/assets/769b8ef9-d1eb-44ff-addd-8cf4ba0aeb8e

## About project

This is the third project created during *"Physical simulations in a virtual environment"* course, specialty **CAD/CAM systems design** of degree in *Computer Science and Information Systems* at *Warsaw University of Technology*. 

The project is a simulation of 64 mass points arranged in a 4x4x4 cube (**jelly**) and connected to each other by springs. Jelly's corner points are connected to the corners of **the control frame** with special springs of zero rest length. User can freely move and rotate the control frame that the jelly will follow.

The jelly is inside **the collision frame** that he can't get out of. The control frame can be freely moved outside the collision frame. 

Normaly the jelly is rendered as **6 Bézier patches** based on control points which are outer jelly's points. Additionally, any user's model can be inserted inside the jelly. A model will be deformed using **free-form deformation (FFD)**: 

$$F(u, v, w) = \sum_{i}^3\sum_{j}^3\sum_{k}^3 P^{ijk} B_i^3(u)B_j^3(v)B_j^3(w),$$

where  $`(u, v, w) \in [0, 1]^3`$, $`P^{ijk}`$ - jelly points, $`B^3_i(t)`$ - bernstein polynomial.

The project has **4 example models** which can be found in `./resources/models` folder.

User model shading is computed based on this definition of FFD transofrmation.

## Navigation

<table>
    <tbody>
		<tr>
            <th colspan=2> Camera</th>
        </tr>
		<tr>
            <td><kbd>MMB</kbd> <i>hold</i></td>
            <td>camera rotation</td>
        </tr>
		<tr>
            <td><kbd>Shift</kbd> + <kbd>MMB</kbd> <i>hold</i></td>
            <td>camera movement</td>
        </tr>
		<tr>
            <td><kbd>MMB</kbd> <i>scroll</i></td>
            <td>camera zoom in/out</td>
        </tr>
		<tr>
            <th colspan=2> Collision Frame (if enabled)</th>
        </tr>
		<tr>
            <td><kbd>Ctrl</kbd> + <kbd>LMB</kbd></td>
            <td>move collision frame</td>
        </tr>
		<tr>
            <td><kbd>R</kbd></td>
            <td>rotate collision frame around camera's <code>front</code> vector. To accept rotation click <kbd>LMB</kbd></td>
        </tr>
		<tr>
            <td><kbd>R</kbd> + <kbd>R</kbd></td>
            <td>rotate collision frame around camera's <code>right</code> and <code>up</code> vectors. To accept rotation click <kbd>LMB</kbd></td>
        </tr>
		<tr>
            <th colspan=2> Jelly's points (if collision frame is disabled)</th>
        </tr>
		<tr>
            <td><kbd>Ctrl</kbd> + <kbd>LMB</kbd></td>
            <td>move selected point</td>
        </tr>
	</tbody>
</table>

######

## Compilation

The project is written in C++ (OpenGL) using CMake as a build system. It has two configuration CMake options: 

| Option | Description | 
| :---:         |     :---      |
| `PR3_EMBEDDED_SHADERS`   | If `ON` shader code will be embedded into a resulting app's binary. If `OFF` shader code will be read from `shaders` directory, so a binary has to be run from repository root directory.      |
| `PR3_SHOW_DEBUG_CONSOLE` | If `ON` - beside app's window - a debug console will be shown on app start up. If `OFF` only app's window will be shown.        |

### MS Windows

All subsequent commands should be called from Windows Power Shell.

```
git clone --recurse-submodules -j8 https://github.com/Tolisz/Simulation_PR3.git
cd Simulation_PR3
cmake -S . -B build -DPR3_EMBEDDED_SHADERS=ON -DPR3_SHOW_DEBUG_CONSOLE=OFF
cmake --build build --config Release
```

```
.\build\Release\SimulationPr3.exe
```

### Linux

All subsequent commands should be called from terminal. Compilation was checked using **g++** (version 11.4.0) and **clang++** (version 14.0.0) compilers.

```
$ git clone --recurse-submodules -j8 https://github.com/Tolisz/Simulation_PR3.git
$ cd Simulation_PR3
$ cmake -S . -B build -DPR3_EMBEDDED_SHADERS=ON -DPR3_SHOW_DEBUG_CONSOLE=OFF
$ cmake --build build --config Release -j 26
```

```
./build/SimulationPr3
```

## Used Libraries

Libraries `GLFW`, `GLM`, `Assimp`, `STB` and `ImGui` are used as submodules located in `externals` directory. `GLAD` is used as a generated file from [glad](https://glad.dav1d.de/) web page. 

| Library | Licence |
| :---:   | :---:   |
| [GLFW](https://github.com/glfw/glfw) | [Zlib](https://github.com/glfw/glfw?tab=Zlib-1-ov-file#readme) |
| [GLM](https://github.com/g-truc/glm) | [Modified MIT](https://github.com/g-truc/glm?tab=License-1-ov-file#readme)|
| [ImGui](https://github.com/ocornut/imgui) | [MIT](https://github.com/ocornut/imgui?tab=MIT-1-ov-file#readme)| 
| [GLAD](https://github.com/Dav1dde/glad) | [MIT](https://github.com/Dav1dde/glad?tab=License-1-ov-file#readme) | 
| [STB](https://github.com/nothings/stb) | [Modified MIT](https://github.com/nothings/stb?tab=License-1-ov-file#readme) | 
| [Assimp](https://github.com/assimp/assimp/) | [BSD](https://github.com/assimp/assimp/?tab=License-1-ov-file#readme) | 
