# OpenGL & GLSL: from the fixed pipeline to compute shaders

**English** · [Español](#español)

The coursework exercises from *GPU Programming* (Programación de Tarjetas Gráficas, BSc Multimedia Engineering, Universitat de València, 2025-26), in order. They go from immediate-mode OpenGL to tessellation, geometry and compute shaders. The final project of the course, a deferred **SSAO** renderer, is in its own repo: **[ssao-opengl](https://github.com/reylui-sys/ssao-opengl)**.

| # | Folder | Topic | Key techniques |
|---|---|---|---|
| 1.0 | `Ejercicio_1_0` | First steps with GLUT | Window, input callbacks, immediate mode |
| 1.1 | `Ejercicio_1_1` | Transformations | Matrix stack (`push/pop`), orthographic projection, polygon modes |
| 1.2 | `Ejercicio_1_2` | Fixed-pipeline lighting & textures | `glLight`/`glMaterial`, PNG textures with *lodepng* |
| 1.3 | `Ejercicio_1_3` | Geometry submission | Vertex arrays → VBO → VAO |
| 2.1 | `Ejercicio_2_1` | First GLSL shaders | Attributes and uniforms, `discard`-based effects |
| 3.1 | `Ejercicio_3_1` | Environment mapping | Cube maps, reflection, refraction, Fresnel |
| 3.2 | `Ejercicio_3_2` | Per-fragment lighting | Phong, spotlights |
| 3.3 | `Ejercicio_3_3` | Surface detail | Normal mapping, parallax mapping, gloss maps |
| 3.4 | `Ejercicio_3_4` | Loading meshes | OBJ loading (dragon model) with environment reflections |
| 3.5 | `Ejercicio_3_5` | Shadows | Shadow mapping with PCF |
| 4.1 | `Ejercicio_4_1` | Tessellation | TCS/TES: fixed, view-dependent (global and per-patch) and crack-free tessellation levels |
| 4.2 | `Ejercicio_4_2` | Geometry shaders | Billboards and sphere *impostors* |
| 4.3 | `Ejercicio_4_3` | GPU particles | **Compute shader** particle system, rendered as billboards from a geometry shader |

Each folder is a standalone project: `src/` (C++), `shaders/` (GLSL) and, where needed, `texturas/` and `modelos_obj/`.

### Build
Dependencies: GLEW, freeGLUT, GLM, C++17. The exercises were built on Windows with **MSYS2 UCRT64** and the VS Code task in each `.vscode/tasks.json` (`g++ src/*.cpp -lfreeglut -lglew32 -lopengl32 -lglu32`). Run each executable from its own folder so it can find `shaders/` and `texturas/`.

> Textures larger than 2048 px were downscaled to 2048 px to keep the repository light.

### Credits
Exercise statements and base code are from the course. Teapot/torus helpers derive from the *OpenGL 4 Shading Language Cookbook*, PNG loading uses [lodepng](https://github.com/lvandeve/lodepng) (zlib licence) and OBJ loading uses [OBJ-Loader](https://github.com/Bly7/OBJ-Loader) (MIT). Academic work — don't hand it in as your own.

---

## Español

Los ejercicios de la asignatura *Programación de Tarjetas Gráficas* (Ingeniería Multimedia, UV, 2025-26), en orden. Van desde OpenGL en modo inmediato hasta los shaders de teselación, de geometría y de cómputo. El proyecto final, un render diferido con **SSAO**, tiene su propio repo: **[ssao-opengl](https://github.com/reylui-sys/ssao-opengl)**.

- **Tema 1**: GLUT, transformaciones, iluminación y texturas del pipeline fijo, y el paso de vertex arrays a VBO y VAO.
- **Tema 2**: primeros shaders GLSL (atributos, uniforms, `discard`).
- **Tema 3**: cube maps (reflexión, refracción, Fresnel), Phong y focos, normal y parallax mapping, carga de OBJ y shadow mapping con PCF.
- **Tema 4**: teselación (TCS/TES, fija y dependiente de la distancia, sin grietas), geometry shaders (billboards e impostores) y un sistema de partículas con **compute shader**.

Cada carpeta es un proyecto independiente. Se compilaba con MSYS2 UCRT64 y la tarea de VS Code incluida. Las texturas de más de 2048 px se han reducido a 2048 px para aligerar el repositorio. Los enunciados y el código base son de la asignatura, así que no lo entregues como trabajo propio.
