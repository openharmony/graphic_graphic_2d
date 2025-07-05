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

#include "drawable/rs_logical_display_render_node_drawable.h"

#include "rs_trace.h"
#include "graphic_feature_param_manager.h"
#include "feature/drm/rs_drm_util.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_pixel_map_util.h"
#include "static_factory.h"

// dfx
#include "drawable/dfx/rs_refresh_rate_dfx.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"

#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

namespace OHOS::Rosen::DrawableV2 {

namespace {
std::string RectVectorToString(const std::vector<RectI>& regionRects)
{
    std::string results = "";
    for (auto& rect : regionRects) {
        results += rect.ToString();
    }
    return results;
}
}
RSLogicalDisplayRenderNodeDrawable::Registrar RSLogicalDisplayRenderNodeDrawable::instance_;
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
RSLogicalDisplayRenderNodeDrawable::RSLogicalDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
}

RSLogicalDisplayRenderNodeDrawable::~RSLogicalDisplayRenderNodeDrawable()
{
}

RSRenderNodeDrawable::Ptr RSLogicalDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSLogicalDisplayRenderNodeDrawable(std::move(node));
}

void RSLogicalDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    RS_TRACE_NAME("RSLogicalDisplayRenderNodeDrawable::OnDraw");
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }

    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    curCanvas_ = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);

    if (!curCanvas_) {
        SetDrawSkipType(DrawSkipType::CANVAS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw curCanvas is nullptr!");
        return;
    }

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        SetDrawSkipType(DrawSkipType::RENDER_THREAD_PARAMS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw uniParam is nullptr!");
        return;
    }

    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw params is nullptr!");
        return;
    }
    SetScreenRotationForPointLight(*params);
    RSAutoCanvasRestore acr(curCanvas_, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        SetDrawSkipType(DrawSkipType::SCREEN_MANAGER_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw screenManager is nullptr!");
        return;
    }

    auto [_, screenParams] = GetScreenParams(*params);
    if (!screenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw screenParams is nullptr!");
        return;
    }

    auto screenInfo = screenParams->GetScreenInfo();
    const auto& processor = uniParam->GetRSProcessor();
    auto mirroredDrawable = params->GetMirrorSourceDrawable().lock();
    ScreenId paramScreenId = params->GetScreenId();
    auto mirroredRenderParams = mirroredDrawable ? mirroredDrawable->GetRenderParams().get() : nullptr;
    if (mirroredRenderParams ||
        params->GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        if (!processor->UpdateMirrorInfo(*this)) {
            SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
            RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirroredRenderParams) {
            currentBlackList_ = screenManager->GetVirtualScreenBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetBlackList(currentBlackList_);
            curVisibleRect_ = RSUniRenderThread::Instance().GetVisibleRect();
            enableVisibleRect_ = RSUniRenderThread::Instance().GetEnableVisibleRect();
            if (params->GetCompositeType() == CompositeType::UNI_RENDER_COMPOSITE) {
                WiredScreenProjection(*params, processor);
                lastBlackList_ = currentBlackList_;
                return;
            }
            currentTypeBlackList_ = screenManager->GetVirtualScreenTypeBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetTypeBlackList(currentTypeBlackList_);
            RSUniRenderThread::Instance().SetWhiteList(screenInfo.whiteList);
            curSecExemption_ = params->GetSecurityExemption();
            uniParam->SetSecExemption(curSecExemption_);
            DrawMirrorScreen(*params, processor);
            lastVisibleRect_ = curVisibleRect_;
            lastBlackList_ = currentBlackList_;
            lastTypeBlackList_ = currentTypeBlackList_;
            lastSecExemption_ = curSecExemption_;
        } else {
            // wireless expand screen draw
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
        }
        return;
    }

    bool needOffScreen = params->GetNeedOffscreen() || screenInfo.isSamplingOn;
    if (needOffScreen) {
        scaleManager_ = uniParam->GetSLRScaleManager();
        UpdateSlrScale(screenInfo, screenParams);
        ScaleCanvasIfNeeded(screenInfo);
        PrepareOffscreenRender(*this, !screenInfo.isSamplingOn);
#ifdef RS_PROFILER_ENABLED
        if (auto canvas =
                RSCaptureRecorder::GetInstance().TryInstantCapture(static_cast<float>(curCanvas_->GetWidth()),
                    static_cast<float>(curCanvas_->GetHeight()), SkpCaptureType::EXTENDED)) {
            curCanvas_->AddCanvas(canvas);
        }
#endif
        curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }

    // prepare canvas
    if (!params->GetNeedOffscreen()) {
        params->ApplyAlphaAndMatrixToCanvas(*curCanvas_);
    }

    // canvas draw
    RSRenderNodeDrawable::OnDraw(*curCanvas_);
    DrawAdditionalContent(*curCanvas_);

    if (needOffScreen && canvasBackup_) {
        Drawing::AutoCanvasRestore acr(*canvasBackup_, true);
        params->ApplyAlphaAndMatrixToCanvas(*curCanvas_);
#ifdef RS_PROFILER_ENABLED
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::EXTENDED);
#endif
        FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE),
            screenInfo.isSamplingOn);
    }

    if (RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable() && !params->IsRotationChanged()) {
        offscreenSurface_ = nullptr;
        offscreenCanvas_ = nullptr;
    }
}

void RSLogicalDisplayRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    if (!ShouldPaint()) {
        return;
    }

    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture params is null!");
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!paintFilterCanvas) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture paintFilterCanvas is null!");
        return;
    }
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        params->GetAncestorScreenDrawable().lock());
    if (!screenDrawable) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture screenDrawable is null!");
        return;
    }
    auto screenParam = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    if (!screenParam) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture screenParams is null!");
        return;
    }
    bool noBuffer = RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
        screenDrawable->GetRSSurfaceHandlerOnDraw()->GetBuffer() == nullptr;
    // temporary: copy not support when until display node on mirrored screen node
    bool forceRedraw = screenParam->GetChildDisplayCount() > 1;
    if (noBuffer) {
        RS_LOGW("RSLogicalDisplayRenderNodeDrawable::OnCapture: buffer is null!");
    }
    auto specialLayerType = GetSpecialLayerType(*params);
    // Screenshot blackList, exclude surfaceNode in blackList while capturing displayNode
    auto currentBlackList = RSUniRenderThread::Instance().GetBlackList();
    if (specialLayerType != NO_SPECIAL_LAYER || UNLIKELY(noBuffer) || screenParam->GetScreenInfo().isSamplingOn ||
        UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_) || screenDrawable->IsRenderSkipIfScreenOff() ||
        !currentBlackList.empty() || forceRedraw) {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::OnCapture: \
            process RSLogicalDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->GetId());
        RS_TRACE_NAME_FMT("RSLogicalDisplayRenderNode::%s screenId: [%" PRIu64 "]"
            " Not using UniRender buffer. specialLayer: %d, noBuffer: %d, "
            "isSamplingOn: %d, isRenderSkipIfScreenOff: %d, blackList: %lu", __func__, params->GetScreenId(),
            specialLayerType != NO_SPECIAL_LAYER, noBuffer, screenParam->GetScreenInfo().isSamplingOn,
            screenDrawable->IsRenderSkipIfScreenOff(), currentBlackList.size());
        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
        }

        RSRenderNodeDrawable::OnCapture(canvas);
        DrawAdditionalContent(*paintFilterCanvas);
    } else {
        canvas.Clear(Drawing::Color::COLOR_BLACK);
        DrawHardwareEnabledNodes(canvas, *params);
    }
}

void RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(
    Drawing::Canvas& canvas, RSLogicalDisplayRenderParams& params)
{
    auto rsCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rsCanvas) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes rscanvas is null!");
        return;
    }
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        params.GetAncestorScreenDrawable().lock());
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());

    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcNodes;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcTopNodes;
    RSUniRenderUtil::CollectHardwareEnabledNodesByDisplayNodeId(hwcNodes, hwcTopNodes, params.GetId());
    
    RS_LOGI("RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: \
        process RSScreenRenderNode(id:[%{public}" PRIu64 "]) \
        Using UniRender buffer with hwcNodes(%{public}zu), hwcTopNodes(%{public}zu).",
        GetId(), hwcNodes.size(), hwcTopNodes.size());
    RS_TRACE_NAME_FMT("Process ScreenRenderNodeDrawable[%{public}" PRIu64 "] \
        using UniRender buffer with hwcNodes(%{public}zu), hwcTopNodes(%{public}zu).",
        params.GetScreenId(), hwcNodes.size(), hwcTopNodes.size());
    
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcNodes, canvas, *screenParams);

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*screenDrawable->GetRSSurfaceHandlerOnDraw(), false);
    RSBaseRenderUtil::WriteSurfaceBufferToPng(drawParams.buffer);
    renderEngine->DrawScreenNodeWithParams(*rsCanvas, *screenDrawable->GetRSSurfaceHandlerOnDraw(), drawParams);
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcTopNodes, canvas, *screenParams);
}

void RSLogicalDisplayRenderNodeDrawable::DrawAdditionalContent(RSPaintFilterCanvas& canvas)
{
    RS_TRACE_FUNC();
    DrawWatermarkIfNeed(canvas);
    RSRefreshRateDfx(*this).OnDraw(canvas);
}

void RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed(RSPaintFilterCanvas& canvas)
{
    if (!RSUniRenderThread::Instance().GetWatermarkFlag()) {
        return;
    }
    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (UNLIKELY(!params)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed params is null");
        return;
    }
    auto image = RSUniRenderThread::Instance().GetWatermarkImg();
    if (UNLIKELY(image == nullptr)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed image is null");
        return;
    }
    if (auto screenManager = CreateOrGetScreenManager()) {
        RS_TRACE_FUNC();
        auto screenInfo = screenManager->QueryScreenInfo(params->GetScreenId());
        auto mainWidth = static_cast<float>(screenInfo.width);
        auto mainHeight = static_cast<float>(screenInfo.height);
        canvas.Save();
        canvas.ResetMatrix();
        auto rotation = params->GetScreenRotation();
        auto screenId = params->GetScreenId();
        auto screenCorrection = screenManager->GetScreenCorrection(screenId);
        if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
            screenCorrection != ScreenRotation::ROTATION_0) {
            // Recaculate rotation if mirrored screen has additional rotation angle
            rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
                - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
        }

        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }

        if (rotation != ScreenRotation::ROTATION_0) {
            if (rotation == ScreenRotation::ROTATION_90) {
                canvas.Rotate(-(RS_ROTATION_90), 0, 0); // 90 degree
                canvas.Translate(-(static_cast<float>(mainWidth)), 0);
            } else if (rotation == ScreenRotation::ROTATION_180) {
                canvas.Rotate(-(RS_ROTATION_180), static_cast<float>(mainWidth) / 2, // 2 half of screen width
                    static_cast<float>(mainHeight) / 2); // 2 half of screen height
            } else if (rotation == ScreenRotation::ROTATION_270) {
                canvas.Rotate(-(RS_ROTATION_270), 0, 0); // 270 degree
                canvas.Translate(0, -(static_cast<float>(mainHeight)));
            }
        }

        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
        Drawing::Brush rectBrush;
        canvas.AttachBrush(rectBrush);
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
        canvas.Restore();
    }
}

void RSLogicalDisplayRenderNodeDrawable::UpdateDisplayDirtyManager(std::shared_ptr<RSDirtyRegionManager> dirtyManager,
    int32_t bufferage, bool useAlignedDirtyRegion)
{
    dirtyManager->SetBufferAge(bufferage);
    dirtyManager->UpdateDirty(useAlignedDirtyRegion);
}

