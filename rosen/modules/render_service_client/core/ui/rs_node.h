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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H

#include "pipeline/rs_recording_canvas.h"
#include "ui/rs_property_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RSNodeMap;

class RS_EXPORT RSNode : public RSPropertyNode {
public:
    using WeakPtr = std::weak_ptr<RSNode>;
    using SharedPtr = std::shared_ptr<RSNode>;
    virtual ~RSNode();

    static SharedPtr Create();

    SkCanvas* BeginRecording(int width, int height);
    bool IsRecording() const;
    void SetPaintOrder(bool drawContentLast);

protected:
    RSNode();
    RSNode(const RSNode&) = delete;
    RSNode(const RSNode&&) = delete;
    RSNode& operator=(const RSNode&) = delete;
    RSNode& operator=(const RSNode&&) = delete;

private:
    RSNode(NodeId id);
    void OnUpdateRecording();
    void UpdateRecording();

    SkCanvas* recordingCanvas_ = nullptr;
    bool drawContentLast_ = false;

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    template<typename T>
    friend class RSPropertyAnimation;
    friend class RSNodeMap;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
