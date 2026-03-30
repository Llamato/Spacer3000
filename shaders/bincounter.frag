#version 330 core
#extension GL_ARB_gpu_shader_int64 : enable
in vec4 textColor;
in vec4 bgColor;

uniform vec2 iResolution;
uniform float iTime;

out vec4 FragColor;

/*const vec4 textColor = vec4(1.0,1.0,1.0,1.0);
const vec4 bgColor = vec4(0.0,1.0,1.0,1.0);*/

const int columns = 8;
const int rows = 8;

float frac(float f) {
    return f - int(f);
}

bool iand(int x, int n) {
    return frac(x/(2^n))*(2^n) >= 1.0;
}

bool getPixelValue(int index) {
    int row = index / 8;
    int col = index % 8;
    return (row + col) % 2 == 0;
}

bool getPixelValueOverTime(int index) {
    uint64_t counter = uint64_t(mod(iTime * 1000.0, 18446744073709551615.0));
    return ((counter >> uint64_t(index)) & 1u) != 0u;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 blockCoord = floor(uv * float(columns));
    int index = int(blockCoord.y) * columns + int(blockCoord.x);
    
    if(index >= 0 && index < columns * rows) {
        FragColor = mix(bgColor, textColor, float(getPixelValueOverTime(index)));
    } else {
        FragColor = bgColor;
    }
}