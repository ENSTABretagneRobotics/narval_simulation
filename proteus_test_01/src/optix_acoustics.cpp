#include <iostream>
using namespace std;

#include <rtac_base/time.h>
using namespace rtac::time;

#include <rtac_base/containers/HostVector.h>
using namespace rtac;

#include <rtac_base/external/npy.h>
using namespace rtac::external;

#include <rtac_base/cuda/DeviceObject.h>
#include <rtac_base/cuda/CudaVector.h>
#include <rtac_base/cuda/Texture2D.h>
#include <rtac_base/cuda/texture_utils.h>
using namespace rtac::cuda;

#include <rtac_optix/Context.h>
#include <rtac_optix/Pipeline.h>
#include <rtac_optix/MeshGeometry.h>
#include <rtac_optix/ObjectInstance.h>
#include <rtac_optix/GroupInstance.h>
#include <rtac_optix/ShaderBinding.h>
#include <rtac_optix/ShaderBindingTable.h>
using namespace rtac::optix;

#include "ProfileExamples.h"
//using namespace proteus::algorithm;

#include <optix_acoustics02/ptx_files.h>

#include "optix_acoustics.h"

//std::ostream& operator<<(std::ostream& os, const float3& v)
//{
//    //os << setprecision(2) << v.x << ", " << v.y << ", " << v.z;
//    os << v.x << ", " << v.y << ", " << v.z;
//    return os;
//}

void save_ray_paths(const std::string& filename,
                    unsigned int rayCount, unsigned int iterationCount,
                    const HostVector<float3>& paths)
{
    assert(paths.size() == rayCount * iterationCount);

    save_npy_array(filename, {rayCount, iterationCount, 3},
                   3*paths.size(), (const float*)paths.data());

    //std::ofstream of(filename, std::ofstream::out);
    //if(!of.is_open()) {
    //    std::ostringstream oss;
    //    oss << "Could not open file for writing : " << filename << std::endl;
    //    throw std::runtime_error(oss.str());
    //}
    //
    //for(int n = 0; n < rayCount; n++) {
    //    std::ostringstream oss;
    //    oss <<         paths[iterationCount*n].x
    //        << ", " << paths[iterationCount*n].y
    //        << ", " << paths[iterationCount*n].z;
    //        
    //    for(int i = 1; i < iterationCount; i++) {
    //        oss << ", " << paths[iterationCount*n + i].x
    //            << ", " << paths[iterationCount*n + i].y
    //            << ", " << paths[iterationCount*n + i].z;
    //    }
    //    oss << "\r\n";
    //    of << oss.str();
    //}
}

MeshGeometry::Ptr dickins_bathy(const Context::ConstPtr& context)
{
    HostMesh<> mesh;

    mesh.points().resize(10);
    mesh.points()[0].x = -100000; mesh.points()[0].y = -50000; mesh.points()[0].z = -3000;
    mesh.points()[1].x = -100000; mesh.points()[1].y =  50000; mesh.points()[1].z = -3000;
    mesh.points()[2].x =   10000; mesh.points()[2].y = -50000; mesh.points()[2].z = -3000;
    mesh.points()[3].x =   10000; mesh.points()[3].y =  50000; mesh.points()[3].z = -3000;
    mesh.points()[4].x =   20000; mesh.points()[4].y = -50000; mesh.points()[4].z =  -500;
    mesh.points()[5].x =   20000; mesh.points()[5].y =  50000; mesh.points()[5].z =  -500;
    mesh.points()[6].x =   30000; mesh.points()[6].y = -50000; mesh.points()[6].z = -3000;
    mesh.points()[7].x =   30000; mesh.points()[7].y =  50000; mesh.points()[7].z = -3000;
    mesh.points()[8].x =  100000; mesh.points()[8].y = -50000; mesh.points()[8].z = -3000;
    mesh.points()[9].x =  100000; mesh.points()[9].y =  50000; mesh.points()[9].z = -3000;

    mesh.faces().resize(8);
    mesh.faces()[0].x = 0; mesh.faces()[0].y = 1; mesh.faces()[0].z = 3;
    mesh.faces()[1].x = 0; mesh.faces()[1].y = 3; mesh.faces()[1].z = 2;
    mesh.faces()[2].x = 2; mesh.faces()[2].y = 3; mesh.faces()[2].z = 5;
    mesh.faces()[3].x = 2; mesh.faces()[3].y = 5; mesh.faces()[3].z = 4;
    mesh.faces()[4].x = 4; mesh.faces()[4].y = 5; mesh.faces()[4].z = 7;
    mesh.faces()[5].x = 4; mesh.faces()[5].y = 7; mesh.faces()[5].z = 6;
    mesh.faces()[6].x = 6; mesh.faces()[6].y = 7; mesh.faces()[6].z = 9;
    mesh.faces()[7].x = 6; mesh.faces()[7].y = 9; mesh.faces()[7].z = 8;

    return MeshGeometry::Create(context, DeviceMesh<>::Create(mesh));
}

