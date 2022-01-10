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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H

#include "ui/rs_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RSNodeMap;

class RS_EXPORT RSCanvasNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasNode>;
    using SharedPtr = std::shared_ptr<RSCanvasNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::CANVAS_NODE;

    virtual ~RSCanvasNode();

    static SharedPtr Create(bool isRenderServiceNode = false);

    SkCanvas* BeginRecording(int width, int height);
    bool IsRecording() const;
    void FinishRecording();

    RSUINodeType GetType() const override
    {
        return RSUINodeType::CANVAS_NODE;
    }

protected:
    RSCanvasNode(bool isRenderServiceNode);
    RSCanvasNode(const RSCanvasNode&) = delete;
    RSCanvasNode(const RSCanvasNode&&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&&) = delete;

private:
    SkCanvas* recordingCanvas_ = nullptr;

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    template<typename T>
    friend class RSPropertyAnimation;
    friend class RSNodeMap;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
