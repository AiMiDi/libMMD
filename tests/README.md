# libMMD Unit Tests

基于 CTest 的单元/集成测试，不依赖 gtest 或 catch2。每个测试可执行文件都使用同一套轻量断言宏，按返回码与 CTest 集成。

## 测试项组织

`libMMD` 的测试现在按测试项拆成独立 runner，并补上了 CTest label，既可以按目标跑，也可以按标签过滤。

| 测试项 | CTest target | Labels | 说明 |
|---|---|---|---|
| Core | `mmd_model_test` | `core;unit` | 基础类型、模型骨架、管理器、坐标系等核心行为 |
| File IO | `mmd_model_io_test` | `io;parser` | PMX/VMD 文件读取、头信息、数据段与 buffer/path 一致性 |
| Animation | `mmd_model_animation_test` | `animation;integration` | 节点局部/全局变换、VMD 动画集成、C4D 矩阵提取、真实文件动画流程、Bezier |
| Regression | `mmd_model_regression_test` | `regression;optimization` | 节点分区、Morph/SyncPhysics、保存/回环、物理/IK 回归与优化一致性验证 |
| Benchmark | `mmd_model_benchmark_test` | `benchmark;performance` | 真实文件场景下的 `RigidInverse` / `LocalTransform` / `ChainPath` 性能输出 |
| File Base | `file_base_test` | `base;io;unit` | `MemoryReader` / `TextFileReader` |
| Path Util | `path_util_test` | `path;unit` | 路径处理工具 |
| File Round-trip | `file_roundtrip_test` | `io;roundtrip` | 读写回环一致性 |
| PMX Export Invariants | `pmx_export_invariants_test` | `io;roundtrip;export` | PMX 导出不变量、读写失败路径与 round-trip |
| Physics | `mmd_physics_test` | `integration;physics` | Bullet 世界、刚体、外部节点同步 |
| IK Solver | `mmd_ik_solver_test` | `ik;unit` | IK 求解器与 `IMMDNode` 适配测试 |
| VMD Interpolation | `vmd_interpolation_test` | `animation;interpolation` | VMD 插值实现与边界行为 |

## 从主仓运行

推荐直接用主仓预设，这会在 `dependency/` 子项目里打开 `libMMD` 的 CTest。

```bash
cmake --preset dev-windows-deps-test
cmake --build --preset cmt-deps-build --config Debug
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug --output-on-failure
```

## 定向运行

按测试项标签过滤：

```bash
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -L animation --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -L regression --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -L benchmark --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -L physics --output-on-failure
```

按具体 runner 过滤：

```bash
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -R mmd_model_io_test --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -R pmx_export_invariants_test --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -R mmd_model_benchmark_test --output-on-failure
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -R vmd_interpolation_test --output-on-failure
```

PMX 导出插件改动的常用 focused 验证组合：

```bash
cmake --build _build_msvc/cmt_deps/libMMD/tests --config Debug --target pmx_export_invariants_test
ctest --test-dir _build_msvc/cmt_deps/libMMD -C Debug -R "(pmx_export_invariants_test|mmd_ik_solver_test|mmd_physics_test)" --output-on-failure
```

## 单独构建 libMMD

如果只在 `dependency/libMMD` 内部调试，也可以直接配置子模块：

```bash
cmake -S . -B build -G "Ninja Multi-Config" ^
  -D LIBMMD_BULLET_ROOT="../../install" ^
  -D LIBMMD_ENABLE_TEST=ON

cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## 维护约定

- 新测试优先归到已有测试项，而不是继续向单个“大而全” runner 里堆。
- 如果一个测试只输出性能数字或吞吐统计，优先放进 `mmd_model_benchmark_test`。
- 如果一个测试同时校验正确性与回归行为，优先放进 `mmd_model_regression_test`。
- 新增 runner 时，记得同步更新：
  - `tests/CMakeLists.txt`
  - 本文档
  - 需要的话补充对应的 CTest label
