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

#include "cmdlisthelper_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/mask_cmd_list.h"
#include "recording/record_cmd.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t FUNCTYPE_SIZE = 4;
constexpr size_t MATRIXTYPE_SIZE = 5;
constexpr size_t MAX_SIZE = 5000;
} // namespace
namespace Drawing {
/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddImageToCmdList
 * 2. GetImageFromCmdList
 */
bool CmdListHelperFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    ImageInfo imageInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_UNPREMUL };
    std::shared_ptr<Image> image = Image::MakeRasterData(imageInfo, dataVal, length);
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    CmdListHelper::AddImageToCmdList(*cmdList, image);
    OpDataHandle opDataHandle { GetObject<uint32_t>(), length };
    CmdListHelper::GetImageFromCmdList(*cmdList, opDataHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddVerticesToCmdList
 * 2. GetVerticesFromCmdList
 */
bool CmdListHelperFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    Vertices vertices;
    VertexMode vertexMode = GetObject<VertexMode>();
    uint32_t arr_size = GetObject<uint32_t>() % MAX_SIZE;
    Point* positions = new Point[arr_size];
    Point* texs = new Point[arr_size];
    ColorQuad* colors = new ColorQuad[arr_size];
    uint16_t* indices = new uint16_t[arr_size];
    for (size_t i = 0; i < arr_size; i++) {
        positions[i] = { GetObject<scalar>(), GetObject<scalar>() };
        texs[i] = { GetObject<scalar>(), GetObject<scalar>() };
        colors[i] = GetObject<ColorQuad>();
        indices[i] = GetObject<uint16_t>();
    }
    vertices.MakeCopy(vertexMode, arr_size, positions, texs, colors, arr_size, indices);
    CmdListHelper::AddVerticesToCmdList(*cmdList, vertices);
    OpDataHandle opDataHandle { GetObject<uint32_t>(), GetObject<size_t>() };
    CmdListHelper::GetVerticesFromCmdList(*cmdList, opDataHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (positions != nullptr) {
        delete [] positions;
        positions = nullptr;
    }
    if (texs != nullptr) {
        delete [] texs;
        texs = nullptr;
    }
    if (colors != nullptr) {
        delete [] colors;
        colors = nullptr;
    }
    if (indices != nullptr) {
        delete [] indices;
        indices = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddBitmapToCmdList
 * 2. GetBitmapFromCmdList
 */
bool CmdListHelperFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    CmdListHelper::AddBitmapToCmdList(*cmdList, bitmap);
    ImageHandle imageHandle { GetObject<uint32_t>(), length, GetObject<int32_t>(), GetObject<int32_t>() };
    CmdListHelper::GetBitmapFromCmdList(*cmdList, imageHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. GetRecordCmdFromCmdList
 * 2. AddImageObjectToCmdList
 * 3. GetImageObjectFromCmdList
 * 4. AddImageBaseObjToCmdList
 * 5. GetImageBaseObjFromCmdList
 * 6. AddPictureToCmdList
 * 7. GetPictureFromCmdList
 */
bool CmdListHelperFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    OpDataHandle recordCmdHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::GetRecordCmdFromCmdList(*cmdList, recordCmdHandle);
    uint32_t id = GetObject<uint32_t>();
    std::shared_ptr<ExtendImageObject> object = cmdList->GetImageObject(id);
    OpDataHandle objectHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddImageObjectToCmdList(*cmdList, object);
    CmdListHelper::GetImageObjectFromCmdList(*cmdList, objectHandle);
    std::shared_ptr<ExtendImageBaseObj> objectT = cmdList->GetImageBaseObj(id);
    OpDataHandle objectTHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddImageBaseObjToCmdList(*cmdList, objectT);
    CmdListHelper::GetImageBaseObjFromCmdList(*cmdList, objectTHandle);
    Picture picture;
    auto dataVal = std::make_shared<Data>();
    dataVal->BuildWithoutCopy(dataText, length);
    picture.Deserialize(dataVal);
    picture.Serialize();
    OpDataHandle pictureHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddPictureToCmdList(*cmdList, picture);
    CmdListHelper::GetPictureFromCmdList(*cmdList, pictureHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddCompressDataToCmdList
 * 2. GetCompressDataFromCmdList
 * 3. AddDrawFuncObjToCmdList
 * 4. GetDrawFuncObjFromCmdList
 */
bool CmdListHelperFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    OpDataHandle imageHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddCompressDataToCmdList(*cmdList, dataVal);
    CmdListHelper::GetCompressDataFromCmdList(*cmdList, imageHandle);
    uint32_t id = GetObject<uint32_t>();
    std::shared_ptr<ExtendDrawFuncObj> object = cmdList->GetDrawFuncObj(id);
    uint32_t objectHandle = GetObject<uint32_t>();
    CmdListHelper::AddDrawFuncObjToCmdList(*cmdList, object);
    CmdListHelper::GetDrawFuncObjFromCmdList(*cmdList, objectHandle);

    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddTextBlobToCmdList
 * 2. GetTextBlobFromCmdList
 */
bool CmdListHelperFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 2;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    auto textBlob = TextBlob::MakeFromString(text, font, TextEncoding::UTF8);
    if (textBlob) {
        OpDataHandle textBlobHandle = { GetObject<uint32_t>(), length };
        CmdListHelper::AddTextBlobToCmdList(*cmdList, textBlob.get(), text);
        CmdListHelper::GetTextBlobFromCmdList(*cmdList, textBlobHandle);
    }
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

/*
 * CmdListHelperFuzzTest007(const uint8_t* data, size_t size)
 * 测试以下 CmdListHelper 接口：
 * 1. AddDataToCmdList
 * 2. GetDataFromCmdList
 * 3. AddPathToCmdList
 * 4. GetPathFromCmdList
 */
bool CmdListHelperFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Data dataVal;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal.BuildWithoutCopy(dataText, length);
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    OpDataHandle dataHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddDataToCmdList(*cmdList, &dataVal);
    CmdListHelper::GetDataFromCmdList(*cmdList, dataHandle);
    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* str = new char[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] = GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    OpDataHandle pathHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddPathToCmdList(*cmdList, path);
    CmdListHelper::GetPathFromCmdList(*cmdList, pathHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddRegionToCmdList
 * 2. GetRegionFromCmdList
 * 3. AddColorSpaceToCmdList
 * 4. GetColorSpaceFromCmdList
 * 5. AddShaderEffectToCmdList
 * 6. GetShaderEffectFromCmdList
 */
bool CmdListHelperFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    Region region;
    RectI rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    region.SetRect(rect);
    OpDataHandle regionHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddRegionToCmdList(*cmdList, region);
    CmdListHelper::GetRegionFromCmdList(*cmdList, regionHandle);
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    OpDataHandle colorSpaceHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddColorSpaceToCmdList(*cmdList, colorSpace);
    CmdListHelper::GetColorSpaceFromCmdList(*cmdList, colorSpaceHandle);
    ColorQuad color = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    FlattenableHandle shaderEffectHandle = { GetObject<uint32_t>(), length, GetObject<uint32_t>() };
    CmdListHelper::AddShaderEffectToCmdList(*cmdList, shaderEffect);
    CmdListHelper::GetShaderEffectFromCmdList(*cmdList, shaderEffectHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddPathEffectToCmdList
 * 2. GetPathEffectFromCmdList
 * 3. AddMaskFilterToCmdList
 * 4. GetMaskFilterFromCmdList
 * 5. AddColorFilterToCmdList
 * 6. GetColorFilterFromCmdList
 */
bool CmdListHelperFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    scalar radius = GetObject<scalar>();
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateCornerPathEffect(radius);
    FlattenableHandle pathEffectHandle = { GetObject<uint32_t>(), length, GetObject<uint32_t>() };
    CmdListHelper::AddPathEffectToCmdList(*cmdList, pathEffect);
    CmdListHelper::GetPathEffectFromCmdList(*cmdList, pathEffectHandle);
    BlurType blurType = GetObject<BlurType>();
    float sigma = GetObject<float>();
    bool respectCTM = GetObject<bool>();
    std::shared_ptr<MaskFilter> maskFilter = MaskFilter::CreateBlurMaskFilter(blurType, sigma, respectCTM);
    FlattenableHandle maskFilterHandle = { GetObject<uint32_t>(), length, GetObject<uint32_t>() };
    CmdListHelper::AddMaskFilterToCmdList(*cmdList, maskFilter);
    CmdListHelper::GetMaskFilterFromCmdList(*cmdList, maskFilterHandle);
    ColorQuad colorQuad = GetObject<ColorQuad>();
    BlendMode mode = GetObject<BlendMode>();
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateBlendModeColorFilter(colorQuad, mode);
    FlattenableHandle colorFilterHandle = { GetObject<uint32_t>(), length, GetObject<uint32_t>() };
    CmdListHelper::AddColorFilterToCmdList(*cmdList, colorFilter);
    CmdListHelper::GetColorFilterFromCmdList(*cmdList, colorFilterHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddImageFilterToCmdList
 * 2. GetImageFilterFromCmdList
 * 3. AddBlurDrawLooperToCmdList
 * 4. GetBlurDrawLooperFromCmdList
 * 5. AddLatticeToCmdList
 * 6. GetLatticeFromCmdList
 */
bool CmdListHelperFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateColorBlurImageFilter(*colorFilter, sigmaX, sigmaY);
    FlattenableHandle imageFilterHandle = { GetObject<uint32_t>(), length, GetObject<uint32_t>() };
    CmdListHelper::AddImageFilterToCmdList(*cmdList, imageFilter);
    CmdListHelper::GetImageFilterFromCmdList(*cmdList, imageFilterHandle);
    float blurRadius = GetObject<float>();
    float dx = GetObject<float>();
    float dy = GetObject<float>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, dx, dy, color);
    OpDataHandle blurDrawLooperHandle = { GetObject<uint32_t>(), length };
    CmdListHelper::AddBlurDrawLooperToCmdList(*cmdList, blurDrawLooper);
    CmdListHelper::GetBlurDrawLooperFromCmdList(*cmdList, blurDrawLooperHandle);
    Lattice lattice;
    lattice.fXCount = GetObject<int>();
    lattice.fYCount = GetObject<int>();
    LatticeHandle latticeHandle;
    latticeHandle.fXCount = GetObject<int>();
    latticeHandle.fYCount = GetObject<int>();
    CmdListHelper::AddLatticeToCmdList(*cmdList, lattice);
    CmdListHelper::GetLatticeFromCmdList(*cmdList, latticeHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddSymbolToCmdList
 * 2. GetSymbolFromCmdList
 * 3. AddSymbolLayersToCmdList
 * 4. GetSymbolLayersFromCmdList
 */
bool CmdListHelperFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    DrawingSymbolLayers symbolInfo { GetObject<uint32_t>() };
    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* str = new char[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] = GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    uint64_t symbolId = GetObject<uint64_t>();
    DrawingHMSymbolData symbol { symbolInfo, path, symbolId };
    SymbolLayersHandle symbolLayerHandle { GetObject<uint32_t>() };
    OpDataHandle pathHandle { GetObject<uint32_t>(), length };
    SymbolOpHandle symbolHandle { symbolLayerHandle, pathHandle, symbolId };
    CmdListHelper::AddSymbolToCmdList(*cmdList, symbol);
    CmdListHelper::GetSymbolFromCmdList(*cmdList, symbolHandle);
    CmdListHelper::AddSymbolLayersToCmdList(*cmdList, symbolInfo);
    CmdListHelper::GetSymbolLayersFromCmdList(*cmdList, symbolLayerHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    return true;
}

/*
 * 测试以下 CmdListHelper 接口：
 * 1. AddRenderGroupToCmdList
 * 2. AddGroupInfoToCmdList
 * 3. AddExtendObjectToCmdList
 * 4. GetExtendObjectFromCmdList
 */
bool CmdListHelperFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    CmdListData cmdListData;
    cmdListData.first = dataText;
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    std::vector<size_t> layerIndexes { GetObject<size_t>() % MAX_SIZE };
    std::vector<size_t> maskIndexes { GetObject<size_t>() % MAX_SIZE };
    DrawingGroupInfo groupInfo { layerIndexes, maskIndexes };
    std::vector<DrawingGroupInfo> groupInfos { groupInfo };
    DrawingSColor color { GetObject<float>(), GetObject<U8CPU>(), GetObject<U8CPU>(), GetObject<U8CPU>() };
    DrawingRenderGroup group { groupInfos, color };
    CmdListHelper::AddRenderGroupToCmdList(*cmdList, group);
    CmdListHelper::AddGroupInfoToCmdList(*cmdList, groupInfo);
    uint32_t offset = GetObject<uint32_t>();
    std::shared_ptr<ExtendObject> object = cmdList->GetExtendObject(offset);
    uint32_t index = GetObject<uint32_t>();
    CmdListHelper::AddExtendObjectToCmdList(*cmdList, object);
    CmdListHelper::GetExtendObjectFromCmdList(*cmdList, index);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest001(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest002(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest003(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest004(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest005(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest006(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest007(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest008(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest009(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest010(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest011(data, size);
    OHOS::Rosen::Drawing::CmdListHelperFuzzTest012(data, size);
    return 0;
}
