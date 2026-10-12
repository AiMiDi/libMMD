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
from penetration. After moving the arm, avoidance counter-rotates writable
wrists toward their incoming palm world orientations, with a 30-degree wrist
correction budget. This also checks every arm's sampled clearance because a
wrist-following body can sweep into the opposite arm. Partial compensation is
retained when the full orientation is unsafe; fingers are not articulated to
make the error smaller. Append-driven, fixed-axis and nonrotatable wrist tracks
are not newly baked or replaced by this compensation. This wrist budget is not
a total arm-avoidance rotation budget.
Hand contacts retain source landmark-pair offsets, scaled
by the mean target/source palm-size ratio of each pair (forearm fallback).
An anchored weighted graph reconciles multiple pairs. A weak new connection
does not abruptly merge groups or move an existing strong group's centroid;
inconsistent pairs with different palm scales are resolved by weighted least
squares rather than forcing one group-wide scale.
Landmark confidence also weights shared-wrist targets and their acceptance
error, including floor blends. A weak fingertip entering detection therefore
cannot dilute another strong contact on the same hand through an equal vote.
They translate hands through arm joints while preserving incoming palm world
orientation and authored finger rotations. They do not synthesize finger curls
or force all nearby landmarks to coincide. Both single-character and batch
contact stages limit each local rotation correction to 30 degrees relative to
the incoming pose. If a full hand step is rejected, the accepted smaller step
is refined inside the rejected/accepted bracket using a fixed number of
bisections. Every candidate retains the pose, collision and error guards and
cannot increase the residual of the accepted step. This avoids a discrete
half-step/full-step switch when palm compensation approaches its 30-degree
budget; it does not filter or freeze authored motion across frames.
Fixed-axis/append-driven wrists are left unchanged because
their palm orientation cannot be freely preserved. IK-controlled hand effectors
are also skipped rather than writing FK corrections that playback would replace.

`legAvoidance` is opt-in and independent of arm avoidance. It measures closest
distances between canonical left/right thigh and shin static capsules/spheres,
then adjusts the two standard foot IK goals in the horizontal plane. Each foot
is limited to eight percent of mean leg length; foot height, authored knee/hip
rotations and VMD IK switches are retained. Temporal regularization and a
nine-frame triangular filter act on corrections only. Every candidate protects
each collision pair as well as the maximum penetration and ankle-to-IK-goal
error. When the local solve stalls, a bounded set of horizontal directions is
tested with the same guards. Filtered, raw and previous accepted corrections
are compared without changing the authored trajectory. After the first frame,
candidate correction changes are limited to one quarter of the travel budget;
larger offsets also need a safe release path through up to four incoming frames.
The exact per-foot release step remains available as a fallback. If zero would
exceed the step limit, a validated release is kept as the initial fallback even
when zero has a lower scalar objective. Other candidates still need to improve
that fallback and pass the collision and IK guards. A pair already below
tolerance may have a larger squared residual during release; continuity does
not justify exceeding any original spatial guard. IK switches
still take effect immediately. These bounded checks reduce branch jumps but do
not guarantee continuity for every possible abrupt pose or rig transition; a
spatially unsafe correction is rejected even when that leaves residual crossing.
Append-driven
and non-translatable foot controls are skipped with warnings.
Floor contact backtracks through bounded smaller displacements if the full
step would undo this leg collision improvement or increase IK target error.
After finding a safe floor step, bounded bisection refines it without increasing
the actual ankle-to-floor-target residual of the accepted candidate. Every
candidate uses the original guards; if none passes the input is retained.
This removes discrete quarter/half-step artifacts inside a stable IK branch.
A second floor pass limits changes in the added vertical correction and looks
ahead through four frames for a safe release. The per-frame limit is the lesser
of two percent of target leg length and one quarter of the source contact
distance scaled by the leg-length ratio. With zero contact distance, only the
leg-length bound is used. The pass reduces correction magnitude; it does not
filter the authored center trajectory or horizontal foot channels. Inactive
feet and disabled IK retain their input channels. Both foot IK errors and each
leg collision pair are checked against the incoming pose, using the same
spatial tolerances. Float-channel rounding is included only in the temporal
step bound. Diagnostics are updated to the final ankle pose.
Anticipating a lift can increase the floor residual by releasing the correction
earlier. A disconnected safe IK interval can prevent a continuous release near
a straight leg. In that case, the guarded spatial correction is retained and
a warning reports the continuity limitation; the pass does not discard every
floor correction to hide the jump. These bounded checks do not guarantee
continuity for every abrupt pose, rig transition or conflicting two-foot goal.
Foot diagnostics measure the actual ankle position.
Residual diagnostics use capsule surfaces and include unresolved conflicts.
This does not model mesh thickness, noncanonical/absent leg bodies, thigh/torso
or scenery collisions, or guarantee foot locking and zero residual penetration.

With avoidance enabled, contact corrections are backtracked if they increase
penetration beyond the incoming depth or tolerance at wrists, elbows, or three
interior samples per arm segment. This guard does not resolve existing segment
penetrations and does not model skin thickness or cross-character collisions.
Conflicting goals remain explicitly unresolved instead of distorting the hand.
Contact detection is still a per-frame source-distance heuristic. Hand-pair
and floor influence fades with smoothstep through the last 20% of the contact
distance, retaining full influence below 80%. This reduces threshold entry,
exit and jitter discontinuities without filtering authored motion. A zero
contact distance accepts exact coincidence only. Floor blending retains the
collision and IK-error rejection guards. It has no surface contact
classification or persistent contact-window classification, and cannot guarantee
velocity continuity when a collision or IK-error guard rejects a conflicting correction.
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