std::vector<RectI> RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirty(
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSScreenRenderNodeDrawable& curScreenDrawable,
    RSLogicalDisplayRenderParams& params, Drawing::Matrix canvasMatrix)
{
    // uniParam/drawable/mirroredParams not null in caller
    std::vector<RectI> mappedDamageRegionRects;
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto curScreenParams = static_cast<RSScreenRenderParams*>(curScreenDrawable.GetRenderParams().get());
    auto drawable = curScreenParams->GetMirrorSourceDrawable().lock();
    auto mirroredDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(drawable);
    auto mirrorParams = static_cast<RSScreenRenderParams*>(mirroredDrawable->GetRenderParams().get());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirty ScreenManager is nullptr");
        virtualProcesser->SetRoiRegionToCodec(mappedDamageRegionRects);
        return mappedDamageRegionRects;
    }
    const auto& curScreenInfo = curScreenParams->GetScreenInfo();
    if (!curScreenInfo.isEqualVsyncPeriod) {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirty frame rate is irregular");
        virtualProcesser->SetRoiRegionToCodec(mappedDamageRegionRects);
        return mappedDamageRegionRects;
    }
    const auto& mainScreenInfo = mirrorParams->GetScreenInfo();
    int32_t bufferAge = virtualProcesser->GetBufferAge();
    std::vector<RectI> damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, false);
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    for (auto& rect : damageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        mappedDamageRegionRects.emplace_back(mappedRect);
    }

    bool needRefresh = !(lastCanvasMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirrorParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || lastBlackList_ != currentBlackList_ ||
        lastTypeBlackList_ != currentTypeBlackList_ || params.IsSpecialLayerChanged() ||
        lastSecExemption_ != curSecExemption_ || uniParam->GetVirtualDirtyRefresh() || virtualDirtyNeedRefresh_ ||
        (enableVisibleRect_ && (lastVisibleRect_ != curVisibleRect_ || params.HasSecLayerInVisibleRectChanged()));
    if (needRefresh) {
        curScreenDrawable.GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyNeedRefresh_ = false;
        lastCanvasMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirrorParams->GetMatrix();
    }
    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
        if (mainScreenInfo.isSamplingOn && mainScreenInfo.samplingScale > 0) {
            Drawing::Matrix scaleMatrix;
            scaleMatrix.SetScaleTranslate(mainScreenInfo.samplingScale, mainScreenInfo.samplingScale,
                mainScreenInfo.samplingTranslateX, mainScreenInfo.samplingTranslateY);
            hwcRect = RSObjAbsGeometry::MapRect(hwcRect.ConvertTo<float>(), scaleMatrix);
            const Vector4<int> expandSize{mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance,
                mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance};
            hwcRect = hwcRect.MakeOutset(expandSize);
        }
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        curScreenDrawable.GetSyncDirtyManager()->MergeDirtyRect(mappedHwcRect);
    }
    UpdateDisplayDirtyManager(curScreenDrawable.GetSyncDirtyManager(), bufferAge, false);
    auto extraDirty = curScreenDrawable.GetSyncDirtyManager()->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        mappedDamageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        virtualProcesser->SetDirtyInfo(mappedDamageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            mappedDamageRegionRects.size(), RectVectorToString(mappedDamageRegionRects).c_str());
    }
    return mappedDamageRegionRects;
}

std::vector<RectI> RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen(
    RSScreenRenderNodeDrawable& curScreenDrawable, Drawing::Matrix canvasMatrix)
{
    std::vector<RectI> damageRegionRects;
    auto curScreenParams = static_cast<RSScreenRenderParams*>(curScreenDrawable.GetRenderParams().get());
    auto mirroredDrawable =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(curScreenParams->GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable || !mirroredDrawable->GetRenderParams()) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen mirroredNode is null");
        return damageRegionRects;
    }
    const auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr || !uniParam->IsVirtualDirtyEnabled() ||
        (enableVisibleRect_ && curVisibleRect_.GetTop() > 0)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen invalid uniparam");
        return damageRegionRects;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen screenManager is null");
        return damageRegionRects;
    }
    auto curScreenInfo = curScreenParams->GetScreenInfo();
    if (!curScreenInfo.isEqualVsyncPeriod) {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen frame rate is irregular");
        return damageRegionRects;
    }
    int32_t bufferAge = curScreenDrawable.GetBufferAge();
    const auto mirroredParams = static_cast<RSScreenRenderParams*>(mirroredDrawable->GetRenderParams().get());
    const auto& mainScreenInfo = mirroredParams->GetScreenInfo();
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    // merge history dirty and map to mirrored wired screen by matrix
    auto tempDamageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, true);
    for (auto& rect : tempDamageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        damageRegionRects.emplace_back(mappedRect);
    }

    auto syncDirtyManager = curScreenDrawable.GetSyncDirtyManager();
    // reset dirty rect as mirrored wired screen size when first time connection or matrix changed
    bool needRefresh = !(lastCanvasMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirroredParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || (enableVisibleRect_ && lastVisibleRect_ != curVisibleRect_) ||
        lastBlackList_ != currentBlackList_ || uniParam->GetVirtualDirtyRefresh() || virtualDirtyNeedRefresh_;
    if (needRefresh) {
        curScreenDrawable.GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyNeedRefresh_ = false;
        lastCanvasMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirroredParams->GetMatrix();
    }
    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        syncDirtyManager->MergeDirtyRect(mappedHwcRect);
    }
    UpdateDisplayDirtyManager(syncDirtyManager, bufferAge, false);
    auto extraDirty = syncDirtyManager->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        damageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        curScreenDrawable.SetDamageRegion(damageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            damageRegionRects.size(), RectVectorToString(damageRegionRects).c_str());
    }
    return damageRegionRects;
}

void RSLogicalDisplayRenderNodeDrawable::WiredScreenProjection(
    RSLogicalDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_FUNC();
    if (!curCanvas_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::WiredScreenProjection failed to create canvas");
        return;
    }
    auto [curScreenDrawable, curScreenParams] = GetScreenParams(params);
    if (!curScreenDrawable || !curScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::WiredScreenProjection curScreen is null");
        return;
    }
    auto [mirroredDrawable, mirroredParams,
        mirroredScreenDrawable, mirroredScreenParams] = GetMirrorSourceParams(params);
    if (!mirroredDrawable || !mirroredParams || !mirroredScreenDrawable || !mirroredScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::WiredScreenProjection mirror source is null");
        return;
    }
    // temporary: copy not support when multi display node on mirrored screen node
    bool forceRedraw = mirroredScreenParams->GetChildDisplayCount() > 1;

    bool isProcessSecLayer = !MultiScreenParam::IsExternalScreenSecure() &&
        mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    auto isRedraw = RSSystemParameters::GetDebugMirrorOndrawEnabled() || forceRedraw ||
        (RSSystemParameters::GetWiredScreenOndrawEnabled() && !enableVisibleRect_ &&
            (mirroredScreenParams->GetHDRPresent()  || isProcessSecLayer || !currentBlackList_.empty() ||
                mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED)));
    if (isRedraw) {
        isMirrorSLRCopy_ = false;
    } else {
        auto cacheImage = mirroredScreenDrawable->GetCacheImgForCapture();
        isMirrorSLRCopy_ = cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled() &&
                           !enableVisibleRect_ && RSSystemProperties::GetSLRScaleEnabled();
    }

    curCanvas_->Save();
    ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    Drawing::Matrix canvasMatrix = curCanvas_->GetTotalMatrix();
    RSDirtyRectsDfx rsDirtyRectsDfx(*curScreenDrawable);
    // HDR does not support wired screen
    if (isRedraw) {
        MirrorRedrawDFX(true, params.GetScreenId());
        DrawWiredMirrorOnDraw(*mirroredDrawable, params);
    } else {
        MirrorRedrawDFX(false, params.GetScreenId());
        std::vector<RectI> damageRegionRects = CalculateVirtualDirtyForWiredScreen(*curScreenDrawable,
            isMirrorSLRCopy_ ? scaleManager_->GetScaleMatrix() : curCanvas_->GetTotalMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, curScreenParams->GetScreenInfo());
        DrawWiredMirrorCopy(*mirroredDrawable);
    }
    curCanvas_->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    if (isRedraw && mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED)) {
        RSDrmUtil::DRMCreateLayer(processor, canvasMatrix);
        curScreenParams->SetGlobalZOrder(curScreenParams->GetGlobalZOrder() + 1);
    }
}

void RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorCopy(RSLogicalDisplayRenderNodeDrawable& mirroredDrawable)
{
    RS_TRACE_FUNC();
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable.GetRenderParams().get());
    if (!mirroredParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorCopy mirroredParams is null");
        return;
    }
    auto mirroredScreenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        mirroredParams->GetAncestorScreenDrawable().lock());
    if (!mirroredScreenDrawable) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorCopy mirroredScreenDrawable is null");
        return;
    }
    auto cacheImage = mirroredScreenDrawable->GetCacheImgForCapture();
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawWiredMirrorCopy with cacheImage");
        if (isMirrorSLRCopy_) {
            RS_TRACE_NAME("DrawWiredMirrorCopy with SLRScale");
            scaleManager_->ProcessCacheImage(*curCanvas_, *cacheImage);
        } else if (!enableVisibleRect_) {
            RS_TRACE_NAME("DrawWiredMirrorCopy with SkiaScale");
            RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
        } else {
            RS_TRACE_NAME_FMT("DrawWiredMirrorCopy with VisibleRect[%d, %d, %d, %d]",
                curVisibleRect_.GetLeft(), curVisibleRect_.GetTop(),
                curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight());
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas_, *cacheImage, curVisibleRect_,
                Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight()));
        }
    } else {
        RS_TRACE_NAME("DrawWiredMirrorCopy with displaySurface");
        auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(
            *mirroredScreenDrawable->GetRSSurfaceHandlerOnDraw(), false); // false: draw with gpu
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (enableVisibleRect_) {
            drawParams.srcRect = curVisibleRect_;
            drawParams.dstRect = Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight());
        }
        drawParams.isMirror = true;
        renderEngine->DrawScreenNodeWithParams(*curCanvas_,
            *mirroredScreenDrawable->GetRSSurfaceHandlerOnDraw(), drawParams);
        RSMainThread::Instance()->RequestNextVSync();
    }
}

void RSLogicalDisplayRenderNodeDrawable::CheckDirtyRefresh(CompositeType type, bool hasSecLayerInVisibleRect)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        return;
    }
    auto [mirroredDrawable, mirroredParams, _, __] = GetMirrorSourceParams(*params);
    if (!mirroredDrawable || !mirroredParams) {
        return;
    }
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr) {
        return;
    }
    auto hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    if (type == CompositeType::UNI_RENDER_COMPOSITE) {
        if (hasSecSurface) {
            uniParam->SetVirtualDirtyRefresh(true);
        }
    } else if (type == CompositeType::UNI_RENDER_MIRROR_COMPOSITE) {
        if ((((!enableVisibleRect_ && hasSecSurface) || (enableVisibleRect_ && hasSecLayerInVisibleRect)) &&
            !uniParam->GetSecExemption()) || params->GetVirtualScreenMuteStatus()) {
            uniParam->SetVirtualDirtyRefresh(true);
        }
    }
}

void RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorOnDraw(
    RSLogicalDisplayRenderNodeDrawable& mirroredDrawable, RSLogicalDisplayRenderParams& params)
{
    RS_TRACE_FUNC();
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr) {
        return;
    }
    const auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable.GetRenderParams().get());
    if (mirroredParams == nullptr) {
        return;
    }
    RS_TRACE_NAME("DrawWiredMirror with Redraw");
    // for HDR
    curCanvas_->SetOnMultipleScreen(true);
    curCanvas_->SetDisableFilterCache(true);
    if (!MultiScreenParam::IsExternalScreenSecure()) {
        auto hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
        if (hasSecSurface) {
            curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
            virtualDirtyNeedRefresh_ = true;
            RS_LOGI("RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorOnDraw, "
                "set canvas to black because of security layer.");
            return;
        }
    }
    curCanvas_->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
    bool isOpDropped = uniParam->IsOpDropped();
    uniParam->SetOpDropped(false);

    auto [_, mirroredScreenParams] = GetScreenParams(*mirroredParams);
    if (!mirroredScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWiredMirrorOnDraw mirroredScreenParams is null");
        return;
    }

    auto screenInfo = mirroredScreenParams->GetScreenInfo();
    uniParam->SetScreenInfo(screenInfo);
    Drawing::Rect rect(0, 0, screenInfo.width, screenInfo.height);
    curCanvas_->ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    mirroredDrawable.RSRenderNodeDrawable::OnDraw(*curCanvas_);
    auto [__, screenParam] = GetScreenParams(params);
    bool displayP3Enable = screenParam ? screenParam->GetNewColorSpace() == GRAPHIC_COLOR_GAMUT_DISPLAY_P3 : false;
     // 1.f: wired screen not use hdr, use default value 1.f
    RSUniRenderUtil::SwitchColorFilter(*curCanvas_, 1.f, displayP3Enable);

    uniParam->SetOpDropped(isOpDropped);
    RSUniRenderThread::Instance().SetBlackList({});
}

