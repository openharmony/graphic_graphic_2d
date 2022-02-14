/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_hardware_processor.h"

#include <iterator>

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"

#include "common/rs_obj_abs_geometry.h"
namespace OHOS {
namespace Rosen {

RSHardwareProcessor::RSHardwareProcessor() {}

RSHardwareProcessor::~RSHardwareProcessor() {}

void RSHardwareProcessor::Init(ScreenId id)
{
    backend_ = HdiBackend::GetInstance();
    backend_->RegPrepareComplete(std::bind(&RSHardwareProcessor::Redraw, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3), nullptr);
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        ROSEN_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    rotation_ = screenManager_->GetRotation(id);
    output_ = screenManager_->GetOutput(id);
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::Init output_ is nullptr");
        return;
    }
    screenManager_->GetScreenActiveMode(id, curScreenInfo_);
    ROSEN_LOGI("RSHardwareProcessor::Init screen w:%{public}d, w:%{public}d",
        curScreenInfo_.GetScreenWidth(), curScreenInfo_.GetScreenHeight());
    IRect damageRect;
    damageRect.x = 0;
    damageRect.y = 0;
    damageRect.w = curScreenInfo_.GetScreenWidth();
    damageRect.h = curScreenInfo_.GetScreenHeight();
    output_->SetOutputDamage(1, damageRect);
}

void RSHardwareProcessor::PostProcess()
{
    if (output_ == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::PostProcess output is nullptr");
        return;
    }
    if (rotation_ != ScreenRotation::ROTATION_0) {
        OnRotate();
    }
    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
    }
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface start node id:%llu available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    if (node.GetRenderProperties().GetBoundsPositionX() >= curScreenInfo_.GetScreenWidth() ||
        node.GetRenderProperties().GetBoundsPositionY() >= curScreenInfo_.GetScreenHeight()) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface this node:%llu no need to composite", node.GetId());
        return;
    }
    OHOS::sptr<SurfaceBuffer> cbuffer;
    RSProcessor::SpecialTask task = [] () -> void{};
    bool ret = ConsumeAndUpdateBuffer(node, task, cbuffer);
    if (!ret) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface consume buffer fail");
        return;
    }
    if (node.IsBufferAvailable() == false) {
        // Only ipc for one time.
        ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface id = %llu Notify RT buffer available", node.GetId());
        node.NotifyBufferAvailable(true);
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }
    ComposeInfo info = {
        .srcRect = {
            .x = 0,
            .y = 0,
            .w = node.GetBuffer()->GetSurfaceBufferWidth(),
            .h = node.GetBuffer()->GetSurfaceBufferHeight(),
        },
        .dstRect = {
            .x = geoPtr->GetAbsRect().left_,
            .y = geoPtr->GetAbsRect().top_,
            .w = geoPtr->GetAbsRect().width_,
            .h = geoPtr->GetAbsRect().height_,
        },
        .zOrder = node.GetGlobalZOrder(),
        .alpha = {
            .enGlobalAlpha = true,
            .gAlpha = node.GetAlpha() * node.GetRenderProperties().GetAlpha() * 255,
        },
        .buffer = node.GetBuffer(),
        .fence = node.GetFence(),
        .preBuffer = node.GetPreBuffer(),
        .preFence = node.GetPreFence(),
        .blendType = node.GetBlendType(),
    };
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    CalculateInfo(transitionProperties, info, node);
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode id:%llu name:[%s] dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        node.GetBuffer()->GetWidth(), node.GetBuffer()->GetHeight(), node.GetBuffer()->GetSurfaceBufferWidth(),
        node.GetBuffer()->GetSurfaceBufferHeight(), node.GetBuffer().GetRefPtr(),
        node.GetRenderProperties().GetPositionZ(), info.zOrder, info.blendType);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info, &node);
}

