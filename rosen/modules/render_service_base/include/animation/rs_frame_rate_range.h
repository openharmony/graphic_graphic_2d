/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H
#define ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H
#include <string>
#include <string_view>
#include <unordered_map>

#define RANGE_MAX_REFRESHRATE 144

namespace OHOS {
namespace Rosen {
constexpr uint32_t UNKNOWN_FRAME_RATE_TYPE = 0;
// ace scene
constexpr uint32_t REFRESH_DRAG_FRAME_RATE_TYPE = (1 << 0);
constexpr uint32_t SWIPER_DRAG_FRAME_RATE_TYPE = (1 << 1);
constexpr uint32_t SCROLLABLE_DRAG_FRAME_RATE_TYPE = (1 << 2);
constexpr uint32_t SCROLLBAR_DRAG_FRAME_RATE_TYPE = (1 << 3);
constexpr uint32_t SPLIT_DRAG_FRAME_RATE_TYPE = (1 << 4);
constexpr uint32_t PICKER_DRAG_FRAME_RATE_TYPE = (1 << 5);
constexpr uint32_t SCROLLABLE_MULTI_TASK_FRAME_RATE_TYPE = (1 << 6);
constexpr uint32_t WEB_FRAME_RATE_TYPE = (1 << 7);
constexpr uint32_t ACE_COMPONENT_FRAME_RATE_TYPE = 0b11111111;
// animator
constexpr uint32_t ANIMATION_STATE_FIRST_FRAME = (1 << 12);
constexpr uint32_t RS_ANIMATION_FRAME_RATE_TYPE = (1 << 13);
constexpr uint32_t UI_ANIMATION_FRAME_RATE_TYPE = (1 << 14);
constexpr uint32_t ANIMATION_FRAME_RATE_TYPE = (0b111 << 12);
// developer's voter
constexpr uint32_t DISPLAY_SOLOIST_FRAME_RATE_TYPE = (1 << 20);
constexpr uint32_t NATIVE_VSYNC_FRAME_RATE_TYPE = (1 << 21);
constexpr uint32_t XCOMPONENT_FRAME_RATE_TYPE = (1 << 22);
constexpr uint32_t ANIMATOR_DISPLAY_SYNC_FRAME_RATE_TYPE = (1 << 23);
constexpr uint32_t OTHER_DISPLAY_SYNC_FRAME_RATE_TYPE = (1 << 24);
constexpr uint32_t DISPLAY_SYNC_FRAME_RATE_TYPE = (0b111 << 22);

constexpr uint32_t FRAME_RATE_TYPE_MAX_BIT = 32;

const inline std::string_view SWIPER_DRAG_SCENE = "swiper_drag_scene";

enum ComponentScene : int32_t {
    UNKNOWN_SCENE = 0,
    SWIPER_FLING = 1,
};

class FrameRateRange {
public:
    FrameRateRange() : min_(0), max_(0), preferred_(0), type_(0), isEnergyAssurance_(false),
        componentScene_(ComponentScene::UNKNOWN_SCENE) {}

    FrameRateRange(int min, int max, int preferred) : min_(min), max_(max), preferred_(preferred) {}

    FrameRateRange(int min, int max, int preferred, uint32_t type) : min_(min), max_(max),
        preferred_(preferred), type_(type) {}

    FrameRateRange(int min, int max, int preferred, uint32_t type, ComponentScene componentScene)
        : min_(min), max_(max), preferred_(preferred), type_(type), componentScene_(componentScene) {}

    bool IsZero() const
    {
        return this->preferred_ == 0;
    }

    bool IsValid() const
    {
        return !this->IsZero() && this->min_ <= this->preferred_ && this->preferred_ <= this->max_ &&
            this->min_ >= 0 && this->max_ <= RANGE_MAX_REFRESHRATE;
    }

    bool IsDynamic() const
    {
        return IsValid() && this->min_ != this->max_;
    }

    void Reset()
    {
        this->min_ = 0;
        this->max_ = 0;
        this->preferred_ = 0;
        this->type_ = UNKNOWN_FRAME_RATE_TYPE;
        this->isEnergyAssurance_ = false;
        this->componentScene_ = ComponentScene::UNKNOWN_SCENE;
    }

    void Set(int min, int max, int preferred)
    {
        this->min_ = min;
        this->max_ = max;
        this->preferred_ = preferred;
    }

    void Set(int min, int max, int preferred, uint32_t type)
    {
        this->min_ = min;
        this->max_ = max;
        this->preferred_ = preferred;
        this->type_ = type;
    }

    bool Merge(const FrameRateRange& other)
    {
        if (other.IsValid()) {
            type_ |= other.type_;
        }
        if (this->preferred_ < other.preferred_) {
            this->Set(other.min_, other.max_, other.preferred_);
            this->isEnergyAssurance_ = other.isEnergyAssurance_;
            this->componentScene_ = other.componentScene_;
            return true;
        }
        if (this->preferred_ == other.preferred_ && other.componentScene_ == ComponentScene::SWIPER_FLING) {
            this->componentScene_ = other.componentScene_;
        }
        return false;
    }

