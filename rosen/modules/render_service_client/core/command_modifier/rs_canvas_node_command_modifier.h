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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_COMMAND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_COMMAND_MODIFIER_H

#include "command_modifier/rs_command_modifier.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
class DrawCmdList;
using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
}

class RSCanvasNode;

struct HdrPresentCmdParam {
    bool hdrPresent_;
};

class HdrPresentCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HDR_PRESENT;

    HdrPresentCmdModifier(std::weak_ptr<RSNode> node, const HdrPresentCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HdrPresentCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const HdrPresentCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HdrPresentCmdParam param_;
};

struct ColorGamutCmdParam {
    uint32_t colorGamut_;
};

class ColorGamutCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::COLOR_GAMUT;

    ColorGamutCmdModifier(std::weak_ptr<RSNode> node, const ColorGamutCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ColorGamutCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ColorGamutCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ColorGamutCmdParam param_;
};

struct IsFreezeCmdParam {
    bool isFreeze_;
    bool isMarkedByUI_;
};

class IsFreezeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::IS_FREEZE;

    IsFreezeCmdModifier(std::weak_ptr<RSNode> node, const IsFreezeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const IsFreezeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const IsFreezeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    IsFreezeCmdParam param_;
};

struct ClearRecordingCmdParam {
    int32_t width_;
    int32_t height_;
};

class ClearRecordingCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::CLEAR_RECORDING;

    ClearRecordingCmdModifier(std::weak_ptr<RSNode> node, const ClearRecordingCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const ClearRecordingCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const ClearRecordingCmdParam& GetParam() const
    {
        return param_;
    }

private:
    ClearRecordingCmdParam param_;
};

struct FinishRecordCmdParam {
    Drawing::DrawCmdListPtr drawCmdList_;
    uint16_t modifierType_;
};

class FinishRecordCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::FINISH_RECORD;

    FinishRecordCmdModifier(std::weak_ptr<RSNode> node, const FinishRecordCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const FinishRecordCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const FinishRecordCmdParam& GetParam() const
    {
        return param_;
    }

private:
    FinishRecordCmdParam param_;
};

struct DrawOnNodeCmdParam {
    Drawing::DrawCmdListPtr drawCmdList_;
    uint16_t modifierType_;
};

class DrawOnNodeCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::DRAW_ON_NODE;

    DrawOnNodeCmdModifier(std::weak_ptr<RSNode> node, const DrawOnNodeCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const DrawOnNodeCmdParam& param)
    {
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    RSCmdModifier::UpdateResult UpdateToRenderWithResult() override;

    void DumpParam(std::string& out) const override;

    const DrawOnNodeCmdParam& GetParam() const
    {
        return param_;
    }

private:
    DrawOnNodeCmdParam param_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_COMMAND_MODIFIER_H