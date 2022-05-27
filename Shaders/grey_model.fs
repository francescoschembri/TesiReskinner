#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 light_dir;

void main()
{    
    FragColor = vec4(0.48, 0.48, 0.48, 1.0);
}