    std::string GetExtInfo() const
    {
        auto componentName = GetComponentName();
        if (isEnergyAssurance_ && componentName != "UNKNOWN_SCENE") {
            return std::string("COMPONENT_") + componentName + "_ASSURANCE";
        }
        std::string extInfo = "";
        if (type_ & RS_ANIMATION_FRAME_RATE_TYPE) {
            extInfo = "RS_ANIMATION";
        } else if (type_ & UI_ANIMATION_FRAME_RATE_TYPE) {
            extInfo = "UI_ANIMATION";
        } else if (type_ & DISPLAY_SYNC_FRAME_RATE_TYPE) {
            extInfo = "DISPLAY_SYNC";
        } else if (type_ & ACE_COMPONENT_FRAME_RATE_TYPE) {
            extInfo = "ACE_COMPONENT";
        } else if (type_ & DISPLAY_SOLOIST_FRAME_RATE_TYPE) {
            extInfo = "DISPLAY_SOLOIST";
        } else if (type_ & NATIVE_VSYNC_FRAME_RATE_TYPE) {
            extInfo = "NATIVE_VSYNC";
        }
        if ((type_ & ANIMATION_STATE_FIRST_FRAME) != 0) {
            extInfo += "_FIRST_UI_ANIMATION_FRAME";
        }
        return extInfo + (isEnergyAssurance_ ? "_ENERGY_ASSURANCE" : "");
    }

    static std::string FrameRateType2Str(uint32_t frameRateType)
    {
        static const std::unordered_map<uint32_t, std::string> frameRateTypesMap = {
            { REFRESH_DRAG_FRAME_RATE_TYPE, "REFRESH_DRAG" },
            { SWIPER_DRAG_FRAME_RATE_TYPE, "SWIPER_DRAG" },
            { SCROLLABLE_DRAG_FRAME_RATE_TYPE, "SCROLLABLE_DRAG" },
            { SCROLLBAR_DRAG_FRAME_RATE_TYPE, "SCROLLBAR_DRAG" },
            { SPLIT_DRAG_FRAME_RATE_TYPE, "SPLIT_DRAG" },
            { PICKER_DRAG_FRAME_RATE_TYPE, "PICKER_DRAG" },
            { SCROLLABLE_MULTI_TASK_FRAME_RATE_TYPE, "SCROLLABLE_MULTI_TASK" },
            { WEB_FRAME_RATE_TYPE, "WEB_FRAME_RATE_TYPE" },
            { ANIMATION_STATE_FIRST_FRAME, "ANIMATION_STATE_FIRST_FRAME" },
            { RS_ANIMATION_FRAME_RATE_TYPE, "RS_ANIMATION" },
            { UI_ANIMATION_FRAME_RATE_TYPE, "UI_ANIMATION" },
            { DISPLAY_SOLOIST_FRAME_RATE_TYPE, "DISPLAY_SOLOIST" },
            { NATIVE_VSYNC_FRAME_RATE_TYPE, "NATIVE_VSYNC" },
            { XCOMPONENT_FRAME_RATE_TYPE, "XCOMPONENT" },
            { ANIMATOR_DISPLAY_SYNC_FRAME_RATE_TYPE, "ANIMATOR_DISPLAY_SYNC" },
            { OTHER_DISPLAY_SYNC_FRAME_RATE_TYPE, "OTHER_DISPLAY_SYNC" },
        };

        if (auto iter = frameRateTypesMap.find(frameRateType); iter != frameRateTypesMap.end()) {
            return iter->second;
        }
        return "";
    }

    std::string GetAllTypeDescription() const
    {
        std::string allTypeDescription("[" + std::to_string(type_) + "] ");
        for (uint32_t i = 0; i < FRAME_RATE_TYPE_MAX_BIT; i++) {
            if (auto typeDescription = FrameRateType2Str(type_ & (1 << i)); !typeDescription.empty()) {
                allTypeDescription += typeDescription + ";";
            }
        }
        return allTypeDescription;
    }

    std::string GetComponentName() const
    {
        switch (componentScene_) {
            case ComponentScene::SWIPER_FLING:
                return "SWIPER_FLING";
            default:
                return "UNKNOWN_SCENE";
        }
    }

    bool operator==(const FrameRateRange& other) const
    {
        return this->min_ == other.min_ && this->max_ == other.max_ &&
            this->preferred_ == other.preferred_ && this->type_ == other.type_;
    }

    bool operator!=(const FrameRateRange& other) const
    {
        return !operator==(other);
    }

    int min_ = 0;
    int max_ = 0;
    int preferred_ = 0;
    uint32_t type_ = UNKNOWN_FRAME_RATE_TYPE;
    bool isEnergyAssurance_ = false;
    ComponentScene componentScene_ = ComponentScene::UNKNOWN_SCENE;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H