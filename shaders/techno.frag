out vec4 FragColor;
in vec2 fragCord;
in vec3 iResolution;

in vec3 color;

float triangleArea(vec2 p1, vec2 p2, vec2 p3)
{
    float l1 = length(p2-p1);
    float l2 = length(p3-p2);
    float l3 = length(p1-p3);
    float s = 0.5 * (l1+l2+l3);
    return sqrt(s * (s-l1) * (s-l2) * (s-l3));
}

float isInsideTriangle(vec2 p, vec2 p1, vec2 p2, vec2 p3)
{
    // Compute vectors
    vec2 v0 = p2 - p1;
    vec2 v1 = p3 - p1;
    vec2 v2 = p - p1;

    // Compute dot products
    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1);
    float dot02 = dot(v0, v2);
    float dot11 = dot(v1, v1);
    float dot12 = dot(v1, v2);

    // Compute barycentric coordinates
    float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return step(0.0, u) * step(0.0, v) * step(0.0, 1.0 - u - v);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec2 outerTop = vec2(0.5, 1.0);
    vec2 outerLeft = vec2(0.0,0.0);
    vec2 outerRight = vec2(1.0,0.0);
    vec2 innerTop = vec2(outerTop.x, outerTop.y - 0.1/3.0);
    vec2 innerLeft = vec2(outerLeft.x + 0.1 * 1.0/3.0, outerLeft.y + 0.1 * 1.0/3.0);
    vec2 innerRight = vec2(outerRight.x - 0.1 * 1.0/3.0, outerRight.y + 0.1 * 1.0/3.0);
    
    float innerDistance = isInsideTriangle(uv, innerLeft, innerRight, innerTop);
    float outerDistance = isInsideTriangle(uv, outerLeft, outerRight, outerTop);
    fragColor = step(0.5,innerDistance) * vec4(1,1, 1.0, 1.0) + step(0.9, outerDistance) * vec4(mix(uv.xy, uv.yx, abs(sin(iTime))),1.0,1.0);
}