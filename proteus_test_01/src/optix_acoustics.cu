#include "optix_acoustics.h"

#include <rtac_optix/helpers/maths.h>

extern "C" {
    __constant__ Params params;
}

/**
 * This the "main" program of ray tracing. It is called for each ray. 
 */
extern "C" __global__ void __raygen__acoustic_ray()
{
    auto idx = optixGetLaunchIndex();
    
    AcousticRay ray;
    ray.value          = Complex<float>(0,0);
    ray.travelDistance = 0.0f;
    ray.travelTime     = 0.0f;

    for(int i = 0; i < params.iterationCount; i++) {
        ray.trace(params.topObject,
                  params.rayOrigins[idx.x],
                  params.rayDirections[idx.x],
                  1.0e-3, params.rayStep);
        params.rayPaths[params.iterationCount*idx.x + i] = params.rayOrigins[idx.x];
    }
}

/**
 * The miss program is called if a ray does not intersected with anything
 * before reaching its maximumrange.
 *
 * Here it is used to implement a Euler intgration scheme to curve the rays. At
 * each step, the local sound celerity and its gradiant is fetched from the
 * local Acoustic medium and are used to compute a change in ray direction.
 */
extern "C" __global__ void __miss__acoustic_ray()
{
    // This get the propagation medium information from the shader binding
    // table.

    auto medium = (const AcousticMedium*)optixGetSbtDataPointer();
    auto idx    = optixGetLaunchIndex();
    
    // These fetch current ray information (payload form ray registers, ray
    // origin in world frame and direction in workd frame.
    auto ray      = AcousticRay::from_registers();
    float3 origin = AcousticRay::world_origin();
    float3 dir    = AcousticRay::world_direction();
    
    // Euler integration step
    float3 deltaDir = medium->profile.get_direction_delta(origin, dir);
    params.rayOrigins[idx.x]    = origin + params.rayStep * dir;
    params.rayDirections[idx.x] = normalized(dir + params.rayStep * deltaDir);

    // updating ray payload
    float dist = params.rayStep*length(dir);
    ray.travelDistance += dist;
    ray.travelTime     += dist / medium->profile.get_celerity(origin.z);
    ray.set_payload(); // storing ray payload in ray registers
}

/**
 * The closest hit program is called on the ray closest intersection with an
 * object.
 *
 * This specific program implements a perfect mirror material (suitable for
 * water surface reflexion for example)
 */
extern "C" __global__ void __closesthit__mirror()
{
    auto idx = optixGetLaunchIndex();

    // is a perfect mirror material for now
    float3 hitPosition, hitNormal;
    helpers::get_triangle_hit_data(hitPosition, hitNormal);

    hitPosition = optixTransformPointFromObjectToWorldSpace(hitPosition);
    hitNormal   = optixTransformNormalFromObjectToWorldSpace(hitNormal);

    float3 origin = AcousticRay::world_origin();
    float3 dir    = AcousticRay::world_direction();

    float3 nextDir = normalized(dir - 2.0*dot(dir, hitNormal)*hitNormal);

    auto ray = AcousticRay::from_registers();
    params.rayOrigins[idx.x]    = hitPosition + 0.05 * nextDir;
    params.rayDirections[idx.x] = nextDir;

    ray.set_payload();
}


