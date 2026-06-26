/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "animation/rs_keyframe_animation.h"
#include "rs_animation_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSKeyframeAnimationTest : public testing::Test {
};

/**
 * @tc.name: RebuildInRender001
 * @tc.desc: Verify RebuildInRender with empty keyframes
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RebuildInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRender001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATIONTEST::ANIMATION_START_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(rsUIContext, property);
    keyframeAnimation->SetRebuildParam({0.5f, false});
    keyframeAnimation->RebuildInRender();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RebuildInRender001 end";
}

} // namespace Rosen
} // namespace OHOS
