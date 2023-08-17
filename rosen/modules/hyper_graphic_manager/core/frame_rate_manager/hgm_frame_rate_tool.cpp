/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_frame_rate_tool.h"

namespace OHOS::Rosen {
std::once_flag g_createFlag;
std::shared_ptr<HgmFrameRateTool> instance_ = nullptr;

std::shared_ptr<HgmFrameRateTool> HgmFrameRateTool::GetInstance()
{
    std::call_once(g_createFlag, [&] {
        instance_ = std::shared_ptr<HgmFrameRateTool>(new HgmFrameRateTool());
    });
    return instance_;
}

HgmFrameRateTool::HgmFrameRateTool()
{
}

HgmFrameRateTool::~HgmFrameRateTool() noexcept
{
}

int32_t HgmFrameRateTool::AddScreenProfile(
    ScreenId id, int32_t width, int32_t height, int32_t phyWidth, int32_t phyHeight)
{
    auto screenProfile = std::make_shared<ScreenProfile>(id, width, height, phyWidth, phyHeight);
    std::lock_guard<std::mutex> lock(listMutex_);
    screenProfileList_.push_back(screenProfile);
    return EXEC_SUCCESS;
}

int32_t HgmFrameRateTool::RemoveScreenProfile(ScreenId id)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    for (auto screenProfile = screenProfileList_.begin(); screenProfile != screenProfileList_.end(); ++screenProfile) {
        if ((*screenProfile)->GetId() == id) {
            screenProfileList_.erase(screenProfile);
            break;
        }
    }
    return EXEC_SUCCESS;
}

std::shared_ptr<HgmFrameRateTool::ScreenProfile> HgmFrameRateTool::GetScreenProfile(ScreenId id) const
{
    for (auto screenProfile = screenProfileList_.begin(); screenProfile != screenProfileList_.end(); ++screenProfile) {
        if ((*screenProfile)->GetId() == id) {
            return *screenProfile;
        }
    }
    return nullptr;
}

int32_t HgmFrameRateTool::CalModifierPreferred(
    ScreenId id, HgmModifierProfile &hgmModifierProfile, std::shared_ptr<ParsedConfigData> parsedConfigData) const
{
    auto screenProfile = GetScreenProfile(id);
    if (!screenProfile) {
        return HGM_ERROR;
    }
    auto [xSpeed, ySpeed] = applyDimension(
        SpeedTransType::TRANS_PIXEL_TO_MM, hgmModifierProfile.xSpeed, hgmModifierProfile.ySpeed, screenProfile);
    auto mixSpeed = sqrt(xSpeed * xSpeed + ySpeed * ySpeed);

    auto dynamicSettingMap = parsedConfigData->GetAnimationDynamicSettingMap(hgmModifierProfile.hgmModifierType);
    for (const auto &iter: dynamicSettingMap) {
        if (mixSpeed >= iter.second.min && (mixSpeed < iter.second.max || iter.second.max == -1)) {
            return iter.second.preferred_fps;
        }
    }
    return HGM_ERROR;
}

std::pair<float, float> HgmFrameRateTool::applyDimension(
    SpeedTransType speedTransType, float xSpeed, float ySpeed, std::shared_ptr<ScreenProfile> screenProfile)
{
    switch (speedTransType) {
        case SpeedTransType::TRANS_MM_TO_PIXEL:
            return std::pair<float, float>(
                xSpeed * screenProfile->GetXDpi() / INCH_2_MM, ySpeed * screenProfile->GetYDpi() / INCH_2_MM);
        case SpeedTransType::TRANS_PIXEL_TO_MM:
            return std::pair<float, float>(
                xSpeed / screenProfile->GetXDpi() * INCH_2_MM, ySpeed / screenProfile->GetYDpi() * INCH_2_MM);
        default:
            return std::pair<float, float>(0, 0);
    }
}

} // namespace OHOS::Rosen