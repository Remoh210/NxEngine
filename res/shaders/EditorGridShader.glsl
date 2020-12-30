#define TILE_SIZE 1.0
#define LINE_WIDTH 1.0

#define ANTIALIASED_LINES 0

// gives the grid lines alpha factor
float grid(in vec2 uv)
{
    vec2 grid = vec2(0.0);
    float line = 0.0;
    
    for (int i = 0; i < 3; ++i)
    {
#if ANTIALIASED_LINES == 1
    	// antialiased lines
    	grid = 1.0 * abs(mod(uv + 0.5*TILE_SIZE, TILE_SIZE * pow(10.0, float(i))) - 0.5) / fwidth(uv) / LINE_WIDTH;
    	line = max(line, pow(4.0, float(i)) * (1.0 - min(min(grid.x, grid.y), 1.0)));
#else
    	// almost pixel-perfect lines
    	grid = 1.0 * fwidth(uv)/mod(uv, pow(10.0, float(i)));
    	line = max(line, pow(4.0, float(i)) * step(1.0/LINE_WIDTH, max(grid.x, grid.y)));
#endif
    }
    
    return line;
}

// intersection with floor plane
bool iFloor(in vec3 ro, in vec3 rd, out float t)
{
    float floor_y = -1.5;
    t = (floor_y - ro.y) / rd.y;
    return t > 0.0;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    
    // Normalized pixel coordinates (from -1 to 1)
    vec2 nuv = -1.0 + 2.0 * uv;
    nuv.x *= iResolution.x / iResolution.y; // aspect ratio correction

    // Camera -> world matrix
    vec3 obs = getValueAt(INDEX_POSITION).xyz;
    vec3 vrp = obs + getValueAt(INDEX_DIRECTION).xyz;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 ww = normalize(obs - vrp);
    vec3 uu = cross(up, ww);
    vec3 vv = cross(ww, uu);
    
    // Ray origin / direction in world space
   	vec3 ro = obs;
    vec3 rd = uu * nuv.x + vv * nuv.y - ww * 2.8;
    
    // Background color
    vec3 topColor = vec3(0.4);
    vec3 botColor = vec3(0.1);
    vec3 col = mix(botColor, topColor, smoothstep(-0.05, 0.1, rd.y));
    
    float t;
    if (iFloor(ro, rd, t))
    {
        vec3 floorPos = ro + t*rd;
        vec2 uv = floorPos.xz;
        
        float alpha = grid(uv);
        
        alpha = min(1.0, alpha/(t*t)); // distance attenuation
        
        col = mix(col, vec3(1.0), alpha);
    }
    
    col = sqrt(col);

    // Output to screen
    fragColor = vec4(col,1.0);
}