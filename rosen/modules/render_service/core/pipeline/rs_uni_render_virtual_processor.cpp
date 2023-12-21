/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_virtual_processor.h"

#include <ctime>

#include "platform/common/rs_log.h"
#ifndef NEW_RENDER_CONTEXT
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#endif
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderVirtualProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                       std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine)) {
        return false;
    }

    // Do expand screen if the mirror id is invalid.
    if (mirroredId == INVALID_SCREEN_ID) {
        isExpand_ = true;
    } else {
        isExpand_ = false;
    }

    auto screenManager = CreateOrGetScreenManager();
    auto mirrorScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    canvasRotation_ = screenManager->GetCanvasRotation(node.GetScreenId());
    mirrorWidth_ = static_cast<float>(mirrorScreenInfo.width);
    mirrorHeight_ = static_cast<float>(mirrorScreenInfo.height);

    renderFrameConfig_.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;

    producerSurface_ = screenManager->GetProducerSurface(node.GetScreenId());
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!",
            node.GetScreenId());
        return false;
    }
    renderFrame_ = renderEngine_->RequestFrame(producerSurface_, renderFrameConfig_, forceCPU_, false);
    if (renderFrame_ == nullptr) {
        return false;
    }
    canvas_ = renderFrame_->GetCanvas();
    if (canvas_ == nullptr) {
        return false;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    if (mirrorNode) {
        mainScreenRotation_ = mirrorNode->GetScreenRotation();
        RS_LOGD("RSUniRenderVirtualProcessor::Init, Screen(id %{public}" PRIu64 "), Rotation: %d", node.GetScreenId(),
            static_cast<uint32_t>(mainScreenRotation_));
    }
    if (mirrorNode && node.IsFirstTimeToProcessor()) {
        if (isPhone_) {
            node.setFirstTimeScreenRotation(mainScreenRotation_);
        } else {
            auto boundsGeoPtr = (mirrorNode->GetRenderProperties().GetBoundsGeometry());
            if (boundsGeoPtr != nullptr) {
                boundsGeoPtr->UpdateByMatrixFromSelf();
                node.SetInitMatrix(boundsGeoPtr->GetMatrix());
            }
        }
    }
#ifndef USE_ROSEN_DRAWING
    if (mirrorNode && isPhone_) {
        CanvasRotation(node.getFirstTimeScreenRotation(), renderFrameConfig_.width, renderFrameConfig_.height);
        if (node.getFirstTimeScreenRotation() != ScreenRotation::ROTATION_0) {
            canvas_->translate(-(renderFrameConfig_.height / 2.0f), -(renderFrameConfig_.width / 2.0f));
        }
    } else {
        SkMatrix invertMatrix;
        if (node.GetInitMatrix().invert(&invertMatrix)) {
            screenTransformMatrix_.postConcat(invertMatrix);
        }
        canvas_->concat(screenTransformMatrix_);
    }
#else
    if (mirrorNode && isPhone_) {
        CanvasRotation(node.getFirstTimeScreenRotation(), renderFrameConfig_.width, renderFrameConfig_.height);
        if (node.getFirstTimeScreenRotation() != ScreenRotation::ROTATION_0) {
            canvas_->Translate(-(renderFrameConfig_.height / 2.0f), -(renderFrameConfig_.width / 2.0f));
        }
    } else {
        Drawing::Matrix invertMatrix;
        if (node.GetInitMatrix().Invert(invertMatrix)) {
            screenTransformMatrix_ = screenTransformMatrix_ * invertMatrix;
        }
        canvas_->ConcatMatrix(screenTransformMatrix_);
    }
#endif
    return true;
}

void RSUniRenderVirtualProcessor::CanvasRotation(ScreenRotation screenRotation, float width, float height)
{
#ifndef USE_ROSEN_DRAWING
    if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->translate(width / 2.0f, height / 2.0f);
        canvas_->rotate(90); // 90 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->translate(width / 2.0f, height / 2.0f);
        canvas_->rotate(270); // 270 degrees
    }
#else
    if (screenRotation == ScreenRotation::ROTATION_90) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(90, 0, 0); // 90 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_180) {
        canvas_->Rotate(180, width / 2.0f, height / 2.0f); // 180 degrees
    } else if (screenRotation == ScreenRotation::ROTATION_270) {
        canvas_->Translate(width / 2.0f, height / 2.0f);
        canvas_->Rotate(270, 0, 0); // 270 degrees
    }
