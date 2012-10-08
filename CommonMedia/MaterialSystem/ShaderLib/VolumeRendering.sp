// Volume Rendering Utils

const vec3 BBoxMin = vec3( -1.0, -1.0, -1.0 );
const vec3 BBoxMax = vec3(  1.0,  1.0,  1.0 );

struct Ray {
   vec3 o;   // origin
   vec3 d;   // direction
};

// calculate intersection between ray and box
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
bool IntersectBox(Ray r, vec3 boxmin, vec3 boxmax, out float tnear, out float tfar)
{
    // compute intersection of ray with all six bbox planes
    vec3 invR = 1.0 / r.d;
    vec3 tbot = invR * (boxmin.xyz - r.o);
    vec3 ttop = invR * (boxmax.xyz - r.o);

    // re-order intersections to find smallest and largest on each axis
    vec3 tmin = min (ttop, tbot);
    vec3 tmax = max (ttop, tbot);

    // find the largest tmin and the smallest tmax
    vec2 t0 = max (tmin.xx, tmin.yz);
    float largest_tmin = max (t0.x, t0.y);
    t0 = min (tmax.xx, tmax.yz);
    float smallest_tmax = min (t0.x, t0.y);

    // check for hit
    bool hit;

    if ( largest_tmin > smallest_tmax ) hit = false; else hit = true;

    tnear = largest_tmin;
    tfar = smallest_tmax;

    return hit;
}
