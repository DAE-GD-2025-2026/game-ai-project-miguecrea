
#include"DrawDebugHelpers.h"
#define DRAW_CIRCLE(World, Pos, Radius, Color, Thickness) \
DrawDebugCircle( \
World, \
Pos, \
Radius, \
64, \
Color, \
false, \
0.f, \
0, \
Thickness, \
FVector(1,0,0), \
FVector(0,1,0), \
false \
)

#define DRAW_VECTOR(World,Start,End, Color) \
DrawDebugLine(World\
,Start\
,End\
,Color); 

