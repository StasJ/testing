#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308
#define EPSILON 1.19e-07

bool IntersectRayBoundingBox(vec3 o, vec3 d, float rt0, vec3 boxMin, vec3 boxMax, OUT float t0, OUT float t1)
{
    vec3 tMin = (boxMin - o) / d;
    vec3 tMax = (boxMax - o) / d;
    vec3 bt1 = min(tMin, tMax);
    vec3 bt2 = max(tMin, tMax);
    t0 = max(max(max(bt1.x, bt1.y), bt1.z), rt0);
    t1 = min(min(bt2.x, bt2.y), bt2.z);
    return t0 <= t1;
}

bool IntersectRayPlane(vec3 o, vec3 d, float rt0, vec3 v0, vec3 n, OUT float t)
{
    float denom = dot(n, d);
    
    if (abs(denom) > 1e-6) {
        t = dot(v0 - o, n) / denom;
        return t >= rt0;
    }
    return false;
}

bool IntersectRayTriangle(vec3 o, vec3 d, float rt0, vec3 v0, vec3 v1, vec3 v2, OUT float t, OUT vec3 barycentric)
{
    vec3 n = cross(v1-v0,v2-v0);
    
    if (IntersectRayPlane(o, d, rt0, v0, n, t)) {
        vec3 P = o + d * t;
        
        vec3 edge0 = v1-v0;
        vec3 vp0 = P - v0;
        vec3 C0 = cross(edge0, vp0);
        if (dot(n, C0) < 0) return false;
        
        vec3 edge1 = v2-v1;
        vec3 vp1 = P - v1;
        vec3 C1 = cross(edge1, vp1);
        if (dot(n, C1) < 0) return false;
        
        vec3 edge2 = v0 - v2;
        vec3 vp2 = P - v2;
        vec3 C2 = cross(edge2, vp2);
        if (dot(n, C2) < 0) return false;
        
        float area = length(n);
        float u = length(C1) / area;
        float v = length(C2) / area;
        float w = 1-u-v;
        barycentric = vec3(u,v,w);
        
        return true;
    }
    return false;
}

int MaxDimension(vec3 v)
{
	return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

vec3 Permute(vec3 v, int x, int y, int z)
{
	return vec3(v[x], v[y], v[z]);
}

bool IntersectRayTriangleIntel(vec3 o, vec3 dir, float rt0, vec3 v0, vec3 v1, vec3 v2, OUT float t, OUT vec3 barycentric)
{
	// Transform triangle vertices to ray coordinate space
	// Translate vertices based on ray origin
	vec3 p0t = v0 - o;
	vec3 p1t = v1 - o;
	vec3 p2t = v2 - o;

	// Permute components of triangle vertices and ray direction
	int kz = MaxDimension(abs(dir));
	int kx = kz + 1; if (kx == 3) kx = 0;
	int ky = kx + 1; if (ky == 3) ky = 0;
	vec3 d = Permute(dir, kx, ky, kz);
	p0t = Permute(p0t, kx, ky, kz);
	p1t = Permute(p1t, kx, ky, kz);
	p2t = Permute(p2t, kx, ky, kz);

	// Apply shear transformation to translated vertex positions
	float Sx = -d.x / d.z;
	float Sy = -d.y / d.z;
	float Sz = 1.f / d.z;
	p0t.x += Sx * p0t.z;
	p0t.y += Sy * p0t.z;
	p1t.x += Sx * p1t.z;
	p1t.y += Sy * p1t.z;
	p2t.x += Sx * p2t.z;
	p2t.y += Sy * p2t.z;


	// Compute edge function coefficients e0, e1, and e2
	float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
	float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
	float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

#if __VERSION__ >= 400
	// Fall back to double-precision test at triangle edges
	if (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f) {
		double p2txp1ty = double(p2t.x) * double(p1t.y);
		double p2typ1tx = double(p2t.y) * double(p1t.x);
		e0 = float(p2typ1tx - p2txp1ty);
		double p0txp2ty = double(p0t.x) * double(p2t.y);
		double p0typ2tx = double(p0t.y) * double(p2t.x);
		e1 = float(p0typ2tx - p0txp2ty);
		double p1txp0ty = double(p1t.x) * double(p0t.y);
		double p1typ0tx = double(p1t.y) * double(p0t.x);
		e2 = float(p1typ0tx - p1txp0ty);
	}
#endif

	// Perform triangle edge and determinant tests
	if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
		return false;
	float det = e0 + e1 + e2;
	if (det == 0)
		return false;

	// Compute scaled hit distance to triangle and test against ray t range
//    p0t.z *= Sz;
//    p1t.z *= Sz;
//    p2t.z *= Sz;
//    float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
//    if (det < 0 && (tScaled >= rt0*det || tScaled < FLT_MAX * det))
//        return false;
//    else if (det > 0 && (tScaled <= rt0*det || tScaled > FLT_MAX * det))
//        return false;
//    t = tScaled * invDet;

    // This routine provides consistent results at t = t0
    // as opposed to the one above
    if (!IntersectRayPlane(o, dir, rt0, v0, cross(v1-v0,v2-v0), t))
        return false;

	float invDet = 1 / det;
	float b0 = e0 * invDet;
	float b1 = e1 * invDet;
	float b2 = e2 * invDet;
    barycentric = vec3(b0, b1, b2);

	return true;
}

#ifdef USE_INTEL_TRI_ISECT
#define TRI_INSTERSECT_ROUTINE IntersectRayTriangleIntel
#else
#define TRI_INSTERSECT_ROUTINE IntersectRayTriangle
#endif

bool IntersectRayQuad(vec3 o, vec3 d, float rt0, vec3 v0, vec3 v1, vec3 v2, vec3 v3, OUT float t, OUT vec4 weights)
{
    vec3 uvw;
    if (TRI_INSTERSECT_ROUTINE(o, d, rt0, v0, v1, v2, t, uvw)) {
        weights = vec4(uvw, 0);
        return true;
    }
    if (TRI_INSTERSECT_ROUTINE(o, d, rt0, v0, v2, v3, t, uvw)) {
        weights = vec4(uvw.x, 0, uvw.y, uvw.z);
        return true;
    }
    return false;
}
