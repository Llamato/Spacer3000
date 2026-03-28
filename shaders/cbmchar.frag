#version 330 core
in vec2 fragCoord;

uniform int packedPixels[2];

out vec4 fragColor;
const int intSize = 4;

bool getPixel(int index) {
    return bool((packedPixels[index / intSize]) << (index % intSize));
}

int setPixel(int pixels, int index) {
    return pixels | (1 << index);
}

int clearPixel(int pixels, int index) {
    return pixels & ~(1 << index);
}

void main() {
    vec2 uv = fragCoord;
    vec2 myuv = vec2(uv.x,1.0-uv.y);
    vec4 textColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 backgroundColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 pixelCoord = myuv * 8.0;
    int currentPixel = int(floor(pixelCoord.y) * 8.0 + floor(pixelCoord.x));
    fragColor = mix(textColor, backgroundColor, float(getPixel(currentPixel)));
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}