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
    float baseTriangleArea = triangleArea(p1, p2, p3);
    float a1 = triangleArea(p, p1, p2);
    float a2 = triangleArea(p, p2, p3);
    float a3 = triangleArea(p, p3, p1);
    float asum = a1+a2+a3;
    return step(baseTriangleArea,asum);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec2 top = vec2(0.5, 1.0);
    vec2 left = vec2(0.0,0.0);
    vec2 right = vec2(1.0,0.0);
    fragColor = vec4(isInsideTriangle(uv, left, right, top));
}