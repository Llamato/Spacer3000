#version 330 core

in vec2 fragCoord;

void main() {
    float cycleTimeS = 10.0;
    float fuelQuantity = 
    vec2 uv = fragCoord
    
    vec3 fuelEmptyColor = vec3(1.0, 0.0, 0.0);
    vec3 fuelFullColor = vec3(0.0, 1.0, 0.0);
    vec4 fuelbarColor = vec4(mix(fuelEmptyColor, fuelFullColor, uv.x), 1.0);
    float fuelbarMask = 1.0-step(fuelQuantity, uv.x);
    fragColor = fuelbarColor * fuelbarMask;
    if(fragColor.a < 0.01) discard;
}