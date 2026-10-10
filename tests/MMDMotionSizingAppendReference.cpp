#include "libMMD/Model/MMD/PMXModel.h"

// Keep PMXNode in a translation unit compiled with libMMD's ISA/alignment.
// Only scalar arrays cross this test boundary; the sizing library's private
// double-precision Eigen types are not part of the libMMD node ABI.
void SizingAppendReference(double rotations[4][4], double translations[4][3])
{
    libmmd::PMXNode nodes[4];
    for (int i = 0; i < 4; ++i)
    {
        nodes[i].BeginUpdateTransform();
        nodes[i].SetAnimationRotate(Eigen::Quaternionf(Eigen::AngleAxisf(.2f * (i + 1), Eigen::Vector3f(1, 2, 3).normalized())));
        nodes[i].SetTranslate(Eigen::Vector3f(.3f * i, -.2f * i, .1f * i));
        if (i < 3)
        {
            nodes[i].SetAppendNode(&nodes[i + 1]);
            nodes[i].EnableAppendRotate(true); nodes[i].EnableAppendTranslate(true);
            nodes[i].EnableAppendLocal(i == 1);
            nodes[i].SetAppendWeight(i == 0 ? -1.25f : .5f);
        }
    }
    for (int i = 3; i >= 0; --i)
    {
        nodes[i].UpdateAppendTransform();
        for (int j = 0; j < 4; ++j) rotations[i][j] = nodes[i].GetAppendRotate().coeffs()[j];
        for (int j = 0; j < 3; ++j) translations[i][j] = nodes[i].GetAppendTranslate()[j];
    }
}
