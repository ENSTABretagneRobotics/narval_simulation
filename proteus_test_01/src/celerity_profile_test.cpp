#include <iostream>
using namespace std;

#include <matplotlibcpp.h>
namespace plt = matplotlibcpp;

#include "CelerityProfile.h"
using namespace proteus::algorithm;
using Vector = CelerityProfile::Vector;

Vector linspace(float vmin, float vmax, unsigned int N)
{
    Vector output(N);
    for(int n = 0; n < N; n++) {
        output(n) = (vmax - vmin) * n / (N - 1) + vmin;
    }
    return output;
}

std::vector<float> to_vector(const Vector& v)
{
    std::vector<float> out(v.size());
    for(int i = 0; i < out.size(); i++) {
        out[i] = v(i);
    }
    return out;
}

int main()
{
    auto profile = CelerityProfile::dickins();

    unsigned int N = 8192;
    auto x = linspace(-3000.0, 0.0, N);
    Vector c(N), grad(N);
    profile.interpolate(x,c,grad);
    
    plt::subplot(1,2,1);
    plt::plot(to_vector(c), to_vector(x), {{"label", "celerity"}});
    plt::grid(true);
    plt::legend();

    plt::subplot(1,2,2);
    plt::plot(to_vector(grad), to_vector(x), {{"label", "grad(1/c)"}});
    plt::grid(true);
    plt::legend();

    plt::show();
    
    return 0;
}
