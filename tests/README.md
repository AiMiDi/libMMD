# libMMD Unit Tests

基于 CTest 的单元测试，不依赖任何外部测试框架。

## 构建与运行

```bash
# 1. 配置（在 libMMD 根目录）
cd build
cmake .. -G "Ninja Multi-Config" \
  -D LIBMMD_BULLET_ROOT="../../install" \
  -D LIBMMD_ENABLE_TEST=on

# 2. 编译
cmake --build . --config Debug --target mmd_model_test

# 3. 运行
ctest --test-dir . -C Debug -V
```

## 测试框架

使用 `TEST_ASSERT` / `TEST_ASSERT_EQ` / `TEST_ASSERT_FLOAT_EQ` 三个宏实现断言，
测试可执行文件以返回码 `EXIT_SUCCESS`（全部通过）或 `EXIT_FAILURE`（有失败）与 CTest 集成。

## 测试用例一览

### MMDNode（12 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `MMDNode_DefaultConstruction` | 默认构造后名称为空、索引为 0、IK 关闭、平移 (0,0,0)、缩放 (1,1,1)、旋转为单位四元数、父/子/兄弟指针均为 null |
| `MMDNode_SetName` | `SetName` / `GetName` 正确读写 |
| `MMDNode_SetIndex` | `SetIndex` / `GetIndex` 正确读写 |
| `MMDNode_EnableIK` | `EnableIK(true/false)` 与 `IsIK()` 一致 |
| `MMDNode_TranslateRotateScale` | `SetTranslate` / `SetRotate` / `SetScale` 及对应 getter 正确 |
| `MMDNode_AnimationTranslateRotate` | 动画平移 / 旋转的 set/get |
| `MMDNode_AnimateCombined` | `AnimateTranslate()` 返回 translate + animTranslate |
| `MMDNode_IKRotate` | IK 旋转的 set/get |
| `MMDNode_ParentChild` | `AddChild` 后子节点的 `GetParent` 正确，父节点的 `GetChild` 非空 |
| `MMDNode_SaveLoadInitialTRS` | `SaveInitialTRS` / `LoadInitialTRS` / `GetInitial*` 生命周期 |
| `MMDNode_SaveLoadBaseAnimation` | `SaveBaseAnimation` / `LoadBaseAnimation` / `ClearBaseAnimation` 生命周期 |
| `MMDNode_LocalGlobalTransform` | 局部 / 全局变换矩阵的 set/get |

### MMDMorph（3 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `MMDMorph_DefaultConstruction` | 默认名称为空、权重为 0 |
| `MMDMorph_SetNameAndWeight` | 名称和权重的 set/get |
| `MMDMorph_SaveLoadBaseAnimation` | Save → 修改 → Load 恢复 → Clear 重置 |

### MMDIkSolver（5 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `MMDIkSolver_DefaultConstruction` | IK 节点 / 目标节点为 null、名称为空、默认启用 |
| `MMDIkSolver_SetNodes` | 绑定 IK 节点和目标节点后 getter 正确，`GetName` 返回 IK 节点名称 |
| `MMDIkSolver_Enable` | `Enable(true/false)` 与 `Enabled()` 一致 |
| `MMDIkSolver_SaveLoadBaseAnimation` | Save → Enable(true) → Load 恢复 → Clear 重置为 true |
| `MMDIkSolver_SetParameters` | `SetIterateCount` / `SetLimitAngle` 不崩溃 |

### MMDMaterial（2 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `MMDMaterial_DefaultConstruction` | 全量默认值：diffuse=(1,1,1), alpha=1, specular=(0,0,0), specularPower=1, ambient=(0.2,0.2,0.2), edgeFlag=0, edgeSize=0, 纹理路径为空, bothFace=false, groundShadow/shadowCaster/shadowReceiver=true, MulFactor=1, AddFactor=0 |
| `MMDMaterial_SetFields` | 各字段赋值后读取正确 |

