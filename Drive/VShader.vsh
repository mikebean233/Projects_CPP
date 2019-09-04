vs.1.0                      
m4x4 oPos, v0, c0

// lighting calculations
dp3 r0, v3, c4     
mul oD0, r0.x, v5

// texture pass-through 
//mov oT0.xy, v7