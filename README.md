🎨 GLSL Shader Collection
A repository of custom GLSL shaders focusing on procedural generation and environmental effects. This collection demonstrates techniques in raymarching, vertex manipulation, and surface physics.

🌊 Current Shaders
1. Marching Cubes
An implementation of the Marching Cubes algorithm used to generate procedural 3D meshes from scalar fields.

Key Features: Iso-surface extraction, smooth blending between metaballs, and real-time mesh generation.

Best For: Procedural terrain, organic blobs, or liquid simulations.

2. Water Shader
A dynamic water surface shader designed for realistic or stylized aquatic environments.

Key Features: Gerstner wave displacement, depth-based transparency (shallows vs. deep), and procedural foam generation.

Best For: Lakes, oceans, and stylized water planes.

🛠️ Getting Started
Prerequisites
To run these shaders, you will need a GLSL-compatible environment. I recommend using:

ShaderToy (for web-based testing)

VS Code with the GLSL Literal or Shader Languages Support extensions.

Custom Engine: Compatible with any engine supporting GLSL (Godot, Three.js, etc.).

How to Use
Navigate to the specific folder (e.g., /Water).

Open the .frag or .vert files.

Copy the code into your shader editor or link it to your material system.

⚙️ Technical Specs
Language: GLSL

Algorithm Focus: Marching Cubes, Procedural Noise, Trigonometric Waveforms.

Optimization: High-performance code designed to run at 60fps on modern GPUs.
