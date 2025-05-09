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

#ifndef RENDER_SERVICE_TEST_TREE_H
#define RENDER_SERVICE_TEST_TREE_H

#include <memory>

#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
class TestTreeBuilder {
public:
    TestTreeBuilder();
    std::vector<std::shared_ptr<RSRenderNode>> Build(RSContext& context, NodeId topId, bool withDisplay);

private:
    inline static const int startX = 200;
    inline static const int startY = 1200;
    inline static const int width = 900;
    inline static const int height = 900;
    inline static const int width13 = width / 3;
    inline static const int width23 = 2 * width / 3;
    inline static const int height13 = height / 3;
    inline static const int height23 = 2 * height / 3;
    inline static const int zero = 0;
    inline static const int one = 1;
    inline static const int two = 2;
    inline static const int three = 3;
    inline static const int four = 4;
    inline static const int five = 5;
    inline static const int visibleZPosition = 5000;
    const Color almostWhite_ = Color(240, 240, 240, 255);
    std::mt19937 mt_;
    std::uniform_int_distribution<uint8_t> distribution_;
    Drawing::Image GenerateRandomImage(int width, int height);
    NodeId insideId_;
    bool withDisplay_;
    void CreateNode00(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode01(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode02(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode03(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode04(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode05(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode06(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode07(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
    void CreateNode08(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree);
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_TEST_TREE_H