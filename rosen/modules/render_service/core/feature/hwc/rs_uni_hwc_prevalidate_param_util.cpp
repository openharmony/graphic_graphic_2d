/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_uni_hwc_prevalidate_param_util.h"

#include <securec.h>
#include <string>

#include "common/rs_common_hook.h"
#include "engine/rs_base_render_util.h"
#include "hwc_param.h"
#include "feature/hwc/hpae_offline/rs_offline_processor.h"
#include "platform/common/rs_log.h"
#include "system/rs_system_parameters.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcPrevalidateParamUtil"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATRIX_SIZE = 9;
} // namespace

const std::unordered_set<std::string> RSUniHwcPrevalidateParamUtil::VIDEO_LAYERS {
    "xcomponentIdSurface",
    "componentIdSurface",
    "SceneViewer Model totemweather0",
    "UnityPlayerSurface",
};

bool RSUniHwcPrevalidateParamUtil::isVcldEnabled_ = false;

void RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(bool isEnabled)
{
    isVcldEnabled_ = isEnabled;
}

bool RSUniHwcPrevalidateParamUtil::GetIsVcldEnabled()
{
    return isVcldEnabled_;
}

bool RSUniHwcPrevalidateParamUtil::IsYUVBufferFormat(const sptr<SurfaceBuffer>& buffer)
{
    if (!buffer) {
        return false;
    }
    auto format = buffer->GetFormat();
    if (format < GRAPHIC_PIXEL_FMT_YUV_422_I || format == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
        format > GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        return false;
    }
    return true;
}

bool RSUniHwcPrevalidateParamUtil::CheckIfDoArsrPre(const sptr<SurfaceBuffer>& buffer, const std::string& nodeName)
{
    if (!buffer) {
        return false;
    }
    if (IsYUVBufferFormat(buffer) || (VIDEO_LAYERS.count(nodeName) > 0)) {
        return true;
    }
    return false;
}

void RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(RSSurfaceRenderNode::SharedPtr node, RequestLayerInfo& info)
{
    if (!node || !node->GetRSSurfaceHandler()) {
        return;
    }
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    if (!RSSystemParameters::GetArsrPreEnabled() || !CheckIfDoArsrPre(buffer, node->GetName())) {
        return;
    }
    std::string bundleName = node->GetBundleName();
    auto hwcHmsAppConfigFromHgm = HWCParam::GetSourceTuningForHmsApp();
    auto hwcHmsAppIter = hwcHmsAppConfigFromHgm.find(bundleName);
    if (hwcHmsAppIter != hwcHmsAppConfigFromHgm.end() && hwcHmsAppIter->second == "1") {
        node->SetArsrTag(false);
    } else {
        info.perFrameParameters["ArsrDoEnhance"] = std::vector<int8_t> {1};
        node->SetArsrTag(true);
    }
}

bool RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType transform)
{
    if (transform > GRAPHIC_ROTATE_270) {
        transform = RSBaseRenderUtil::GetRotateTransform(transform);
    }
    return (transform == GRAPHIC_ROTATE_90 || transform == GRAPHIC_ROTATE_270);
}

void RSUniHwcPrevalidateParamUtil::ApplyCopybit(const sptr<SurfaceBuffer>& buffer,
    GraphicTransformType transform, RequestLayerInfo& info, const RSSurfaceRenderNode::SharedPtr node)
{
    if (!RSSystemParameters::GetIsCopybitSupported() || !buffer) {
        return;
    }
    if (IsYUVBufferFormat(buffer) && IsNeedDssRotate(transform)) {
        info.perFrameParameters["TryToDoCopybit"] = std::vector<int8_t> {1};
        if (node) {
            node->SetCopybitTag(true);
        }
    }
}

void RSUniHwcPrevalidateParamUtil::ApplyVcldParam(const RSVcldParam& vcldInfo, RequestLayerInfo& info)
{
    if (!isVcldEnabled_) {
        return;
    }
    std::vector<int8_t> valueBlob(sizeof(RSVcldParam));
    *reinterpret_cast<RSVcldParam*>(valueBlob.data()) = vcldInfo;
    info.perFrameParameters["VcldParam"] = valueBlob;
}

void RSUniHwcPrevalidateParamUtil::ApplySourceCropTuning(
    const sptr<SurfaceBuffer>& buffer, RequestLayerInfo& info)
{
    if (RsCommonHook::Instance().GetVideoSurfaceFlag() && IsYUVBufferFormat(buffer)) {
        info.perFrameParameters["SourceCropTuning"] = std::vector<int8_t> {1};
    } else {
        info.perFrameParameters["SourceCropTuning"] = std::vector<int8_t> {0};
    }
}

void RSUniHwcPrevalidateParamUtil::ApplyLayerLinearMatrix(
    const std::vector<float>& matrix, RequestLayerInfo& info)
{
    if (matrix.size() != MATRIX_SIZE) {
        return;
    }
    std::vector<int8_t> valueBlob(MATRIX_SIZE * sizeof(float));
    if (memcpy_s(valueBlob.data(), valueBlob.size(), matrix.data(), MATRIX_SIZE * sizeof(float)) == EOK) {
        info.perFrameParameters["LayerLinearMatrix"] = valueBlob;
    }
}

void RSUniHwcPrevalidateParamUtil::ProcessOfflineStrategy(
    const std::map<uint64_t, RequestCompositionType>& strategy)
{
    std::vector<uint64_t> offlineNodeIds;
    for (const auto& elem : strategy) {
        if (elem.second == RequestCompositionType::OFFLINE_DEVICE ||
            elem.second == RequestCompositionType::OFFLINE_VCLD_OFF) {
            offlineNodeIds.push_back(elem.first);
        }
    }
    RSOfflineProcessor::GetOfflineProcessor().CheckAndPostClearOfflineResourceTask(
        OfflineDeviceType::HPAE_OFFLINE_DEVICE, offlineNodeIds);
}
} // namespace Rosen
} // namespace OHOS