### MMDSubMesh（2 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `SubMeshDefaultConstruction` | 默认 beginIndex=0, vertexCount=0, materialID=0 |
| `SubMeshParameterizedConstruction` | 参数化构造 (10, 200, 3) 正确 |

### PMXModel（13 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `PMXModel_InheritsFromMMDModel` | `PMXModel*` 可隐式转换为 `MMDModel*` |
| `PMXModel_ManagerPointersAreNonNull` | NodeManager / IKManager / MorphManager / PhysicsManager 非空 |
| `PMXModel_InitialCountsAreZero` | 各 Manager 计数和材质/子网格数均为 0 |
| `PMXModel_InitialModelNameIsEmpty` | 模型名、英文名、注释、英文注释均为空 |
| `PMXModel_NodeManager_FindReturnsNPosForEmpty` | 空模型按名查找返回 NPos |
| `PMXModel_NodeManager_GetByNameReturnsNullForEmpty` | 空模型按名获取返回 nullptr |
| `PMXModel_IKManager_FindReturnsNPosForEmpty` | 同上，IK 版 |
| `PMXModel_IKManager_GetByNameReturnsNullForEmpty` | 同上 |
| `PMXModel_MorphManager_FindReturnsNPosForEmpty` | 同上，Morph 版 |
| `PMXModel_MorphManager_GetByNameReturnsNullForEmpty` | 同上 |
| `PMXModel_LoadInvalidPathReturnsFalse` | 不存在的文件路径返回 false |
| `PMXModel_DestroyOnEmptyModelIsSafe` | 空模型调用 Destroy 不崩溃 |
| `PMXModel_VMDAnimation_CreateWithModel` | VMDAnimation::Create 成功，初始关键帧数均为 0 |

### PMDModel（9 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `PMDModel_InheritsFromMMDModel` | `PMDModel*` 可隐式转换为 `MMDModel*` |
| `PMDModel_ManagerPointersAreNonNull` | 同 PMXModel |
| `PMDModel_InitialCountsAreZero` | 同 PMXModel |
| `PMDModel_InitialModelNameIsEmpty` | 同 PMXModel |
| `PMDModel_NodeManager_FindReturnsNPosForEmpty` | 同 PMXModel |
| `PMDModel_NodeManager_GetByNameReturnsNullForEmpty` | 同 PMXModel |
| `PMDModel_LoadInvalidPathReturnsFalse` | 不存在的文件路径返回 false |
| `PMDModel_DestroyOnEmptyModelIsSafe` | 空模型调用 Destroy 不崩溃 |
| `PMDModel_VMDAnimation_CreateWithPMDModel` | VMDAnimation::Create 成功 |

### MMDPhysicsManager（2 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `PhysicsManager_CreateSucceeds` | Create 返回 true，GetMMDPhysics 非空 |
| `PhysicsManager_InitialListsEmpty` | 刚骨体和关节列表初始为空 |

### Polymorphism（3 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `Polymorphism_PMXModelAsMMDModel` | `shared_ptr<MMDModel>` 持有 PMXModel 后接口可用 |
| `Polymorphism_PMDModelAsMMDModel` | 同上，PMDModel 版 |
| `Polymorphism_VMDAnimationAcceptsBothModelTypes` | VMDAnimation::Create 接受 PMXModel 和 PMDModel 的 `shared_ptr<MMDModel>` |

### VMDAnimation（3 个测试）

| 测试函数 | 验证内容 |
|---|---|
| `VMDAnimation_DefaultConstruction` | 默认构造后各计数为 0 |
| `VMDAnimation_DestroyWithoutCreate` | 未 Create 直接 Destroy 不崩溃 |
| `VMDAnimation_CreateAndDestroy` | Create → Destroy 完整生命周期 |

## 统计

- **测试函数**：51 个
- **断言总数**：188 个
- **覆盖类**：MMDNode, MMDMorph, MMDIkSolver, MMDMaterial, MMDSubMesh, PMXModel, PMDModel, MMDPhysicsManager, VMDAnimation
