# Wii Asset Importer – Targeting Platforms Project

This project demonstrates the process of importing and rendering custom 3D models on the Nintendo Wii using the DevkitPro toolchain and the GX graphics library. It showcases a custom asset pipeline from 3ds Max to the Wii, visualized as spinning 3D shapes.

---

## Project Summary

- Uses the GX graphics library (DevkitPro/libogc) to manually render 3D objects
- Includes a MaxScript exporter to convert 3D models to a format readable by the Wii
- Renders multiple rotating triangles and imported models
- Designed to run in the Dolphin emulator or on real hardware via `.dol` executable

---

## How to Run

1. Download and install the [Dolphin Emulator](https://dolphin-emu.org/).
2. Open the file: in Dolphin.
3. You should see a spinning 3D shape in the center of the screen.
4. Use the controls below to move the shape around.

---

## Controls

### Required Setup (Dolphin Emulator)

To control the object, you need to map the Wii Remote D-Pad to your keyboard:

1. Open Dolphin Emulator.
2. Go to: Controllers → Wii Remote 1 → Configure
3. Under the D-Pad section, assign:
- Up → `W`
- Down → `S`
- Left → `A`
- Right → `D`

### In-Game Controls

- `W` – Move object up  
- `A` – Move object left  
- `S` – Move object down  
- `D` – Move object right  

---

## Development Details

- Written in C using DevkitPro and `libogc`
- 3D models are exported via a custom MaxScript, generating a `.h` header file
- Manual rendering of vertices using `GX_Begin` (similar to OpenGL immediate mode)
- Basic transformation and animation logic included
- Documentation generated with Doxygen

---

## Repository Structure
/source → C source files using libogc & GX
Targetting Triangles.dol → Compiled Wii executable


---

## Documentation

The project is documented using Doxygen. It covers:
- The core 3D rendering pipeline using the Wii GX API  
- Setup and configuration of the Wii video and input systems (GX and WPAD)  
- Object rendering logic, including vertex and face data for shapes (cube, pyramid, triangle)  
- User interaction via Wii Remote (object movement, input handling)  
- Function responsibilities, parameters, and return values  
- Global variables, constants, and data arrays (e.g., colours, geometry)  
- Coding conventions and architectural structure of the project
- Header files containing model data
- MaxScript logic for exporting 3D models

Open the `documentation/html/index.html` in the documentation zipped folder in a browser to view full documentation.

---

## Tools Used

- [DevkitPro](https://devkitpro.org/)
- [libogc (GX)](https://devkitpro.org/wiki/libogc/GX)
- [Autodesk 3ds Max](https://www.autodesk.com/products/3ds-max/)
- [Dolphin Emulator](https://dolphin-emu.org/)
- Doxygen for documentation

---

## Future Improvements

- Implement motion controls using the Wii Remote
- Expand model support and improve performance on complex geometry
- Add interactivity (e.g. object reaction to D-Pad or motion gestures)
- Contribute to community tools by releasing an open-source `DrawIndexed` alternative

---

## Links

- [Project Repository](https://github.com/LeviHerring/TargetingPlatformsWii)
- [DevkitPro Documentation](https://devkitpro.org/)
- [GX Graphics Library](https://libogc.devkitpro.org/gx_8h.html)

---

## Author

Levi Herring – [GitHub](https://github.com/LeviHerring)

---

## License

This project is open-source under the MIT License.

 