void RSLogicalDisplayRenderNodeDrawable::DrawMirrorScreen(
    RSLogicalDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_FUNC();
    // uniParam/drawable/mirroredParams/renderParams_ not null in caller
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto [mirroredDrawable, mirroredParams,
        mirroredScreenDrawable, mirroredScreenParams] = GetMirrorSourceParams(params);
    if (!mirroredDrawable || !mirroredParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorScreen mirror source is null");
        return;
    }
    auto specialLayerType = GetSpecialLayerType(*mirroredParams,
        enableVisibleRect_ ? params.HasSecLayerInVisibleRect() : true);
    auto virtualProcesser = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcesser) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorScreen virtualProcesser is null");
        return;
    }
    virtualProcesser->CanvasInit(*this);

    auto cacheImage = mirroredScreenDrawable ? mirroredScreenDrawable->GetCacheImgForCapture() : nullptr;
    const auto screenInfo = uniParam->GetScreenInfo(); // record screenInfo
    auto [__, screenParams] = GetScreenParams(params);
    if (!screenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorScreen screenParams is null");
        return;
    }
    // temporary: copy not support when until display node on mirrored screen node
    bool forceRedraw = mirroredScreenParams->GetChildDisplayCount() > 1;

    uniParam->SetScreenInfo(screenParams->GetScreenInfo());
    // if specialLayer is visible and no CacheImg
    if ((mirroredParams->IsSecurityDisplay() != params.IsSecurityDisplay() && specialLayerType == HAS_SPECIAL_LAYER)
        || !cacheImage || params.GetVirtualScreenMuteStatus() || forceRedraw) {
        MirrorRedrawDFX(true, params.GetScreenId());
        virtualProcesser->SetDrawVirtualMirrorCopy(false);
        DrawMirror(params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, *uniParam);
    } else {
        MirrorRedrawDFX(false, params.GetScreenId());
        virtualProcesser->SetDrawVirtualMirrorCopy(true);
        DrawMirrorCopy(params, virtualProcesser, *uniParam);
    }
    uniParam->SetScreenInfo(screenInfo); // reset screenInfo
}

void RSLogicalDisplayRenderNodeDrawable::DrawMirrorCopy(RSLogicalDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam)
{
    RS_TRACE_FUNC();
    auto [curScreenDrawable, curScreenParams] = GetScreenParams(params);
    if (!curScreenDrawable || !curScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorCopy curScreenDrawable or curScreenParams is null");
        return;
    }
    auto [mirroredDrawable, mirroredParams,
        mirroredScreenDrawable, mirroredScreenParams] = GetMirrorSourceParams(params);
    if (!mirroredDrawable || !mirroredParams || !mirroredScreenDrawable || !mirroredScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorCopy mirror source is null");
        return;
    }
    auto cacheImage = mirroredScreenDrawable ? mirroredScreenDrawable->GetCacheImgForCapture() : nullptr;
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
#ifdef RS_PROFILER_ENABLED
    if (auto canvas =
            RSCaptureRecorder::GetInstance().TryInstantCapture(
                static_cast<float>(virtualProcesser->GetCanvas()->GetWidth()),
                static_cast<float>(virtualProcesser->GetCanvas()->GetHeight()), SkpCaptureType::IMG_CACHED)) {
        if (virtualProcesser && virtualProcesser->GetCanvas()) {
            virtualProcesser->GetCanvas()->AddCanvas(canvas);
        }
    }
#endif
    virtualProcesser->CalculateTransform(GetOriginScreenRotation());
    RSDirtyRectsDfx rsDirtyRectsDfx(*curScreenDrawable);
    std::shared_ptr<RSSLRScaleFunction> slrManager = enableVisibleRect_ ? nullptr : virtualProcesser->GetSlrManager();
    if (!uniParam.IsVirtualDirtyEnabled() || (enableVisibleRect_ && curVisibleRect_.GetTop() > 0)) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    } else {
        auto dirtyRects = CalculateVirtualDirty(virtualProcesser, *curScreenDrawable, params,
            slrManager ? slrManager->GetScaleMatrix() : virtualProcesser->GetCanvasMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, curScreenParams->GetScreenInfo());
    }

    curCanvas_ = virtualProcesser->GetCanvas().get();
    if (!curCanvas_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirrorCopy failed to get canvas.");
        return;
    }
    // Clean up the content of the previous frame
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    virtualProcesser->CanvasClipRegionForUniscaleMode();
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));

    curCanvas_->Save();
    if (slrManager) {
        auto scaleNum = slrManager->GetScaleNum();
        curCanvas_->Scale(scaleNum, scaleNum);
    }
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcNodes;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcTopNodes;
    RSUniRenderUtil::CollectHardwareEnabledNodesByDisplayNodeId(hwcNodes, hwcTopNodes, mirroredDrawable->GetId());
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcNodes, *curCanvas_, *mirroredScreenParams);
    curCanvas_->Restore();

    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawMirrorCopy with cacheImage");
        if (!enableVisibleRect_) {
            virtualProcesser->ProcessCacheImage(*cacheImage);
        } else {
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas_, *cacheImage, curVisibleRect_,
                Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight()));
        }
    } else {
        RS_TRACE_NAME("DrawMirrorCopy with displaySurface");
        virtualProcesser->ProcessScreenSurfaceForRenderThread(*mirroredScreenDrawable);
    }
    curCanvas_->Save();
    if (slrManager) {
        auto scaleNum = slrManager->GetScaleNum();
        curCanvas_->Scale(scaleNum, scaleNum);
    } else if (mirroredScreenParams && mirroredScreenParams->GetScreenInfo().isSamplingOn) {
        auto scaleNum = mirroredScreenParams->GetScreenInfo().samplingScale;
        curCanvas_->Scale(scaleNum, scaleNum);
    }
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcTopNodes, *curCanvas_, *mirroredScreenParams);
    curCanvas_->Restore();
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::IMG_CACHED);
#endif
    RSUniRenderThread::ResetCaptureParam();
    uniParam.SetOpDropped(isOpDropped);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
}

void RSLogicalDisplayRenderNodeDrawable::SetSecurityMask(RSProcessor& processor)
{
    RS_TRACE_FUNC();
    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (auto screenManager = CreateOrGetScreenManager()) {
        auto imagePtr = screenManager->GetScreenSecurityMask(params->GetScreenId());
        auto image = RSPixelMapUtil::ExtractDrawingImage(imagePtr);
        if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
            return;
        }

        auto watermark = RSUniRenderThread::Instance().GetWatermarkImg();
        auto screenInfo = screenManager->QueryScreenInfo(params->GetScreenId());
        float realImageWidth = static_cast<float>(image->GetWidth());
        float realImageHeight = static_cast<float>(image->GetHeight());

        curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        float screenWidth = static_cast<float>(screenInfo.width);
        float screenHeight = static_cast<float>(screenInfo.height);
        // Area to be drawn in the actual image
        auto dstRect = RSUniRenderUtil::GetImageRegions(screenWidth, screenHeight, realImageWidth, realImageHeight);
        // Make sure the canvas is oriented accurately.
        curCanvas_->ResetMatrix();

        Drawing::Brush brush;
        curCanvas_->AttachBrush(brush);
        curCanvas_->DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        if (watermark) {
            curCanvas_->DrawImageRect(*watermark, srcRect, dstRect, Drawing::SamplingOptions(),
                Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        }
        curCanvas_->DetachBrush();

        RS_LOGI("RSLogicalDisplayRenderNodeDrawable::SetSecurityMask, this interface is invoked"
            "when the security layer is used and mask resources are set.");
        curCanvas_->SetDisableFilterCache(false);
    }
}