#endif
}

void RSUniRenderVirtualProcessor::PostProcess(RSDisplayRenderNode* node)
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess surface is null!");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVirtualProcessor::PostProcess renderFrame_ is null.");
        return;
    }
    auto surfaceOhos = renderFrame_->GetSurface();
    renderEngine_->SetUiTimeStamp(renderFrame_, surfaceOhos);
    renderFrame_->Flush();
}

void RSUniRenderVirtualProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessSurface() is not supported.");
}

void RSUniRenderVirtualProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    if (!isExpand_) {
        RS_TRACE_NAME("RSUniRenderVirtualProcessor::ProcessDisplaySurface");
        if (canvas_ == nullptr || node.GetBuffer() == nullptr) {
            RS_LOGE("RSUniRenderVirtualProcessor::ProcessDisplaySurface: Canvas or buffer is null!");
            return;
        }

        const auto& property = node.GetRenderProperties();
        auto geoPtr = (property.GetBoundsGeometry());
        if (geoPtr) {
#ifndef USE_ROSEN_DRAWING
            canvas_->setMatrix(geoPtr->GetAbsMatrix());
#else
            canvas_->SetMatrix(geoPtr->GetAbsMatrix());
#endif
        }

#ifndef USE_ROSEN_DRAWING
        canvas_->save();
        canvas_->clear(SK_ColorBLACK);
        SkMatrix invertMatrix;
        if (screenTransformMatrix_.invert(&invertMatrix)) {
            canvas_->concat(invertMatrix);
        }
#else
        canvas_->Save();
        canvas_->Clear(Drawing::Color::COLOR_BLACK);
        Drawing::Matrix invertMatrix;
        if (screenTransformMatrix_.Invert(invertMatrix)) {
            canvas_->ConcatMatrix(invertMatrix);
        }
#endif
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU_);
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        float mainWidth = static_cast<float>(mainScreenInfo.width);
        float mainHeight = static_cast<float>(mainScreenInfo.height);
        auto screenCorrection = screenManager->GetScreenCorrection(node.GetScreenId());
        if ((RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0) || canvasRotation_) {
            std::swap(mainWidth, mainHeight);
            if (screenCorrection == ScreenRotation::ROTATION_270) {
#ifndef USE_ROSEN_DRAWING
                canvas_->rotate(-270, mirrorWidth_ / 2.0f, mirrorHeight_ / 2.0f);
#else
                canvas_->Rotate(-270, mirrorWidth_ / 2.0f, mirrorHeight_ / 2.0f);
#endif
            }
            if (canvasRotation_) {
                CanvasRotation(mainScreenRotation_, mirrorWidth_, mirrorHeight_);
            }
        }
        
        // If the width and height not match the main screen, calculate the dstRect.
        if (mainWidth != mirrorWidth_ || mainHeight != mirrorHeight_) {
#ifndef USE_ROSEN_DRAWING
            SkRect mirrorDstRect;
            if ((mirrorHeight_ / mirrorWidth_) < (mainHeight / mainWidth)) {
                float mirrorScale = mirrorHeight_ / mainHeight;
                float mainScale = mainHeight / mainWidth;
                if (canvasRotation_) {
                    if (screenCorrection == ScreenRotation::ROTATION_270) {
                        mirrorDstRect = SkRect::MakeXYWH(-(mirrorHeight_ / 2.0f),
                            -(mirrorScale * mainWidth * mainScale) / 2.0f,
                            mirrorHeight_, mirrorScale * mainWidth * mainScale);
                    } else {
                        mirrorDstRect = SkRect::MakeXYWH(-(mirrorHeight_ / 2.0f),
                            -(mirrorScale * mainWidth) / 2.0f,
                            mirrorHeight_, mirrorScale * mainWidth);
                    }
                } else {
                    if (screenCorrection == ScreenRotation::ROTATION_270) {
                        mirrorDstRect = SkRect::MakeXYWH((mirrorWidth_ - (mirrorScale * mainScale * mainWidth)) / 2.0f,
                            0, mirrorScale * mainScale * mainWidth, mirrorHeight_);
                    } else {
                        mirrorDstRect = SkRect::MakeXYWH((mirrorWidth_ - (mirrorScale * mainWidth)) / 2.0f, 0,
                            mirrorScale * mainWidth, mirrorHeight_);
                    }
                }
            } else if ((mirrorHeight_ / mirrorWidth_) > (mainHeight / mainWidth)) {
                float mirrorScale = mirrorWidth_ / mainWidth;
                if (canvasRotation_) {
                    mirrorDstRect = SkRect::MakeXYWH(-(mirrorHeight_ / 2.0f) +
                        ((mirrorHeight_ - (mirrorScale * mainHeight)) / 2.0f), -(mirrorWidth_ / 2.0f),
                        mirrorScale * mainHeight, mirrorWidth_);
                } else {
                    mirrorDstRect = SkRect::MakeXYWH(0,
                        (mirrorHeight_ - (mirrorScale * mainHeight)) / 2.0f,
                        mirrorWidth_, mirrorScale * mainHeight);
                }
            }
            params.dstRect = mirrorDstRect;
#else
            Drawing::Rect mirrorDstRect;
            if ((mirrorHeight_ / mirrorWidth_) < (mainHeight / mainWidth)) {
                float mirrorScale = mirrorHeight_ / mainHeight;
                float mainScale = mainHeight / mainWidth;
                if (canvasRotation_) {
                    if (screenCorrection == ScreenRotation::ROTATION_270) {
                        mirrorDstRect = Drawing::Rect(-(mirrorHeight_ / 2.0f),
                            -(mirrorScale * mainWidth * mainScale) / 2.0f, mirrorHeight_ / 2.0f,
                            (mirrorScale * mainWidth * mainScale) / 2.0f);
                    } else {
                        mirrorDstRect = Drawing::Rect(-(mirrorHeight_ / 2.0f), -(mirrorScale * mainWidth) / 2.0f,
                            mirrorHeight_ / 2.0f, (mirrorScale * mainWidth) / 2.0f);
                    }
                } else {
                    if (screenCorrection == ScreenRotation::ROTATION_270) {
                        mirrorDstRect = Drawing::Rect((mirrorWidth_ - (mirrorScale * mainScale * mainWidth)) / 2.0f, 0,
                            (mirrorWidth_ + (mirrorScale * mainScale * mainWidth)) / 2.0f, mirrorHeight_);
                    } else {
                        mirrorDstRect = Drawing::Rect((mirrorWidth_ - (mirrorScale * mainWidth)) / 2.0f, 0,
                            (mirrorWidth_ + (mirrorScale * mainWidth)) / 2.0f, mirrorHeight_);
                    }
                }
            } else if ((mirrorHeight_ / mirrorWidth_) > (mainHeight / mainWidth)) {
                float mirrorScale = mirrorWidth_ / mainWidth;
                if (canvasRotation_) {
                    mirrorDstRect = Drawing::Rect(-(mirrorHeight_ / 2.0f) +
                        ((mirrorHeight_ - (mirrorScale * mainHeight)) / 2.0f), -(mirrorWidth_ / 2.0f),
                        mirrorScale * mainHeight - (mirrorHeight_ / 2.0f) +
                        ((mirrorHeight_ - (mirrorScale * mainHeight)) / 2.0f), mirrorWidth_ - (mirrorWidth_ / 2.0f));
                } else {
                    mirrorDstRect = Drawing::Rect(0, (mirrorHeight_ - (mirrorScale * mainHeight)) / 2.0f,
                        mirrorWidth_,
                        mirrorScale * mainHeight + (mirrorHeight_ - (mirrorScale * mainHeight)));
                }
            }
            params.dstRect = mirrorDstRect;
#endif
        } else {
            if (canvasRotation_) {
#ifndef USE_ROSEN_DRAWING
                canvas_->translate(-(mirrorHeight_ / 2.0f), -(mirrorWidth_ / 2.0f));
#else
                canvas_->Translate(-(mirrorHeight_ / 2.0f), -(mirrorWidth_ / 2.0f));
#endif
            }
            
        }

        renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
#ifndef USE_ROSEN_DRAWING
        canvas_->restore();
#else
        canvas_->Restore();
#endif
    }
}

void RSUniRenderVirtualProcessor::ProcessDrivenSurface(RSDrivenSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessDrivenSurface() is not supported.");
}

void RSUniRenderVirtualProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    RS_LOGI("RSUniRenderVirtualProcessor::ProcessRcdSurface() is not supported.");
}
} // namespace Rosen
} // namespace OHOS