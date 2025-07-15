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

#include "rs_refresh_rate_dfx.h"

#include <parameters.h>

#include "rs_trace.h"
#include "hgm_core.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/render_thread/rs_uni_render_util.h"

namespace OHOS::Rosen {

void RSRefreshRateDfx::OnDraw(RSPaintFilterCanvas& canvas)
{
    RS_TRACE_FUNC();
    if (!RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        return;
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(logicalDisplayParams_.get());
    if (UNLIKELY(!params)) {
        return;
    }
    auto screenId = params->GetScreenId();
    static const std::string FOLD_SCREEN_TYPE = system::GetParameter("const.window.foldscreen.type", "0,0,0,0");
    const char dualDisplay = '2';
    // fold device with two logic screens
    if ((FOLD_SCREEN_TYPE[0] == dualDisplay) && screenId != 0) {
        return;
    }
    uint32_t currentRefreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(screenId);
    uint32_t realtimeRefreshRate = RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRate(screenId);
    static bool showRealtimeRefreshRate = system::GetParameter("const.logsystem.versiontype", "") == "beta";
    std::string info = std::to_string(currentRefreshRate);
    if (showRealtimeRefreshRate || RSSystemParameters::GetShowRefreshRateEnabled()) {
        info += " " + std::to_string(realtimeRefreshRate);
    }
    std::shared_ptr<Drawing::Typeface> tf = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC", Drawing::FontStyle());
    Drawing::Font font;
    font.SetSize(100); // 100:Scalar of setting font size
    font.SetTypeface(tf);
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(info.c_str(), font);

    Drawing::Brush brush;
    brush.SetColor(currentRefreshRate <= 60 ? SK_ColorRED : SK_ColorGREEN); // low refresh rate 60
    brush.SetAntiAlias(true);
    RSAutoCanvasRestore acr(&canvas);
    canvas.ResetMatrix();
    canvas.AttachBrush(brush);
    if (!RefreshRateRotationProcess(canvas, params->GetScreenRotation(),
        static_cast<uint32_t>(params->GetBounds().GetWidth()),
        static_cast<uint32_t>(params->GetBounds().GetHeight()))) {
        return;
    }
    // 100.f:Scalar x of drawing TextBlob; 200.f:Scalar y of drawing TextBlob
    canvas.DrawTextBlob(textBlob.get(), 100.f, 200.f);
    canvas.DetachBrush();
}

bool RSRefreshRateDfx::RefreshRateRotationProcess(RSPaintFilterCanvas& canvas,
    ScreenRotation rotation, uint32_t translateWidth, uint32_t translateHeight)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(logicalDisplayParams_.get());
    auto screenManager = CreateOrGetScreenManager();
    if (UNLIKELY(params == nullptr || screenManager == nullptr)) {
        return false;
    }
    Drawing::Matrix invertMatrix;
    if (params->GetNeedOffscreen() && params->GetMatrix().Invert(invertMatrix)) {
        canvas.ConcatMatrix(invertMatrix);
    }
    auto screenId = params->GetScreenId();
    auto screenCorrection = screenManager->GetScreenCorrection(screenId);
    if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
        screenCorrection != ScreenRotation::ROTATION_0) {
        // Recaculate rotation if mirrored screen has additional rotation angle
        rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
            - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
    }

    if (rotation != ScreenRotation::ROTATION_0) {
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas.Rotate(-90, 0, 0); // 90 degree for text draw
            canvas.Translate(-(static_cast<float>(translateWidth)), 0);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            // 180 degree for text draw
            canvas.Rotate(-180, static_cast<float>(translateWidth) / 2, // 2 half of screen width
                static_cast<float>(translateHeight) / 2);                 // 2 half of screen height
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas.Rotate(-270, 0, 0); // 270 degree for text draw
            canvas.Translate(0, -(static_cast<float>(translateHeight)));
        } else {
            return false;
        }
    }
    return true;
}
} // namespace OHOS::Rosen
