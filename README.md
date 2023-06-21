# narval_simulator

This repository aims to be a start point for someone wanting to use the RTAC
simulator. It does not contain any core code, but rather some examples a user
could copy and  modifiy to his own use.

## Getting started

Keep in mind that this code was solely tested on Ubuntu 20.04. It should work
reasonably on Ubuntu 18.04 and later but some issues may arise.

### Installation

All of these examples will automatically download source dependencies using
CMake's FetchContent feature. You only need to install system dependencies
beforehand.

#### Eigen

Eigen's version on apt repositories is an old one (RTAC needs Eigen > 3.4). You
can either install from sources yourself or let RTAC download it using CMake's
FetchContent feature.

If you want to install Eigen yourself, get it from
[here](https://gitlab.com/libeigen/eigen).

#### CUDA

To install CUDA, follow the instructions given
[here](https://github.com/ENSTABretagneRobotics/rtac_base) (Installing CUDA on
Ubuntu).

Remember : do not let the CUDA installer update your graphical driver. It is
very bad at this and may leave you without a graphical session. Leave the driver
installation to Ubuntu drivers manager and install CUDA from the runfile (NOT
THE .deb), WITHOUT driver update.

#### OptiX

OptiX is NVIDIA's ray tracing library based on CUDA. Follow instructions
[here](https://github.com/ENSTABretagneRobotics/rtac_base).  to install it.
(RTAC need Optix 7.3.0 or later).

#### Other dependencies.

Install these from apt

```
sudo apt-get install -y build-essential \
                        cmake           \
                        libyaml-cpp-dev \
                        libglfw3-dev    \
                        libglew-dev     \
                        libpng-dev      \
                        libjpeg-dev
```

### Check the installation

Go into the 01_st_raphael_sim directory and compile the CMake package:

```
cd 01_st_raphael_sim
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j8
./01_st_raphael_sim
```
If everything went well, you should see a front scan sonart simulation.


