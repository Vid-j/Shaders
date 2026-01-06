#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));
uniform vec3 viewPos = vec3(0.0, 0.0, 5.0);
uniform vec3 objectColor = vec3(0.2, 0.6, 1.0);

void main() {
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * objectColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * objectColor;

    // Specular
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