void RSLogicalDisplayRenderNodeDrawable::SetCanvasBlack(RSProcessor& processor)
{
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    RS_LOGI("DisplayDrawable::SetCanvasBlack");
    curCanvas_->SetDisableFilterCache(false);
}

void RSLogicalDisplayRenderNodeDrawable::DrawMirror(RSLogicalDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam)
{
    RS_TRACE_FUNC();

    auto [curScreenDrawable, curScreenParams] = GetScreenParams(params);
    if (!curScreenDrawable || !curScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirror curScreenDrawable or curScreenParams is null");
        return;
    }
    // uniParam/drawable/mirroredParams not null in caller
    auto [mirroredDrawable, mirroredParams, _, mirroredScreenParams] = GetMirrorSourceParams(params);
    if (!mirroredDrawable || !mirroredParams || !mirroredScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirror mirroredDrawable or mirroredParams is null");
        return;
    }
    curCanvas_ = virtualProcesser->GetCanvas().get();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirror failed to get canvas.");
        return;
    }
    // for HDR
    curCanvas_->SetOnMultipleScreen(true);
    curCanvas_->SetDisableFilterCache(true);
    auto hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawMirror ScreenManager is nullptr");
        return;
    }
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params.GetScreenId());
    bool isSecurity = (!enableVisibleRect_ && hasSecSurface) ||
                      (enableVisibleRect_ && params.HasSecLayerInVisibleRect());
    bool securitySkip = (isSecurity && !uniParam.GetSecExemption() && !virtualSecLayerOption) ||
        params.GetVirtualScreenMuteStatus();
    if (securitySkip) {
#ifdef RS_PROFILER_ENABLED
        if (auto canvas =
                RSCaptureRecorder::GetInstance().TryInstantCapture(static_cast<float>(curCanvas_->GetWidth()),
                    static_cast<float>(curCanvas_->GetHeight()), SkpCaptureType::ON_CAPTURE)) {
            curCanvas_->AddCanvas(canvas);
        }
#endif
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
        if (screenManager->GetScreenSecurityMask(params.GetScreenId())) {
            SetSecurityMask(*virtualProcesser);
        } else {
            SetCanvasBlack(*virtualProcesser);
        }
#ifdef RS_PROFILER_ENABLED
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);
#endif
        virtualDirtyNeedRefresh_ = true;
        curCanvas_->RestoreToCount(0);
        return;
    }
    curCanvas_->Save();
    virtualProcesser->ScaleMirrorIfNeed(GetOriginScreenRotation(), *curCanvas_);
    auto mirroredScreenInfo = mirroredScreenParams->GetScreenInfo();
    UpdateSlrScale(mirroredScreenInfo);
    ScaleCanvasIfNeeded(mirroredScreenInfo);

    RSDirtyRectsDfx rsDirtyRectsDfx(*curScreenDrawable);
    if (uniParam.IsVirtualDirtyEnabled() && !enableVisibleRect_) {
        Drawing::Matrix matrix = curCanvas_->GetTotalMatrix();
        std::vector<RectI> dirtyRects = CalculateVirtualDirty(virtualProcesser, *curScreenDrawable, params, matrix);
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, curScreenParams->GetScreenInfo());
    } else {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    }
    // Clean up the content of the previous frame
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    virtualProcesser->CanvasClipRegionForUniscaleMode(visibleClipRectMatrix_, mirroredScreenInfo);
    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());
    PrepareOffscreenRender(*mirroredDrawable, false);
#ifdef RS_PROFILER_ENABLED
    if (auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(static_cast<float>(curCanvas_->GetWidth()),
        static_cast<float>(curCanvas_->GetHeight()), SkpCaptureType::ON_CAPTURE)) {
        curCanvas_->AddCanvas(canvas);
    }
#endif
    // set mirror screen capture param
    // Don't need to scale here since the canvas has been switched from mirror frame to offscreen
    // surface in PrepareOffscreenRender() above. The offscreen surface has the same size as
    // the main display that's why no need additional scale.
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));
    RSUniRenderThread::GetCaptureParam().virtualScreenId_ = params.GetScreenId();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false); // disable partial render
    (mirroredDrawable->*drawFunc)(*curCanvas_);
    uniParam.SetOpDropped(isOpDropped);
    RSUniRenderThread::ResetCaptureParam();
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);
#endif
    FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST),
        mirroredScreenInfo.isSamplingOn);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    RSUniRenderThread::Instance().SetBlackList({});
    RSUniRenderThread::Instance().SetTypeBlackList({});
    RSUniRenderThread::Instance().SetWhiteList({});
    uniParam.SetSecExemption(false);
}

