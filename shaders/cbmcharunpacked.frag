#version 330 core
in vec2 fragCoord;
in vec4 textColor;
in vec4 backgroundColor;

uniform vec2 iResolution;
bool pixels[16];

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    vec2 uv = fragCoord / iResolution.xy;
    
    // Flip Y if needed (optional, depending on your preference)
    // vec2 myuv = vec2(uv.x, 1.0 - uv.y);
    
    // Define colors
    vec4 textColor = vec4(1.0, 1.0, 1.0, 1.0);    // White
    vec4 backgroundColor = vec4(0.0, 0.0, 0.0, 1.0); // Black
    
    // Divide screen into 4x4 grid (16 blocks)
    float blocksPerRow = 4.0;
    vec2 blockCoord = floor(uv * blocksPerRow);
    
    // Calculate index (0-15) based on row and column
    int index = int(blockCoord.y) * 4 + int(blockCoord.x);
    
    for(int i = 0; i < 16; i++){
        pixels[i] = false;
    }

    for(int i = 0; i < 16; i+=2){
        pixels[i] = true;
    }

    // Make sure index is within bounds
    if(index >= 0 && index < 16) {
        // Mix between textColor and backgroundColor based on pixel value
        // If pixel is true, show textColor; if false, show backgroundColor
        fragColor = mix(backgroundColor, textColor, float(pixels[index]));
    } else {
        fragColor = backgroundColor;
    }
}