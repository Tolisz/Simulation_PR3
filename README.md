# Jelly Simulator

https://github.com/user-attachments/assets/6be7c8a8-e1dc-4ff9-931d-c66ebc43b431

https://github.com/user-attachments/assets/769b8ef9-d1eb-44ff-addd-8cf4ba0aeb8e

## About project

This is the third project created during *"Physical simulations in a virtual environment"* course, specialty **CAD/CAM systems design** of degree in *Computer Science and Information Systems* at *Warsaw University of Technology*. 

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
            <th colspan=2> Bezier Cube Points (if collision frame is disabled)</th>
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

### Linux

### macOS

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
