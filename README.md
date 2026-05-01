<a id="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h1 align="center">Pen and Ink Non-Photorealistic Renderer</h1>
</div>


<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>


<!-- ABOUT THE PROJECT -->
## About The Project

This project is a 3D non-photorealistic renderer that generates stylized pen and ink drawing of various models. 
It uses OpenGL for rendering and GLSL for shader programming to create a hand-drawn effect. 
The renderer supports various shading techniques, including hatching and cross-hatching, to simulate the look of traditional pen and ink drawings.

### Built With
- C++ 17
- OpenGL (GLFW, GLAD)
- GLSL
- ImGui for GUI implementation

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

Follow these instructions to build and run the project locally. 

### Prerequisites
You need the following installed:
* CMake (>= 3.15)
* C++ 17 compiler
* OpenGL drivers
* GLFW and GLAD
* GLM
* ImGui

### Installation
1. Clone the repository
  ```sh
  git clone https://github.com/bhavyaven/Pen-and-Ink-NPR.git
  ```
2. Navigate to the project directory
  ```sh
  cd Pen-and-Ink-NPR
  ```
3. Create `out/build` directory and run exectutable from build directory
  ```sh
  mkdir out/build
  cd out/build
  cmake ../..
  cmake --build .
  ```
4. Navigate to ```out\build\PenInkRenderer.sln``
5. Click "Start Without Debugging"
6. Navigate 3D rendering using GUI in the window.



<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage
* Navigate the different model options using the GUI in the window.
* Adjust the parameters of the model using the sliders in the GUI.
* The renderer will update in real-time to reflect the changes made to the model parameters.

### Example:


### Result:
![alt text](<Screenshot 2026-05-01 155125.png>)
![alt text](<Screenshot 2026-05-01 155152.png>)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap
- Geometry Pass
- Edge Detection Pass
- Hatching Pass
- Composition Pass (final image)
<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTACT -->
## Contact

Bhavya Venkataraghavan - bhavya.v04@gmail.com   
LinkedIn: http://www.linkedin.com/in/bhavya-venkat    
GitHub: https://github.com/bhavyaven   

Project Link: [https://github.com/bhavyaven/BuildingGenerator](https://github.com/bhavyaven/BuildingGenerator)


<p align="right">(<a href="#readme-top">back to top</a>)</p>