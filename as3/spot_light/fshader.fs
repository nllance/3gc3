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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;      
};

// Number of spot lights
#define NR_SPOT_LIGHTS 3

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureT;
uniform SpotLight lights[NR_SPOT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);

void main() {
    vec3 norm = normalize(Normal);
    vec3 result = vec3(0.0, 0.0, 0.0);
    // Add all lights
    for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
        result += CalcSpotLight(lights[i], norm, FragPos);
    }
    FragColor = vec4(result, 1.0);
}

// Calculates the colour when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos) {
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

// Calculate the colour when using a spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos) {
    // Light direction
    vec3 lightDir = normalize(light.position - fragPos);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(textureT, TexCoord));    
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);  
    // Check whether fragPos is in the spot light
    float theta = dot(lightDir, normalize(-light.direction)); 
    vec3 diffuse;
    if (theta > light.cutOff) {
        diffuse = light.diffuse * vec3(texture(textureT, TexCoord)) * diff;
    }
    else {
        diffuse = vec3(0.0, 0.0, 0.0);
    }
    // diffuse *= attenuation;

    return (ambient + diffuse);
}
