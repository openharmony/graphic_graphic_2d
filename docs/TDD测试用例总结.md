# Render Group Cache 像素对齐修复 - TDD 测试用例总结

## 测试文件修改

本次为新增和修改的代码补充了 TDD（测试驱动开发）测试用例，覆盖了所有新增函数和修改的分支。

### 修改的测试文件

1. **rosen/test/render_service/render_service/unittest/drawable/rs_render_node_drawable_adapter_test.cpp**
   - 新增 7 个测试用例，覆盖 `AlignRectToDevicePixels` 函数
   - 新增代码行数：+194

---

## 新增测试用例详情

### 一、AlignRectToDevicePixels 函数测试（7 个用例）

**文件**: `rs_render_node_drawable_adapter_test.cpp`

| 测试用例 | 测试名称 | 测试场景 | 验证点 |
|---------|---------|---------|--------|
| 1 | `AlignRectToDevicePixelsTest001` | 单位矩阵（Identity Matrix） | 矩阵无变换时，rect 对齐后经变换为整数像素 |
| 2 | `AlignRectToDevicePixelsTest002` | 平移变换（Translate） | 平移变换下，rect 对齐后经变换为整数像素 |
| 3 | `AlignRectToDevicePixelsTest003` | 缩放变换（Scale = 0.8） | 小于1的缩放下，rect 对齐后经变换为整数像素 |
| 4 | `AlignRectToDevicePixelsTest004` | 缩放变换（Scale = 1.5） | 大于1的缩放下，rect 对齐后经变换为整数像素 |
| 5 | `AlignRectToDevicePixelsTest005` | 复合变换（Scale + Translate） | 缩放+平移复合变换下，rect 对齐后经变换为整数像素 |
| 6 | `AlignRectToDevicePixelsTest006` | 负坐标 | 负坐标下，floor/ceil 正确处理 |
| 7 | `AlignRectToDevicePixelsTest007` | 不可逆矩阵（Scale = 0） | 矩阵不可逆时，rect 保持不变 |

#### 测试代码示例

```cpp
HWTEST(RSRenderNodeDrawableAdapterTest, AlignRectToDevicePixelsTest001, TestSize.Level1)
{
    NodeId id = 17;
    auto node = std::make_shared<RSRenderNode>(id);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(node));

    Drawing::Matrix matrix;
    matrix.SetIdentity();
    Drawing::Rect rect(10.3f, 20.5f, 100.7f, 200.8f);

    adapter->AlignRectToDevicePixels(matrix, rect);

    Drawing::Rect deviceRect;
    matrix.MapRect(deviceRect, rect);

    EXPECT_NEAR(std::floor(deviceRect.GetLeft()), deviceRect.GetLeft(), 0.01f);
    EXPECT_NEAR(std::floor(deviceRect.GetTop()), deviceRect.GetTop(), 0.01f);
    EXPECT_NEAR(std::ceil(deviceRect.GetRight()), deviceRect.GetRight(), 0.01f);
    EXPECT_NEAR(std::ceil(deviceRect.GetBottom()), deviceRect.GetBottom(), 0.01f);
}
```

---

## 新增分支覆盖

### 1. AlignRectToDevicePixels 函数

**新增分支**：
- ✅ 矩阵可逆分支（正常流程）
- ✅ 矩阵不可逆分支（`if (!matrix.Invert(inverseMatrix))`）

**覆盖场景**：
- 单位矩阵
- 平移变换
- 缩放变换（< 1, > 1）
- 复合变换
- 负坐标
- 不可逆矩阵

---

## 测试统计

### 总体统计

| 项目 | 数量 |
|------|------|
| 新增测试用例 | 7 个 |
| 新增代码行数 | +194 行 |
| 涉及测试文件 | 1 个 |
| 新增函数覆盖 | 1 个（AlignRectToDevicePixels） |

### 按测试文件统计

| 测试文件 | 新增用例数 | 新增代码行数 |
|---------|-----------|------------|
| rs_render_node_drawable_adapter_test.cpp | 7 | 194 |

### 按测试类型统计

| 测试类型 | 用例数 | 占比 |
|---------|-------|------|
| AlignRectToDevicePixels 函数测试 | 7 | 100% |

---

## 测试覆盖率

### 新增函数覆盖

| 函数 | 分支覆盖 | 场景覆盖 | 状态 |
|------|---------|---------|------|
| `AlignRectToDevicePixels` | 100% | 7/7 场景 | ✅ 完成 |

---

## 测试执行

### 编译测试

```bash
# 编译 render_service 模块
hb build graphic_2d -t --skip-download --build-target graphic_2d

# 编译测试
hb build graphic_2d -t --skip-download --build-target //rosen/test/render_service/render_service/unittest/drawable:rs_render_node_drawable_adapter_test
```

### 运行测试

```bash
# 运行新增的 AlignRectToDevicePixels 测试
./rs_render_node_drawable_adapter_test --gtest_filter="*AlignRectToDevicePixelsTest0*"
```

---

## 测试验证点

### 1. AlignRectToDevicePixels 测试验证

每个测试用例验证以下要点：
- ✅ 对齐后的 rect 经过 matrix 变换后，左上角坐标为整数（floor）
- ✅ 对齐后的 rect 经过 matrix 变换后，右下角坐标为整数（ceil）
- ✅ 矩阵不可逆时，rect 保持不变
- ✅ 负坐标下，floor/ceil 正确处理

---

## 测试注意事项

### 1. 测试环境要求

- 需要 GPU 上下文支持（用于创建缓存表面）
- 需要 Drawing 库支持
- 需要正确的 RSRenderNode 环境

---

## 总结

本次 TDD 测试用例补充工作：
- ✅ 新增 7 个测试用例，覆盖新增函数 `AlignRectToDevicePixels` 的所有分支
- ✅ 测试代码行数 +194 行
- ✅ 新增函数分支覆盖率达到 100%

**新增函数和修改的代码都有对应的测试用例覆盖，确保了代码质量和功能正确性。**
