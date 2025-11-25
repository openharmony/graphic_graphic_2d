/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "effect/rs_render_effect_template.h"

#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_path.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, float value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, bool value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const Vector4f& value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const Vector3f& value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const Vector2f& value)
{
    geFilter.SetParam(desc, std::make_pair(value.x_, value.y_));
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, std::shared_ptr<RSNGRenderMaskBase> value)
{
    std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = value ? value->GenerateGEVisualEffect() : nullptr;
    std::shared_ptr<Drawing::GEShaderMask> geMask = geVisualEffect ? geVisualEffect->GenerateShaderMask() : nullptr;
    geFilter.SetParam(desc, geMask);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, std::shared_ptr<RSNGRenderShapeBase> value)
{
    std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = value ? value->GenerateGEVisualEffect() : nullptr;
    std::shared_ptr<Drawing::GEShaderShape> geShap = geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
    geFilter.SetParam(desc, geShap);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const std::vector<Vector2f>& value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, std::shared_ptr<Media::PixelMap> value)
{
    auto image = RSPixelMapUtil::ExtractDrawingImage(value);
    geFilter.SetParam(desc, image);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const std::vector<float>& value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const RRect& value)
{
    OHOS::Rosen::Drawing::GERRect geRRect(value.rect_.left_, value.rect_.top_,
                                          value.rect_.width_, value.rect_.height_,
                                          value.radius_->x_, value.radius_->y_);

    geFilter.SetParam(desc, geRRect);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, std::shared_ptr<Drawing::Image> value)
{
    geFilter.SetParam(desc, value);
}

void RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(Drawing::GEVisualEffect& geFilter,
    const std::string& desc, const Matrix3f& value)
{
    Matrix3f matrix = value;
    const auto matrixData = matrix.GetData();
    Drawing::Matrix drawingMatrix;
    drawingMatrix.SetMatrix(matrixData[Matrix3f::Index::SCALE_X], matrixData[Matrix3f::Index::SKEW_X],
                            matrixData[Matrix3f::Index::TRANS_X], matrixData[Matrix3f::Index::SKEW_X],
                            matrixData[Matrix3f::Index::SCALE_Y], matrixData[Matrix3f::Index::TRANS_Y],
                            matrixData[Matrix3f::Index::PERSP_0], matrixData[Matrix3f::Index::PERSP_1],
                            matrixData[Matrix3f::Index::PERSP_2]);
    geFilter.SetParam(desc, drawingMatrix);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, float value)
{
    hash = hashFunc_(&value, sizeof(value), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, bool value)
{
    hash = hashFunc_(&value, sizeof(value), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const Vector4f& value)
{
    hash = hashFunc_(&value.data_, Vector4f::DATA_SIZE, hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const Vector3f& value)
{
    hash = hashFunc_(&value.data_, Vector3f::DATA_SIZE, hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const Vector2f& value)
{
    hash = hashFunc_(&value.data_, Vector2f::DATA_SIZE, hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, std::shared_ptr<RSNGRenderMaskBase> value)
{
    if (!value) {
        return;
    }

    uint32_t maskHash = value->CalculateHash();
    hash = hashFunc_(&maskHash, sizeof(maskHash), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, std::shared_ptr<RSNGRenderShapeBase> value)
{
    if (!value) {
        return;
    }

    uint32_t shapeHash = value->CalculateHash();
    hash = hashFunc_(&shapeHash, sizeof(shapeHash), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const std::vector<Vector2f>& value)
{
    for (const auto& vec : value) {
        hash = hashFunc_(vec.data_, Vector2f::DATA_SIZE, hash);
    }
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, std::shared_ptr<Media::PixelMap> value)
{
    auto image = RSPixelMapUtil::ExtractDrawingImage(value);
    if (!image) {
        return;
    }

    auto imageUniqueID = image->GetUniqueID();
    hash = hashFunc_(&imageUniqueID, sizeof(imageUniqueID), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const std::vector<float>& value)
{
    for (size_t i = 0; i < value.size(); i++) {
        hash = hashFunc_(&value[i], sizeof(float), hash);
    }
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const RRect& value)
{
    hash = hashFunc_(&value, sizeof(RRect), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, std::shared_ptr<Drawing::Image> value)
{
    auto imageUniqueID = value->GetUniqueID();
    hash = hashFunc_(&imageUniqueID, sizeof(imageUniqueID), hash);
}

void RSNGRenderEffectHelper::CalculatePropTagHashImpl(uint32_t& hash, const Matrix3f& value)
{
    Matrix3f matrix = value;
    const auto matrixData = matrix.GetData();
    for (size_t i = 0; i < Matrix3f::MATRIX3_SIZE; i++) {
        hash = hashFunc_(&matrixData[i], sizeof(float), hash);
    }
}

std::shared_ptr<Drawing::GEVisualEffect> RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType type)
{
    return std::make_shared<Drawing::GEVisualEffect>(GetEffectTypeString(type), Drawing::DrawingPaintType::BRUSH);
}

void RSNGRenderEffectHelper::AppendToGEContainer(std::shared_ptr<Drawing::GEVisualEffectContainer>& ge,
    std::shared_ptr<Drawing::GEVisualEffect> geShader)
{
    if (!ge) {
        return;
    }
    ge->AddToChainedFilter(geShader);
}
} // namespace Rosen
} // namespace OHOS
