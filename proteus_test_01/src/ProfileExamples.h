#ifndef _DEF_PROTEUS_TESTS_PROFILE_EXAMPLES_H_
#define _DEF_PROTEUS_TESTS_PROFILE_EXAMPLES_H_

#include <rtac_base/types/common.h>
#include <rtac_base/interpolation.h>

namespace proteus { namespace algorithm {

class ProfileExamples : public rtac::algorithm::Interpolator<float>
{
    public:
    
    using Interpolator = rtac::algorithm::Interpolator<float>;
    using Vector       = Interpolator::Vector;
    using Indexes      = Interpolator::Indexes;

    static ProfileExamples dickins();
    static ProfileExamples dickins_linear();
    static ProfileExamples arctic();
    static ProfileExamples munk();

    ProfileExamples(const Vector& x0, const Vector& y0);

    void interpolate(const Vector& x, Vector& c, Vector& grad);
};

}; //namespace algorithm
}; //namespace proteus


#endif //_DEF_PROTEUS_TESTS_PROFILE_EXAMPLES_H_
