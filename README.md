# Raytracing project 
A C++ render engine to produce photo-realistic renders of scenes, using OpenGL Mathematics (GLM).

The main components are :
- Scene representation and parsing (using a scene graph)
- Ray intersection (using implicit 3D equations of shapes)
- Lighting computation (supports 3 types of lights, textures...)
- Antialiasing (leveraging signal theory to improve quality of renders)

Examples of rendered images are:

<img width="1024" height="768" alt="reflections_complex" src="https://github.com/user-attachments/assets/f9b519b6-1bd0-4f4f-8dbd-2e30e0781ea4" />

<img width="1024" height="768" alt="spot_light_2" src="https://github.com/user-attachments/assets/e258468d-f9de-4e7f-8325-34255b85d72b" />

<img width="1024" height="768" alt="shadow_test" src="https://github.com/user-attachments/assets/790d3d69-5cbf-42cf-b249-c63a2bf49c25" />

