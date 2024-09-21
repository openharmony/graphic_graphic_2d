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

#include "cmd_list_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "recording/cmd_list.h"
#include "recording/draw_cmd_list.h"

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATH_TEN = 10;
constexpr size_t ARRAY_MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void CmdListFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % ARRAY_MAX_SIZE + 1;
    size_t arrSize = GetObject<size_t>() % MATH_TEN;
    uint32_t offset = GetObject<uint32_t>();

    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    obj[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = obj;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = DrawCmdList::CreateFromData(cmdListData, false);

    cmdList->GetType();
    cmdList->AddCmdListData(cmdListData);
    cmdList->GetCmdListData(offset);
    cmdList->GetData();
    cmdList->SetUpImageData(obj, arrSize);
    cmdList->AddImageData(obj, arrSize);
    cmdList->GetImageData(offset);
    cmdList->GetAllImageData();
    cmdList->AddBitmapData(obj, arrSize);
    cmdList->GetBitmapData(offset);
    cmdList->SetUpBitmapData(obj, arrSize);
    cmdList->GetAllBitmapData();
    std::shared_ptr<ExtendObject> object = cmdList->GetExtendObject(offset);
    cmdList->AddExtendObject(object);
    std::vector<std::shared_ptr<ExtendObject>> objectList = { object, object };
    cmdList->GetAllExtendObject(objectList);
    cmdList->SetupExtendObject(objectList);
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void CmdListFuzzTest001(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % ARRAY_MAX_SIZE + 1;
    uint32_t id = GetObject<uint32_t>();
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % ARRAY_MAX_SIZE;
    int32_t height = GetObject<int32_t>() % ARRAY_MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    obj[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = obj;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = DrawCmdList::CreateFromData(cmdListData, false);
    std::shared_ptr<ExtendImageObject> object = cmdList->GetImageObject(id);
    cmdList->AddImageObject(object);
    std::vector<std::shared_ptr<ExtendImageObject>> objectList;
    cmdList->GetAllObject(objectList);
    cmdList->SetupObject(objectList);
    std::shared_ptr<ExtendImageBaseObj> object1 = cmdList->GetImageBaseObj(id);
    cmdList->AddImageBaseObj(object1);
    std::vector<std::shared_ptr<ExtendImageBaseObj>> objList;
    cmdList->GetAllBaseObj(objList);
    cmdList->SetupBaseObj(objList);
    std::shared_ptr<ExtendDrawFuncObj> object2 = cmdList->GetDrawFuncObj(id);
    cmdList->AddDrawFuncOjb(object2);
    cmdList->GetOpCnt();
    cmdList->CopyObjectTo(*cmdList);
    OpDataHandle imageHandle = cmdList->AddImage(image);
    cmdList->GetImage(imageHandle);
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::CmdListFuzzTest000(data, size);
    OHOS::Rosen::Drawing::CmdListFuzzTest001(data, size);
    return 0;
}