void RSLogicalDisplayRenderNodeDrawable::UpdateSlrScale(ScreenInfo& screenInfo, RSScreenRenderParams* params)
{
    if (screenInfo.isSamplingOn && RSSystemProperties::GetSLRScaleEnabled()) {
        if (scaleManager_== nullptr) {
            scaleManager_ = std::make_unique<RSSLRScaleFunction>(
                screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
        } else {
            scaleManager_->CheckOrRefreshScreen(
                screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
        }
        if (params != nullptr) {
            scaleManager_->CheckOrRefreshColorSpace(params->GetNewColorSpace());
        }
        screenInfo.samplingDistance = scaleManager_->GetKernelSize();
    } else {
        scaleManager_ = nullptr;
    }
}

void RSLogicalDisplayRenderNodeDrawable::ScaleCanvasIfNeeded(const ScreenInfo& screenInfo)
{
    if (!screenInfo.isSamplingOn) {
        scaleManager_ = nullptr;
        return;
    }
    if (RSSystemProperties::GetSLRScaleEnabled() && scaleManager_ != nullptr) {
        scaleManager_->CanvasScale(*curCanvas_);
        return;
    }
    scaleManager_ = nullptr;
    if (enableVisibleRect_) {
        // save canvas matrix to calculate visible clip rect
        visibleClipRectMatrix_ = curCanvas_->GetTotalMatrix();
    }
    curCanvas_->Translate(screenInfo.samplingTranslateX, screenInfo.samplingTranslateY);
    curCanvas_->Scale(screenInfo.samplingScale, screenInfo.samplingScale);
}

void RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen(
    RSLogicalDisplayRenderNodeDrawable& mirroredDrawable)
{
    auto& mirroredParams = mirroredDrawable.GetRenderParams();
    if (!mirroredParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen mirroredParams is null");
        return;
    }
    auto [_, mirroredScreenParams] = GetScreenParams(*mirroredParams);
    auto& nodeParams = GetRenderParams();
    if (!nodeParams || !mirroredScreenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen nodeParams is null");
        return;
    }
    auto [__, screenParam] = GetScreenParams(*nodeParams);
    if (!screenParam) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen screenParam is null");
        return;
    }
    auto mainScreenInfo = mirroredScreenParams->GetScreenInfo();
    auto mainWidth = enableVisibleRect_ ? curVisibleRect_.GetWidth() : static_cast<float>(mainScreenInfo.width);
    auto mainHeight = enableVisibleRect_ ? curVisibleRect_.GetHeight() : static_cast<float>(mainScreenInfo.height);
    auto mirrorScreenInfo = screenParam->GetScreenInfo();
    auto mirrorWidth = static_cast<float>(mirrorScreenInfo.width);
    auto mirrorHeight = static_cast<float>(mirrorScreenInfo.height);

    auto rotation = mirroredParams->GetScreenRotation();
    auto screenManager = CreateOrGetScreenManager();
    RS_TRACE_NAME_FMT("ScaleAndRotateMirrorForWiredScreen[%" PRIu64 "](%f, %f), [%" PRIu64 "](%f, %f), rotation: %d",
        mirroredScreenParams->GetScreenId(), mainWidth, mainHeight, mirrorScreenInfo.id,
        mirrorWidth, mirrorHeight, rotation);
    if (screenManager) {
        auto screenCorrection = screenManager->GetScreenCorrection(mirroredScreenParams->GetScreenId());
        if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
            screenCorrection != ScreenRotation::ROTATION_0) {
            // Recaculate rotation if mirrored screen has additional rotation angle
            rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
                - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
        }
    }
    // Rotate
    RotateMirrorCanvas(rotation, mirrorWidth, mirrorHeight);
    // not support rotation for MirrorScreen enableVisibleRect
    rotation = enableVisibleRect_ ? ScreenRotation::ROTATION_0 : rotation;
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mirrorWidth, mirrorHeight);
    }
    curCanvas_->Clear(SK_ColorBLACK);
    // must after rotate and swap width/height
    if (isMirrorSLRCopy_) {
        if (scaleManager_ == nullptr) {
            scaleManager_ = std::make_unique<RSSLRScaleFunction>(mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        } else {
            scaleManager_->CheckOrRefreshScreen(mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        }
        scaleManager_->CheckOrRefreshColorSpace(mirroredScreenParams->GetNewColorSpace());
        isMirrorSLRCopy_ = scaleManager_->GetIsSLRCopy();
    }
    // Scale
    if (mainWidth > 0 && mainHeight > 0) {
        if (isMirrorSLRCopy_) {
            scaleManager_->CanvasScale(*curCanvas_);
        } else {
            auto scaleNum = std::min(mirrorWidth / mainWidth, mirrorHeight / mainHeight);
            int angle = RSUniRenderUtil::GetRotationFromMatrix(curCanvas_->GetTotalMatrix());
            if (RSSystemProperties::IsSuperFoldDisplay() && RSBaseRenderUtil::PortraitAngle(angle)) {
                scaleNum = mirrorHeight / mainWidth;
                std::swap(mirrorWidth, mirrorHeight);
            }
            curCanvas_->Translate((mirrorWidth - scaleNum * mainWidth) * 0.5f,
                (mirrorHeight - scaleNum * mainHeight) * 0.5f);
            curCanvas_->Scale(scaleNum, scaleNum);
            curCanvas_->ClipRect(Drawing::Rect(0, 0, mainWidth, mainHeight), Drawing::ClipOp::INTERSECT, false);
        }
    }
}

void RSLogicalDisplayRenderNodeDrawable::RotateMirrorCanvas(ScreenRotation& rotation, float width, float height)
{
    switch (rotation) {
        case ScreenRotation::ROTATION_0:
            break;
        case ScreenRotation::ROTATION_90:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(90, 0, 0); // 90 degrees
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        case ScreenRotation::ROTATION_180:
            // 180 is the rotate angle, calculate half width and half height requires divide by 2
            curCanvas_->Rotate(180, width / 2.0f, height / 2.0f);
            break;
        case ScreenRotation::ROTATION_270:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(270, 0, 0); // 270 degrees
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        default:
            break;
    }
}

int32_t RSLogicalDisplayRenderNodeDrawable::GetSpecialLayerType(
    RSLogicalDisplayRenderParams& params, bool isSecLayerInVisibleRect)
{
    auto& uniRenderThread = RSUniRenderThread::Instance();
    const auto& specialLayerManager = params.GetSpecialLayerMgr();
    bool hasGeneralSpecialLayer = (specialLayerManager.Find(SpecialLayerType::HAS_SECURITY) &&
        isSecLayerInVisibleRect) || specialLayerManager.Find(SpecialLayerType::HAS_SKIP) ||
        specialLayerManager.Find(SpecialLayerType::HAS_PROTECTED) || uniRenderThread.IsColorFilterModeOn();
    auto [_, screenParams] = GetScreenParams(params);
    if (screenParams) {
        hasGeneralSpecialLayer |= screenParams->GetHDRPresent();
    }
    RS_LOGD("RSLogicalDisplayRenderNodeDrawable::SpecialLayer:%{public}" PRIu32 ", CurtainScreen:%{public}d, "
        "HDRPresent:%{public}d, ColorFilter:%{public}d", specialLayerManager.Get(),
        uniRenderThread.IsCurtainScreenOn(), (screenParams && screenParams->GetHDRPresent()),
        uniRenderThread.IsColorFilterModeOn());
    if (RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        hasGeneralSpecialLayer |= (specialLayerManager.Find(SpecialLayerType::HAS_SNAPSHOT_SKIP) ||
            uniRenderThread.IsCurtainScreenOn());
    } else {
        hasGeneralSpecialLayer |= !uniRenderThread.GetWhiteList().empty() || !currentBlackList_.empty() ||
            !currentTypeBlackList_.empty();
    }
    if (hasGeneralSpecialLayer) {
        return HAS_SPECIAL_LAYER;
    } else if (params.HasCaptureWindow()) {
        return CAPTURE_WINDOW;
    } else {
        return NO_SPECIAL_LAYER;
    }
}

void RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender(
    const RSLogicalDisplayRenderNodeDrawable& displayDrawable, bool useFixedSize)
{
    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    if (UNLIKELY(!params)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender params is null!");
        return;
    }
    canvasBackup_ = nullptr;
    useFixedOffscreenSurfaceSize_ = false;
    auto frameSize = params->GetFrameRect();
    auto offscreenWidth = static_cast<int32_t>(frameSize.GetWidth());
    auto offscreenHeight = static_cast<int32_t>(frameSize.GetHeight());

    RS_TRACE_FUNC();
    // use fixed surface size in order to reduce create texture
    if (useFixedSize && RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable()
        && params->IsRotationChanged()) {
        useFixedOffscreenSurfaceSize_ = true;
        int32_t maxRenderLength = {};
        if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
            maxRenderLength = std::ceil(std::hypot(offscreenWidth, offscreenHeight));
            offscreenTranslateX_ = std::round((maxRenderLength - offscreenWidth) * 0.5f);
            offscreenTranslateY_ = std::round((maxRenderLength - offscreenHeight) * 0.5f);
        } else {
            maxRenderLength =
                static_cast<int32_t>(std::max(offscreenWidth, offscreenHeight));
            if (offscreenSurface_ != nullptr
                && maxRenderLength != std::max(offscreenSurface_->Width(), offscreenSurface_->Height())) {
                RS_TRACE_NAME("offscreen surface's max size has changed");
                offscreenSurface_ = nullptr;
            }
        }
        offscreenWidth = maxRenderLength;
        offscreenHeight = maxRenderLength;
    } else {
        offscreenTranslateX_ = 0;
        offscreenTranslateY_ = 0;
    }
    if (params->IsRotationChanged()) {
        if (RSUniRenderThread::Instance().GetVmaOptimizeFlag()) {
            // render this frame with vma cache on
            Drawing::StaticFactory::SetVmaCacheStatus(true);
        }
    }

    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, "
            "offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (curCanvas_->GetSurface() == nullptr) {
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, current surface is nullptr");
        return;
    }
    if (!params->GetNeedOffscreen() || !useFixedOffscreenSurfaceSize_ || !offscreenSurface_) {
        RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d]", offscreenWidth, offscreenHeight);
        offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
    }

    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenSurface is nullptr");
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        return;
    }
    offscreenCanvas_ = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        offscreenCanvas_->ResetMatrix();
        offscreenCanvas_->Translate(offscreenTranslateX_, offscreenTranslateY_);
    }

    // copy HDR properties into offscreen canvas
    offscreenCanvas_->CopyHDRConfiguration(*curCanvas_);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas_->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(curCanvas_, offscreenCanvas_.get());
}

void RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender(
    const Drawing::SamplingOptions& sampling, bool isSamplingOn)
{
    RS_TRACE_FUNC();
    if (canvasBackup_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, offscreenSurface_ is nullptr");
        return;
    }
    auto image = offscreenSurface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, Surface::GetImageSnapshot is nullptr");
        return;
    }
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvasBackup_->AttachBrush(brush);

    if (isSamplingOn) {
        if (RSSystemProperties::GetSLRScaleEnabled() && scaleManager_) {
            scaleManager_->ProcessOffscreenImage(*canvasBackup_, *image);
        } else {
            canvasBackup_->DrawImage(*image, 0, 0, sampling);
        }
    } else if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        canvasBackup_->DrawImage(*image, -offscreenTranslateX_, -offscreenTranslateY_, sampling);
        canvasBackup_->Translate(offscreenTranslateX_, offscreenTranslateY_);
    } else {
        canvasBackup_->DrawImage(*image, 0, 0, sampling);
    }
    canvasBackup_->DetachBrush();

    if (!useFixedOffscreenSurfaceSize_) {
        offscreenSurface_ = nullptr;
        offscreenCanvas_ = nullptr;
    }
    curCanvas_ = canvasBackup_;
    canvasBackup_ = nullptr;
}

void RSLogicalDisplayRenderNodeDrawable::MirrorRedrawDFX(bool mirrorRedraw, ScreenId screenId)
{
    if (!mirrorRedraw_.has_value() || mirrorRedraw_.value() != mirrorRedraw) {
        mirrorRedraw_ = mirrorRedraw;
        RS_LOGI("RSLogicalDisplayRenderNodeDrawable::%{public}s mirror screenId: %{public}" PRIu64
            " drawing path changed, mirrorRedraw_: %{public}d", __func__, screenId, mirrorRedraw);
    }
}

void RSLogicalDisplayRenderNodeDrawable::SetScreenRotationForPointLight(RSLogicalDisplayRenderParams& params)
{
    auto screenManager = CreateOrGetScreenManager();
    auto [mirroredDrawable, mirroredParams, _, __] = GetMirrorSourceParams(params);
    ScreenId screenId = params.GetScreenId();
    ScreenRotation screenRotation = params.GetScreenRotation();
    if (mirroredParams) {
        screenId = mirroredParams->GetScreenId();
        screenRotation = mirroredParams->GetScreenRotation();
    }
    auto screenCorrection = screenManager->GetScreenCorrection(screenId);
    screenRotation = static_cast<ScreenRotation>(
        (static_cast<int>(screenRotation) + SCREEN_ROTATION_NUM - static_cast<int>(screenCorrection)) %
        SCREEN_ROTATION_NUM);
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
}

RSLogicalDisplayRenderNodeDrawable::AncestorParams RSLogicalDisplayRenderNodeDrawable::GetScreenParams(
    RSRenderParams& params)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(&params);
    if (!displayParams) {
        return { nullptr, nullptr };
    }
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        displayParams->GetAncestorScreenDrawable().lock());
    if (!screenDrawable) {
        return { nullptr, nullptr };
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    return {screenDrawable.get(), screenParams};
}

RSLogicalDisplayRenderNodeDrawable::MirrorSourceParams RSLogicalDisplayRenderNodeDrawable::GetMirrorSourceParams(
    RSRenderParams& params)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(&params);
    if (!displayParams) {
        return { nullptr, nullptr, nullptr, nullptr };
    }
    auto mirroredDrawable = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(
        displayParams->GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable) {
        return { nullptr, nullptr, nullptr, nullptr };
    }
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable->GetRenderParams().get());
    if (!mirroredParams) {
        return { mirroredDrawable.get(), nullptr, nullptr, nullptr };
    }

    return std::tuple_cat(std::tuple{mirroredDrawable.get(), mirroredParams}, GetScreenParams(*mirroredParams));
}
} // namespace OHOS::ROSEN::DrawableV2