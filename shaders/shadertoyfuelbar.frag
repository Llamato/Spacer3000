float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float cycleTimeS = 10.0;
    float fuelQuantity = 1.0-map(mod(iTime, cycleTimeS), 0.0, cycleTimeS-1.0, 0.0, 1.0);
    vec2 uv = fragCoord/iResolution.xy;
    
    vec3 fuelEmptyColor = vec3(1.0, 0.0, 0.0);
    vec3 fuelFullColor = vec3(0.0, 1.0, 0.0);
    vec4 fuelbarColor = vec4(mix(fuelEmptyColor, fuelFullColor, uv.x), 1.0);
    float fuelbarMask = 1.0-step(fuelQuantity, uv.x);
    fragColor = fuelbarColor * fuelbarMask;
    //if(fragColor.a < 0.01) discard;
}