void RSHardwareProcessor::CalculateInfo(const std::unique_ptr<RSTransitionProperties>& transitionProperties,
    ComposeInfo& info, RSSurfaceRenderNode& node)
{
    if (!transitionProperties) {
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }
    auto delx = (1 - transitionProperties->GetScale().x_) * geoPtr->GetAbsRect().width_ / 2;
    auto dely = (1 - transitionProperties->GetScale().y_) * geoPtr->GetAbsRect().height_ / 2;
    info.dstRect = {
        .x = geoPtr->GetAbsRect().left_ + transitionProperties->GetTranslate().x_ + delx,
        .y = geoPtr->GetAbsRect().top_ + transitionProperties->GetTranslate().y_ + dely,
        .w = geoPtr->GetAbsRect().width_ * transitionProperties->GetScale().x_,
        .h = geoPtr->GetAbsRect().height_ * transitionProperties->GetScale().y_,
    };
    info.alpha = {
        .enGlobalAlpha = true,
        .gAlpha = node.GetAlpha() * node.GetRenderProperties().GetAlpha() * transitionProperties->GetAlpha() * 255,
    };
}

void RSHardwareProcessor::Redraw(sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        ROSEN_LOGI("RsDebug RSHardwareProcessor::Redraw no need to flush frame buffer");
        return;
    }

    if (surface == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: surface is null.");
        return;
    }
    ROSEN_LOGI("RsDebug RSHardwareProcessor::Redraw flush frame buffer start");
    BufferRequestConfig requestConfig = {
        .width = curScreenInfo_.GetScreenWidth(),
        .height = curScreenInfo_.GetScreenHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,      // [TODO] different soc need different format
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
        .timeout = 0,
    };
    auto canvas = CreateCanvas(surface, requestConfig);
    if (canvas == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: canvas is null.");
        return;
    }
    std::vector<LayerInfoPtr>::const_reverse_iterator iter = param.layers.rbegin();
    for (; iter != param.layers.rend(); ++iter) {
        ROSEN_LOGD("RsDebug RSHardwareProcessor::Redraw layer composition Type:%d", (*iter)->GetCompositionType());
        if ((*iter) == nullptr || (*iter)->GetCompositionType() == CompositionType::COMPOSITION_DEVICE) {
            continue;
        }
        ROSEN_LOGE("RsDebug RSHardwareProcessor::Redraw layer [%d %d %d %d]", (*iter)->GetLayerSize().x,
            (*iter)->GetLayerSize().y, (*iter)->GetLayerSize().w, (*iter)->GetLayerSize().h);
        RsRenderServiceUtil::DrawBuffer(canvas.get(), (*iter)->GetBuffer(),
            *static_cast<RSSurfaceRenderNode *>((*iter)->GetLayerAdditionalInfo()));
    }
    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = curScreenInfo_.GetScreenWidth(),
            .h = curScreenInfo_.GetScreenHeight(),
        },
    };
    FlushBuffer(surface, flushConfig);
}

void RSHardwareProcessor::OnRotate()
{
    int32_t width = curScreenInfo_.GetScreenWidth();
    int32_t height = curScreenInfo_.GetScreenHeight();
    for (auto& layer: layers_) {
        IRect rect = layer->GetLayerSize();
        ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate Before Rotate layer size [%d %d %d %d]",
            rect.x, rect.y, rect.w, rect.h);
        switch (rotation_) {
            case ScreenRotation::ROTATION_90: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 90.");
                layer->SetLayerSize({width - rect.y - rect.h, rect.x, rect.h, rect.w});
                layer->SetTransform(TransformType::ROTATE_90);
                break;
            }
            case ScreenRotation::ROTATION_180: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 180.");
                layer->SetLayerSize({width - rect.x - rect.w, height - rect.y - rect.h, rect.w, rect.h});
                layer->SetTransform(TransformType::ROTATE_180);
                break;
            }
            case ScreenRotation::ROTATION_270: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 270.");
                layer->SetLayerSize({rect.y, height - rect.x - rect.w, rect.h, rect.w});
                layer->SetTransform(TransformType::ROTATE_270);
                break;
            }
            case ScreenRotation::INVALID_SCREEN_ROTATION: {
                ROSEN_LOGE("RsDebug RSHardwareProcessor::OnRotate Failed.");
                layer->SetTransform(TransformType::ROTATE_BUTT);
                break;
            }
            default:
                break;
        }
        ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
            layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
    }
}
} // namespace Rosen
} // namespace OHOS