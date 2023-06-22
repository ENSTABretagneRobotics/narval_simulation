#include "ProfileExamples.h"

namespace proteus { namespace algorithm {

using namespace rtac::indexing;

ProfileExamples ProfileExamples::dickins()
{
    Eigen::MatrixXf data(23,2);
    //data <<   -0, 1476.7,
    data <<   -0, 1480.8,
             -38, 1476.7,
             -50, 1472.6,
             -70, 1468.8,
            -100, 1467.2,
            -140, 1471.6,
            -160, 1473.6,
            -170, 1473.6,
            -200, 1472.7,
            -215, 1472.2,
            -250, 1471.6,
            -300, 1471.6,
            -370, 1472.0,
            -450, 1472.7,
            -500, 1473.1,
            -700, 1474.9,
            -900, 1477.0,
           -1000, 1478.1,
           -1250, 1480.7,
           -1500, 1483.8,
           -2000, 1490.5,
           -2500, 1498.3,
           -3000, 1506.5;
    data.colwise().reverseInPlace();
    return ProfileExamples(data(all,0), data(all,1));
}

ProfileExamples ProfileExamples::dickins_linear()
{
    Eigen::MatrixXf data(23,2);
    data <<   -0, 1476.0,
		    -136, 1477.3,
		    -272, 1478.7,
		    -409, 1480.0,
		    -545, 1481.4,
		    -681, 1482.8,
		    -818, 1484.1,
		    -954, 1485.5,
		   -1090, 1486.9,
		   -1227, 1488.2,
		   -1363, 1489.6,
		   -1500, 1491.0,
		   -1636, 1492.3,
		   -1772, 1493.7,
		   -1909, 1495.0,
		   -2045, 1496.4,
		   -2181, 1497.8,
		   -2318, 1499.1,
		   -2454, 1500.5,
		   -2590, 1501.9,
		   -2727, 1503.2,
		   -2863, 1504.6,
		   -3000, 1506.0;

    data.colwise().reverseInPlace();
    return ProfileExamples(data(all,0), data(all,1));
}

ProfileExamples ProfileExamples::arctic()
{
    Eigen::MatrixXf data(7,2);
    data << -0.0, 1436.0,
          -200.0, 1458.4,
          -300.0, 1460.5,
         -1000.0, 1466.7,
         -2000.0, 1479.6,
         -2500.0, 1487.9,
         -3750.0, 1510.4;

    data.colwise().reverseInPlace();
    return ProfileExamples(data(all,0), data(all,1));
}

ProfileExamples ProfileExamples::munk()
{
    int N = 2048;
    float ztop    = 0.0;
    float zbottom = 5000.0;
    float e       = 0.00737;

    Eigen::MatrixXf data(N,2);
    for(int n = 0; n < N; n++) {
        float z  = ztop + ((zbottom - ztop)*n) / (N-1);
        float zt = 2.0*(z - 1300.0) / 1300.0;

        data(n,0) = -z;
        data(n,1) = 1500.0*(1.0 + e*(zt - 1 + exp(-zt)));
    }

    data.colwise().reverseInPlace();
    return ProfileExamples(data(all,0), data(all,1));
}

ProfileExamples::ProfileExamples(const Vector& x0, const Vector& y0) :
    Interpolator(Interpolator::CreateCubicSpline(x0,y0))
{}

void ProfileExamples::interpolate(const Vector& x, Vector& c, Vector& grad)
{
    auto interp = std::dynamic_pointer_cast<const rtac::algorithm::InterpolatorCubicSpline<float>>(
                      this->interpolator_);
    if(!interp) {
        throw std::runtime_error("FATAL : bad conversion");
    }

    Indexes idx = interp->lower_bound_indexes(make_view(x));
    for(int i = 0; i < x.size(); i++) {
        if(idx[i] == interp->x0().size() - 1) {
            idx[i]--;
        }
        float v = x[i] - interp->x0()[idx[i] + 1];
        c[i] = interp->a()[idx[i]]*v*v*v 
             + interp->b()[idx[i]]*v*v
             + interp->c()[idx[i]]*v
             + interp->d()[idx[i]];
        if(fabs(c[i]) > 1.0e-6) {
            grad[i] = -(3.0*interp->a()[idx[i]]*v*v
                      + 2.0*interp->b()[idx[i]]*v
                      +     interp->c()[idx[i]]) / (c[i]*c[i]);
        }
        else {
            grad[i] = 0.0f;
        }
    }
}

}; //namespace algorithm
}; //namespace proteus

