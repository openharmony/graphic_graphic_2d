/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "sk_image_chain_unittest.h"
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

/**
 * @tc.name: DrawTest001
 * @tc.desc: draw nothing
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest001 start";

    // create from PixelMap
    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->Draw();
    skImageChain->ForceCPU(false);
    skImageChain->Draw();
}

/**
 * @tc.name: DrawTest002
 * @tc.desc: create a SKImageChain with PixelMap and draw
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest002 start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->Draw();
}

/**
 * @tc.name: GetPixelMapTest001
 * @tc.desc: test GetPixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, GetPixelMapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest GetPixelMapTest001 start";

    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    ASSERT_EQ(skImageChain-> GetPixelMap(), nullptr);
}

/**
 * @tc.name: SetFiltersTest001
 * @tc.desc: test SetFilters
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetTest001, TestSize.Level1)
{
    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    ASSERT_NE(filterBlur, nullptr);
    skImageChain->SetFilters(filterBlur);
    
    auto filterBrightness = Rosen::SKImageFilterFactory::Brightness(0.5);
    ASSERT_NE(filterBrightness, nullptr);
    skImageChain->SetFilters(filterBrightness);
}

/**
 * @tc.name: SetClipTest001
 * @tc.desc: test Set Clip Functions
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetClipTest001, TestSize.Level1)
{
    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    SkRect rect = SkRect::MakeEmpty();
    SkPath *path = new SkPath();
    SkRRect *rRect = new SkRRect();
    skImageChain->SetClipRect(&rect);
    skImageChain->SetClipPath(path);
    skImageChain->SetClipRRect(rRect);
    delete path;
    delete rRect;
}

} // namespace Rosen
} // namespace OHOS