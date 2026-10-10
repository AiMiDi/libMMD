# Motion sizing

`libMMD/Model/MMD/MMDMotionSizing.h` provides the C++17 API in
`libmmd::sizing`. Link the existing `libMMD` target; there is no separate sizing
library and no Cinema 4D SDK or Python runtime dependency.

```cpp
#include "libMMD/Model/MMD/MMDMotionSizing.h"

libmmd::sizing::Options options;
options.stance = true;
options.twist = true;
const auto result = libmmd::sizing::Run(sourcePmx, targetPmx, motionVmd, options);
if (result.success)
{
    const auto& adjusted = result.stages[
        static_cast<size_t>(libmmd::sizing::Stage::Contact)];
    libmmd::WriteVMDFile(&adjusted, "adjusted.vmd");
}
```

All positions and distances use PMX/VMD units. The caller owns scene scale,
document lifetime, threading and publication of results. Inputs remain
unchanged. `Run` and `RunBatch` accept an optional `const std::atomic_bool*`
cancellation flag; its lifetime must cover the call. Inspect `success`,
`cancelled`, `error` and `analysis` before using results. Stage output includes
Original, Scale, Offset, Stance, Twist, Avoidance, Contact and MultiCharacter.
Disabled stages copy the preceding stage. `RunBatch` accepts per-character
options and optional camera adaptation in a common MMD coordinate system.

The private implementation has five responsibilities under `src/MotionSizing`:

| Directory | Responsibility |
| --- | --- |
| Analysis | Input checks, body ratios and compatible movement offsets |
| Pose | Hierarchy/append evaluation, motion sampling and baking |
| Solvers | Stance, twist, rigid-shape avoidance, contact and camera fitting |
| Pipeline | Stages, batches, cancellation and result assembly |
| Diagnostics | Residuals, targets and bounded diagnostic samples |

Private headers are not installed. Public types use libMMD PMX/VMD data and
Eigen; core code does not contain host scene objects. Solvers use double
precision internally and write VMD floats. On MSVC the seven sizing source
files explicitly retain SSE2, independently of the playback library's AVX2
option. This preserves the original solver's Eigen alignment and iterative
results. Tests that use private pose types match SSE2; the PMXNode append
reference translation unit matches the playback library and returns scalar
arrays only. Changing vectorization can alter contact solver convergence, so
such changes require separate numerical validation rather than being treated
as a transparent build optimization.

## Boundaries

Standard Japanese bone names are required. P1 movement retains the upstream
formula and historical interpolation rules; later pose stages use libMMD's
`InterpolateBoneKeys` playback convention. Rotation/translation append chains
are supported, including negative weights and local append. Complete PMX IK,
bone morph evaluation, external parenting and dynamic physics are outside the
offline pose evaluator.

Avoidance constrains wrist/elbow sample points against bone-following sphere,
box and capsule bodies. It does not guarantee mesh-level or whole-body freedom
from penetration. Contacts report unreachable and budget-limited residuals.
Later stages may conflict with earlier constraints. Camera adaptation fits
common skeleton landmarks; it does not reproduce every upstream heuristic.

## Build and test

Configure libMMD normally with Bullet and Eigen available. For example:

```sh
cmake -S . -B build -DLIBMMD_BULLET_ROOT=/path/to/bullet-install -DLIBMMD_ENABLE_TEST=ON
cmake --build build --config Release --target mmd_motion_sizing_test mmd_motion_sizing_advanced_test
ctest --test-dir build -C Release -R mmd_motion_sizing --output-on-failure
```

The tests and synthetic PMX/VMD/reference fixtures are owned by this repository
under `tests/`. No parent C4D checkout is needed. The basic test covers movement
reference comparisons, malformed inputs, cancellation and preservation of
unmodified channels. The advanced test covers append evaluation, twist,
collision/contact residuals, batching and camera invariants.

`mmd_motion_sizing_assets_probe` is an opt-in target, excluded from the default
build and CTest. With Visual Studio, build it from `build/tests` explicitly.
It takes a UTF-8 four-line manifest (source PMX, target PMX, motion VMD, camera
VMD), an output directory and optional `--batch`. External model assets are not
distributed with this library.

Movement formulae are adapted from miu200521358/vmd_sizing revision
`e5c30358696f688c96544e3af33ea9871961487d`; its MIT notice is preserved in
`license/vmd_sizing-MIT.txt` and installed to `share/libMMD/licenses`.