int main()
{
    // Preparing programs /////////////////////////////////
    auto ptxFiles = optix_acoustics02::get_ptx_files();

    auto context  = Context::Create();
    auto pipeline = Pipeline::Create(context);
    pipeline->compile_options().numPayloadValues = 4;
    auto module   = pipeline->add_module("module", ptxFiles["src/optix_acoustics.cu"]);

    auto raygen = pipeline->add_raygen_program("__raygen__acoustic_ray", "module");
    auto miss   = pipeline->add_miss_program("__miss__acoustic_ray",     "module");

    auto hit = pipeline->add_hit_programs();
    hit->set_closesthit({"__closesthit__mirror", pipeline->module("module")});
    auto mirror = MirrorMaterial::Create(hit);

    // Celerity profile configuration
    // Dickins parameters
    unsigned int rayCount = 80;
    unsigned int iterationCount = 100000;
    float rayStep               = 1.0;
    float zBottom         = -3000;
    float angle           =    20;
    float z0              =   -18;
    //auto celerityData = proteus::algorithm::ProfileExamples::dickins();
    auto celerityData = proteus::algorithm::ProfileExamples::dickins_linear();
    
    // // Arctic parameters
    // unsigned int rayCount = 500;
    // unsigned int iterationCount = 10000;
    // float rayStep               = 10.0;
    // float zBottom         = -3750;
    // float angle           =  16.7;
    // float z0              =  -100;
    // auto celerityData = ProfileExamples::arctic();

    // // Munk parameters
    // float zBottom = -5000;
    // float angle   = 30;
    // float z0      = -1000;
    // auto celerityData = ProfileExamples::dickins();

    
    // Preparing scene objects /////////////////////////////////
    // Default geometry for top and bottom of the sea (will be stretched when
    // creating the ObjectInstances).
    auto cubeGeom = MeshGeometry::CreateCube(context, 0.5f);
    cubeGeom->material_hit_setup({OPTIX_GEOMETRY_FLAG_NONE});
    cubeGeom->enable_vertex_access();
    
    auto dickinsBathy = dickins_bathy(context);
    dickinsBathy->material_hit_setup({OPTIX_GEOMETRY_FLAG_NONE});
    dickinsBathy->enable_vertex_access();

    // Water surface boundary
    auto surface = ObjectInstance::Create(cubeGeom);
    surface->add_material(mirror, 0);
    surface->set_transform({200000,      0,   0,     0,
                                 0, 100000,   0,     0,
                                 0,      0, 1.0,   0.5});

    auto bottom = ObjectInstance::Create(dickinsBathy);
    bottom->add_material(mirror, 0);
    //auto bottom = ObjectInstance::Create(cubeGeom);
    //bottom->add_material(mirror, 0);
    //bottom->set_transform({100000,      0,   0,   50000,
    //                            0, 100000,   0,       0,
    //                            0,      0, 1.0, zBottom-0.5f});

    auto topObject = GroupInstance::Create(context);
    topObject->add_instance(surface);
    topObject->add_instance(bottom);

    // Creating the "MissMaterial" (handle the sound curvature given the sound
    // celerity. ///////////////////////////////////////////////////////
    AcousticMedium missParams;
    //unsigned int N = 8192;
    unsigned int N = 256;
    auto profile = rtac::simulation::CelerityProfile::Create(celerityData.x0().size(),
                                                             celerityData.x0().data(),
                                                             celerityData.y0().data(),
                                                             N);
    missParams.profile = profile->view();

    // Filling the ShaderBindingTable
    auto sbt = ShaderBindingTable::Create(1);
    sbt->set_raygen_record(ShaderBinding<void>::Create(raygen));
    sbt->add_miss_record(MissMaterial::Create(miss, missParams));
    sbt->add_object(surface);
    sbt->add_object(bottom);
    
    // Filling glogal parameters.
    float3 origin({0.0,0.0,z0});
    //float angle                 = 20.0;
    //unsigned int rayCount       = 80;
    // unsigned int iterationCount = 100000;
    // float rayStep               = 1.0;
    CudaVector<float3> paths(rayCount*iterationCount);
    CudaVector<float3> origins;
    CudaVector<float3> dirs;
    {
        std::vector<float3> o(rayCount);
        std::vector<float3> d(rayCount);
        for(int i = 0; i < o.size(); i++) {
            o[i] = origin;
            float a = M_PI*(2.0*angle*i/(o.size()-1) - angle) / 180.0;
            d[i] = float3({cos(a), 0.0, sin(a)});
        }
        origins = o;
        dirs    = d;
    }
    
    DeviceObject<Params> params;
    params.rayCount       = rayCount;
    params.iterationCount = iterationCount;
    params.rayStep        = rayStep;
    params.frequency      = 10e3;
    params.rayOrigins     = origins.data();
    params.rayDirections  = dirs.data();
    params.rayPaths       = paths.data();
    params.topObject      = *topObject;
    params.update_device();
    
    // Pre-build to not take build into account for performance checkup.
    pipeline->build();
    sbt->build();

    cout << "Ray tracing... " << std::flush;
    Clock clock;
    OPTIX_CHECK( 
        optixLaunch(*pipeline, 0,
                    (CUdeviceptr)params.device_ptr(), sizeof(Params),
                    sbt->sbt(), params.rayCount, 1, 1);
    );
    cudaDeviceSynchronize();
    CUDA_CHECK_LAST();

    auto t = clock.now<double>();
    cout << "Done (ellapsed : " << std::setprecision(2)
         << t << "s).\n" << std::flush;

    cout << "Writing data to disk... " << std::flush;
    save_ray_paths("rayPaths.npy", rayCount, iterationCount, paths);
    cout << "Done." << std::endl << std::flush;

    return 0;
}



