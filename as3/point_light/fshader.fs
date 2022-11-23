#version 330 core

struct PointLight {
    vec3 position;
    
    // Properties to calculate attenuation
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureT;
uniform PointLight light;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos);

void main() {
    vec3 norm = normalize(Normal);
    vec3 result = CalcPointLight(light, norm, FragPos);
    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    // Light direction
    vec3 lightDir = normalize(light.position - fragPos);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(textureT, TexCoord));
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);    
    vec3 diffuse = light.diffuse * vec3(texture(textureT, TexCoord)) * diff;
    diffuse *= attenuation;

    return (ambient + diffuse);
}
