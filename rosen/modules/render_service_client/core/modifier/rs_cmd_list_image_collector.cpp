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
    if (drawCmdList == nullptr || drawCmdList->IsEmpty()) {
        return nullptr;
    }

    auto cmdListImages = std::make_unique<CmdListImage>();
    cmdListImages->imageObjectVec = std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageObject>>>();
    cmdListImages->imageBaseObjVec = std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageBaseObj>>>();
    cmdListImages->imageNineObjectVec =
        std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageNineObject>>>();
    cmdListImages->imageLatticeObjectVec =
        std::make_unique<std::vector<std::shared_ptr<Drawing::ExtendImageLatticeObject>>>();
    drawCmdList->GetAllObject(*cmdListImages->imageObjectVec);
    drawCmdList->GetAllBaseObj(*cmdListImages->imageBaseObjVec);
    drawCmdList->GetAllImageNineObject(*cmdListImages->imageNineObjectVec);
    drawCmdList->GetAllImageLatticeObject(*cmdListImages->imageLatticeObjectVec);

    if (cmdListImages->imageObjectVec->empty()) {
        cmdListImages->imageObjectVec = nullptr;
    }
    if (cmdListImages->imageBaseObjVec->empty()) {
        cmdListImages->imageBaseObjVec = nullptr;
    }
    if (cmdListImages->imageNineObjectVec->empty()) {
        cmdListImages->imageNineObjectVec = nullptr;
    }
    if (cmdListImages->imageLatticeObjectVec->empty()) {
        cmdListImages->imageLatticeObjectVec = nullptr;
    }
    if (cmdListImages->imageObjectVec || cmdListImages->imageBaseObjVec || cmdListImages->imageNineObjectVec ||
        cmdListImages->imageLatticeObjectVec) {
        return cmdListImages;
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS