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

#include "modifier/rs_cmd_list_image_collector.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<CmdListImage> RSCmdListImageCollector::CollectCmdListImage(
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    if (drawCmdList == nullptr) {
        return nullptr;
    }

    std::vector<std::shared_ptr<Drawing::ExtendImageObject>> imageObjectVec;
    std::vector<std::shared_ptr<Drawing::ExtendImageBaseObj>> imageBaseObjVec;
    std::vector<std::shared_ptr<Drawing::ExtendImageNineObject>> imageNineObjectVec;
    std::vector<std::shared_ptr<Drawing::ExtendImageLatticeObject>> imageLatticeObjectVec;
    drawCmdList->GetAllObject(imageObjectVec);
    drawCmdList->GetAllBaseObj(imageBaseObjVec);
    drawCmdList->GetAllImageNineObject(imageNineObjectVec);
    drawCmdList->GetAllImageLatticeObject(imageLatticeObjectVec);

    auto cmdListImages = std::make_unique<CmdListImage>();
    bool hasImages = false;
    if (!imageObjectVec.empty()) {
        cmdListImages->imageObjectVec =
            std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageObject>>>(std::move(imageObjectVec));
        hasImages = true;
    }
    if (!imageBaseObjVec.empty()) {
        cmdListImages->imageBaseObjVec =
            std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageBaseObj>>>(std::move(imageBaseObjVec));
        hasImages = true;
    }
    if (!imageNineObjectVec.empty()) {
        cmdListImages->imageNineObjectVec =
            std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageNineObject>>>(
                std::move(imageNineObjectVec));
        hasImages = true;
    }
    if (!imageLatticeObjectVec.empty()) {
        cmdListImages->imageLatticeObjectVec =
            std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageLatticeObject>>>(
                std::move(imageLatticeObjectVec));
        hasImages = true;
    }
    if (hasImages) {
        return cmdListImages;
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS