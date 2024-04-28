/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include <functional>
#include <string>

#include "common/rs_obj_abs_geometry.h"
#include "rs_base_render_util.h"
#include "rs_uni_render_util.h"
#include "pipeline/rs_uni_hwc_prevalidate_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
constexpr uint32_t ROTATION_360 = 360;
RSUniHwcPrevalidateUtil& RSUniHwcPrevalidateUtil::GetInstance()
{
    static RSUniHwcPrevalidateUtil instance;
    return instance;
}

RSUniHwcPrevalidateUtil::RSUniHwcPrevalidateUtil()
{
    preValidateHandle_ = dlopen("/chipset/lib64/chipsetsdk/libdss_enhance.z.so", RTLD_LAZY);
    if (preValidateHandle_ == nullptr) {
        RS_LOGW("[%{public}s_%{public}d]:load library failed, reason: %{public}s", __func__, __LINE__, dlerror());
        return;
    }
    preValidateFunc_ = reinterpret_cast<PreValidateFunc>(dlsym(preValidateHandle_, "RequestLayerStrategy"));
    if (preValidateFunc_ == nullptr) {
        RS_LOGW("[%{public}s_%{public}d]:load func failed, reason: %{public}s", __func__, __LINE__, dlerror());
        dlclose(preValidateHandle_);
    }
    RS_LOGI("[%{public}s_%{public}d]:load success", __func__, __LINE__);
    loadSuccess = true;
}

RSUniHwcPrevalidateUtil::~RSUniHwcPrevalidateUtil()
{
    if (preValidateHandle_) {
        dlclose(preValidateHandle_);
    }
}

bool RSUniHwcPrevalidateUtil::IsLoadSuccess() const
{
    return loadSuccess;
}

bool RSUniHwcPrevalidateUtil::PreValidate(
    ScreenId id, std::vector<RequestLayerInfo> infos, std::map<uint64_t, RequestCompositionType> &strategy)
{
    if (!preValidateFunc_) {
        return false;
    }
    int32_t ret = preValidateFunc_(id, infos, strategy);
    return ret == 0;
}

RequestLayerInfo RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
    RSSurfaceRenderNode::SharedPtr node, GraphicTransformType transform, uint32_t fps)
{
    RequestLayerInfo info;
    if (!node || !node->GetConsumer() || !node->GetBuffer()) {
        return info;
    }
    info.id = node->GetId();
    auto src = node->GetSrcRect();
    info.srcRect = {src.left_, src.top_, src.width_, src.height_};
    auto dst = node->GetDstRect();
    info.dstRect = {dst.left_, dst.top_, dst.width_, dst.height_};
    info.zOrder = node->GetGlobalZOrder();
    info.usage = node->GetBuffer()->GetUsage();
    info.format = node->GetBuffer()->GetFormat();
    info.fps = fps;
    info.transform = static_cast<int>(transform);
    return info;
}

RequestLayerInfo RSUniHwcPrevalidateUtil::CreateDisplayNodeLayerInfo(
    uint32_t zorder, RSDisplayRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps)
{
    RequestLayerInfo info;
    if (!node || !node->GetConsumer() || !node->GetBuffer()) {
        return info;
    }
    info.id = node->GetId();
    info.srcRect = {0, 0, node->GetBuffer()->GetSurfaceBufferWidth(), node->GetBuffer()->GetSurfaceBufferHeight()};
    info.dstRect = {0, 0, screenInfo.GetRotatedPhyWidth(), screenInfo.GetRotatedPhyHeight()};
    info.zOrder = zorder;
    info.usage = node->GetBuffer()->GetUsage() | USAGE_UNI_LAYER;
    info.format = node->GetBuffer()->GetFormat();
    info.fps = fps;
    LayerRotate(info, node->GetConsumer(), screenInfo);
    return info;
}


RequestLayerInfo RSUniHwcPrevalidateUtil::CreateRCDLayerInfo(
    RSRcdSurfaceRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps)
{
    RequestLayerInfo info;
    if (!node || !node->GetConsumer() || !node->GetBuffer()) {
        return info;
    }
    
    info.id = node->GetId();
    auto src = node->GetSrcRect();
    info.srcRect = {src.left_, src.top_, src.width_, src.height_};
    auto dst = node->GetDstRect();
    info.dstRect.x = static_cast<uint32_t>(static_cast<float>(dst.left_) * screenInfo.GetRogWidthRatio());
    info.dstRect.y = static_cast<uint32_t>(static_cast<float>(dst.top_) * screenInfo.GetRogHeightRatio());
    info.dstRect.w = static_cast<uint32_t>(static_cast<float>(dst.width_) * screenInfo.GetRogWidthRatio());
    info.dstRect.h = static_cast<uint32_t>(static_cast<float>(dst.height_) * screenInfo.GetRogHeightRatio());
    info.zOrder = node->GetGlobalZOrder();
    info.usage = node->GetBuffer()->GetUsage();
    info.format = node->GetBuffer()->GetFormat();
    info.fps = fps;
    CopyCldInfo(node->GetCldInfo(), info);
    LayerRotate(info, node->GetConsumer(), screenInfo);
    return info;
}

void RSUniHwcPrevalidateUtil::LayerRotate(
    RequestLayerInfo& info, const sptr<IConsumerSurface>& surface, const ScreenInfo &screenInfo)
{
    if (!surface) {
        return;
    }
    const auto screenWidth = static_cast<int32_t>(screenInfo.width);
    const auto screenHeight = static_cast<int32_t>(screenInfo.height);
    const auto screenRotation = screenInfo.rotation;
    const auto rect = info.dstRect;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_90: {
            info.dstRect = {rect.y, screenHeight - rect.x - rect.w, rect.h, rect.w};
            break;
        }
        case ScreenRotation::ROTATION_180: {
            info.dstRect = {screenWidth - rect.x - rect.w, screenHeight - rect.y - rect.h, rect.w, rect.h};
            break;
        }
        case ScreenRotation::ROTATION_270: {
            info.dstRect = {screenWidth - rect.y - rect.h, rect.x, rect.h, rect.w};
            break;
        }
        default: {
            break;
        }
    }
    int totalRotation = (RotateEnumToInt(screenRotation) + RSBaseRenderUtil::RotateEnumToInt(
        RSBaseRenderUtil::GetRotateTransform(surface->GetTransform()))) % ROTATION_360;
    GraphicTransformType rotateEnum = RSBaseRenderUtil::RotateEnumToInt(totalRotation,
        RSBaseRenderUtil::GetFlipTransform(surface->GetTransform()));
    info.transform = rotateEnum;
}

void RSUniHwcPrevalidateUtil::CopyCldInfo(CldInfo src, RequestLayerInfo& info)
{
    info.cldInfo = new CldInfo();
    info.cldInfo->cldDataOffset = src.cldDataOffset;
    info.cldInfo->cldSize = src.cldSize;
    info.cldInfo->cldWidth = src.cldWidth;
    info.cldInfo->cldHeight = src.cldHeight;
    info.cldInfo->cldStride = src.cldStride;
    info.cldInfo->exWidth = src.exWidth;
    info.cldInfo->exHeight = src.exHeight;
    info.cldInfo->baseColor = src.baseColor;
}
} //Rosen
} //OHOS