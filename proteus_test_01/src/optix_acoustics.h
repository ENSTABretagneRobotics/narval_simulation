#ifndef _DEF_PROTEUS_TESTS_OPTIX_ACOUSTICS_H_
#define _DEF_PROTEUS_TESTS_OPTIX_ACOUSTICS_H_

#include <rtac_base/types/Complex.h>
using namespace rtac;

#include <rtac_optix/RaytypeFactory.h>
#include <rtac_optix/Material.h>
using namespace rtac::optix;

#include <rtac_simulation/CelerityProfile.h>
using namespace rtac::simulation;

struct Params {
    unsigned int rayCount;
    unsigned int iterationCount;
    float rayStep;
    float frequency;
    float3* rayOrigins;
    float3* rayDirections;
    float3* rayPaths;
    OptixTraversableHandle topObject;
};

struct AcousticRayPayload
{
    Complex<float> value;
    float travelDistance;
    float travelTime;
};

struct AcousticMedium
{
    CelerityProfileView profile;
};

using AcousticRay    = RaytypeFactory<AcousticRayPayload>::Raytype<0>;
using MissMaterial   = Material<AcousticRay, AcousticMedium>;
using MirrorMaterial = Material<AcousticRay, void>;


#endif //_DEF_PROTEUS_TESTS_OPTIX_ACOUSTICS_H_


