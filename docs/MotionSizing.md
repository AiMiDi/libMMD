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

Both entry points also accept an optional final `ProgressCallback`. It runs on
the solver's calling thread and reports completed work within the current phase,
not an overall duration estimate. `total == 0` denotes indeterminate work;
`characterIndex` is one-based, with zero reserved for batch-wide phases. The host
must marshal UI updates to its own main thread. Observers can request cancellation
through the existing atomic flag; an observer exception fails the solve. The
callback is throttled by work units and does not modify motion results.

The private implementation has five responsibilities under `src/MotionSizing`:

| Directory | Responsibility |
| --- | --- |
| Analysis | Input checks, body ratios and compatible movement offsets |
| Pose | Hierarchy/append/IK evaluation, motion sampling and baking |
| Solvers | Stance, twist, arm/leg avoidance, contact and camera fitting |
| Pipeline | Stages, batches, cancellation and result assembly |
| Diagnostics | Residuals, targets and bounded diagnostic samples |

Private headers are not installed. Public types use libMMD PMX/VMD data and
Eigen; core code does not contain host scene objects. Solvers use double
precision internally and write VMD floats. On MSVC the sizing source
files explicitly retain SSE2, independently of the playback library's AVX2
option. This preserves the original solver's Eigen alignment and iterative
results. Tests that use private pose types match SSE2; the PMXNode append
reference translation unit and the private PMX IK bridge match the playback
library and exchange scalar arrays only. The bridge uses playback float
precision. Changing vectorization can alter contact solver convergence, so
such changes require separate numerical validation rather than being treated
as a transparent build optimization.

## Boundaries

Standard Japanese bone names are required. P1 movement retains the upstream
formula and historical interpolation rules; later pose stages use libMMD's
`InterpolateBoneKeys` playback convention. Rotation/translation append chains
are supported, including negative weights and local append. Offline evaluation
runs the shared `MMDIkSolver`, including PMX link limits, deform layers and VMD
IK enable tracks. It resets IK state for each sample and never writes the solved
IK rotations into authored FK channels. Bone morph evaluation, external parenting,
host controller overrides and dynamic physics are outside the offline evaluator;
physics-dependent post-physics IK therefore cannot match a simulated host scene.

Avoidance constrains wrist/elbow sample points against bone-following sphere,
box and capsule bodies. It does not guarantee mesh-level or whole-body freedom
from penetration. Hand contacts retain the source contact group's relative
landmark offsets, scaled by mean target/source palm size (forearm fallback).
They translate hands through arm joints while preserving incoming palm world
orientation and authored finger rotations. They do not synthesize finger curls
or force all nearby landmarks to coincide. Both single-character and batch
contact stages limit each local rotation correction to 30 degrees relative to
the incoming pose. Fixed-axis/append-driven wrists are left unchanged because
their palm orientation cannot be freely preserved. IK-controlled hand effectors
are also skipped rather than writing FK corrections that playback would replace.

`legAvoidance` is opt-in and independent of arm avoidance. It measures closest
distances between canonical left/right thigh and shin static capsules/spheres,
then adjusts the two standard foot IK goals in the horizontal plane. Each foot
is limited to eight percent of mean leg length; foot height, authored knee/hip
rotations and VMD IK switches are retained. Temporal regularization and a
nine-frame triangular filter act on corrections only. Filtered corrections are
backtracked if the frame's maximum penetration would exceed the input value or
ankle-to-IK-goal error would exceed the input error plus tolerance. Append-driven
and non-translatable foot controls are skipped with warnings.
Floor contact is rejected if it would undo this leg collision improvement or
increase IK target error; foot diagnostics measure the actual ankle position.
Residual diagnostics use capsule surfaces and include unresolved conflicts.
This does not model mesh thickness, noncanonical/absent leg bodies, thigh/torso
or scenery collisions, or guarantee foot locking and zero residual penetration.

With avoidance enabled, contact corrections are backtracked if they increase
penetration beyond the incoming depth or tolerance at wrists, elbows, or three
interior samples per arm segment. This guard does not resolve existing segment
penetrations and does not model skin thickness or cross-character collisions.
Conflicting goals remain explicitly unresolved instead of distorting the hand.
Contact detection is still a per-frame source-distance heuristic; it has no
surface contact classification or temporal contact-window stabilization.
Camera adaptation fits
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
