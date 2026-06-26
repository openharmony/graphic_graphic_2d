# 渲染资源与图片缓存

## 适用范围

- RSImage 和 RSImageBase 的图片数据模型
- RSImageCache 图片缓存机制（Drawing::Image、PixelMap）
- 图片压缩数据和 HDR 转换
- Drawing::Image 释放机制

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 图片基类 | `rosen/modules/render_service_base/include/render/rs_image_base.h` | RSImageBase：Marshall/Unmarshall、CanvasDrawImage |
| 图片类 | `rosen/modules/render_service_base/include/render/rs_image.h` | RSImage：ImageFit/ImageRepeat/HDR、CanvasDrawImage |
| 图片缓存 | `rosen/modules/render_service_base/include/render/rs_image_cache.h` | RSImageCache：Drawing::Image 和 PixelMap 缓存 |

## 核心模型

### RSImageBase 数据模型

RSImageBase 是图片基类，提供基础数据存储和序列化能力：

**数据源类型**：
- `pixelMap_`：`shared_ptr<Media::PixelMap>`，来自 Media 的位图数据。
- `image_`：`shared_ptr<Drawing::Image>`，GPU 纹理图片或从 PixelMap 提取的 Drawing::Image。
- `compressData_`：`shared_ptr<Drawing::Data>`，ASTC 等压缩格式数据（rs_image_base.cpp:612）。

**位置与尺寸**：
- `srcRect_` / `dstRect_`：`RectF`，源矩形和目标矩形。
- `src_` / `dst_` / `lastRect_`：`Drawing::Rect`，Drawing 层面的源/目标/上一帧矩形。

**生命周期与标识**：
- `uniqueId_`：`uint64_t`，唯一标识（pid << 32 | id），由 `GenUniqueId()` 生成（rs_image_base.cpp:573）。
- `renderServiceImage_`：`bool`，标记是否为 Render Service 侧图像，影响析构时缓存处理逻辑（rs_image_base.cpp:71）。
- `isDrawn_`：`bool`，标记是否已绘制，用于优化重复绘制（rs_image_base.cpp:241）。

**特殊图像类型**：
- `isYUVImage_`：`bool`，标记 YUV 图像，走 `ProcessYUVImage()` 处理路径（rs_image_base.cpp:579）。
- `canPurgeShareMemFlag_`：`CanPurgeFlag`，控制共享内存 purge 能力（rs_image_base.cpp:294）。

**Vulkan 相关**（rs_image_base.h:131）：
- `nativeWindowBuffer_`：`OHNativeWindowBuffer*`，用于 DMA PixelMap 的 Vulkan texture 创建。
- `tid_`：`pid_t`，创建线程 ID，用于资源释放时投递到正确线程。
- `backendTexture_`：`Drawing::BackendTexture`，Vulkan 后端纹理。
- `cleanUpHelper_`：`NativeBufferUtils::VulkanCleanupHelper*`，Vulkan 资源清理辅助。

### RSImage 数据模型

RSImage 继承 RSImageBase，增加绘制适配和变换能力：

**适配模式**：
- `imageFit_`：`ImageFit` 枚举（rs_image.h:68），包含 FILL、CONTAIN、COVER、FIT_WIDTH、FIT_HEIGHT、NONE、SCALE_DOWN、TOP_LEFT、TOP、TOP_RIGHT、LEFT、CENTER、RIGHT、BOTTOM_LEFT、BOTTOM、BOTTOM_RIGHT、COVER_TOP_LEFT、MATRIX 共 18 种模式。
- `fitMatrix_`：`optional<Drawing::Matrix>`，配合 ImageFit::MATRIX 使用的自定义变换矩阵（rs_image.h:206）。
- `isFitMatrixValid_`：`bool`，标记 fitMatrix 是否有效且非 Identity（rs_image.cpp:234）。

**重复与方向**：
- `imageRepeat_`：`ImageRepeat` 枚举（rs_image.h:61），包含 NO_REPEAT、REPEAT_X、REPEAT_Y、REPEAT。
- `orientationFit_`：`OrientationFit` 枚举（rs_image.h:89），包含 NONE、VERTICAL_FLIP、HORIZONTAL_FLIP。
- `isOrientationValid_`：`bool`，标记 orientationFit 是否有效（rs_image.cpp:236）。

**绘制参数**：
- `radius_`：`vector<Drawing::Point>`（4 个角），圆角半径，用于 `ApplyCanvasClip()`（rs_image.h:197）。
- `hasRadius_`：`bool`，标记是否有圆角（rs_image.cpp:550）。
- `innerRect_`：`optional<Drawing::RectI>`，九宫格绘制时的内部矩形（rs_image.h:198）。
- `frameRect_`：`RectF`，绘制框架矩形，限制绘制区域（rs_image.h:200）。
- `scale_`：`double`，缩放比例，用于 ApplyImageFit 计算源尺寸（rs_image.h:201）。
- `rotateDegree_`：`int32_t`，旋转角度（90/-90/180）（rs_image.h:203）。

**HDR 与节点关联**：
- `dynamicRangeMode_`：`uint32_t`，动态范围模式，用于 `HDRConvert()`（rs_image.h:205）。
- `nodeId_`：`NodeId`，节点 ID，用于 MemoryTrack 关联和窗口销毁时批量释放（rs_image.h:202）。
- `paint_`：`Drawing::Paint`，画笔属性，用于 HDR 绘制时附加 shader（rs_image.h:204）。

**异步增强**：
- `isScaledImageAsync_`：`mutable bool`，标记是否启用异步图像增强（rs_image.h:210）。

### 图片绘制流程

#### RSImageBase::DrawImage 基础绘制流程

```
RSImageBase::DrawImage(canvas, samplingOptions, constraint)
  → PixelMapUseCountGuard(pixelMap_, IsPurgeable())    // 保护 purge 过程中 pixelMap 不被 Unmap
  → DePurge()                                          // 恢复已 Unmap 的共享内存（ReMap）
  → [Vulkan DMA 分支] BindPixelMapToDrawingImage()     // DMA PixelMap → Vulkan texture（rs_image_base.cpp:138）
  → [ASTC 分支] SetCompressedDataForASTC() + UploadGpu()  // ASTC 压缩纹理上传（rs_image_base.cpp:141）
  → ConvertPixelMapToDrawingImage()                    // PixelMap → Drawing::Image（若 image_ 为空）（rs_image_base.cpp:147）
  → canvas.DrawImageRect(*image_, src, dst)            // 最终绘制（rs_image_base.cpp:155）
```

说明：PixelMapUseCountGuard 在构造时 IncreaseUseCount，析构时 DecreaseUseCount，防止 purge 过程中的 Unmap/Remap 冲突（rs_image_base.cpp:747）。

#### RSImage::CanvasDrawImage 主入口流程

**首次绘制分支**（!isDrawn_ || rect != lastRect_）：

```
CanvasDrawImage(canvas, rect, samplingOptions)
  → PixelMapUseCountGuard + DePurge                    // purge 保护（rs_image.cpp:238）
  → UpdateNodeIdToPicture(nodeId_)                     // MemoryTrack 关联（rs_image.cpp:242）
  → AutoCanvasRestore(needCanvasRestore)               // 保存 canvas 状态（radius/fitMatrix/rotateDegree）（rs_image.cpp:244）
  → frameRect_.SetAll(rect)                            // 设置框架矩形（rs_image.cpp:246）
  → ApplyImageFit()                                    // 计算适配后的 dstRect（18 种模式）（rs_image.cpp:249）
  → ApplyCanvasClip(canvas)                            // 圆角裁剪（RoundRect Clip）（rs_image.cpp:250）
  → [MATRIX 模式] canvas.ConcatMatrix(fitMatrix_)     // 自定义矩阵变换（rs_image.cpp:253）
  → [rotateDegree != 0] canvas.Rotate + Translate      // 旋转和平移（rs_image.cpp:255）
  → DrawImageRepeatRect()                              // 重复绘制入口（rs_image.cpp:260）
```

**已绘制分支**（isDrawn_ && rect == lastRect_）：

```
CanvasDrawImage(canvas, rect, samplingOptions)
  → AutoCanvasRestore(needCanvasRestore)               //（rs_image.cpp:264）
  → [ASTC] RSPixelMapUtil::TransformDataSetForAstc()   // ASTC transform 处理（rs_image.cpp:266）
  → [MATRIX] canvas.ConcatMatrix(fitMatrix_)           //（rs_image.cpp:269）
  → [innerRect] DrawImageNine()                        // 九宫格绘制（rs_image.cpp:277）
  → [HDRConvert 成功] DrawRect(dst_)                   // HDR shader 绘制（rs_image.cpp:279）
  → [else] DrawImageRect()                             // 普通绘制（rs_image.cpp:281）
  → lastRect_ = rect                                   // 记录上一帧矩形（rs_image.cpp:285）
```

#### DrawImageRepeatRect 重复绘制逻辑

**进入条件**：

DrawImageRepeatRect 在 CanvasDrawImage 首次绘制分支中被调用（rs_image.cpp:260）：
- **首次绘制**：!isDrawn_（从未绘制过）
- **矩形变化**：rect != lastRect_（目标矩形发生变化）

说明：已绘制分支（isDrawn_ && rect == lastRect_）不会进入 DrawImageRepeatRect，直接使用缓存的绘制结果。

**流程与分支条件**：

```
DrawImageRepeatRect(samplingOptions, canvas)
  → CalcRepeatBounds(minX, maxX, minY, maxY)           // 计算 REPEAT_X/Y/REPEAT 边界（rs_image.cpp:612）
     → [NO_REPEAT] minX=maxX=minY=maxY=0               // 无 repeat，循环只执行一次
     → [REPEAT_X] 计算 minX/maxX（水平方向 repeat）
     → [REPEAT_Y] 计算 minY/maxY（垂直方向 repeat）
     → [REPEAT] 计算 minX/maxX/minY/maxY（双向 repeat）
  → ConvertPixelMapToDrawingImage + UploadGpu          // 确保 image_ 可用（rs_image.cpp:614）
  → HDRConvert()                                       // HDR 绘制分支（返回 true 则跳过后续）（rs_image.cpp:616）
  → src_ = Rect2DrawingRect(srcRect_)                  //（rs_image.cpp:617）
  → loopTime = (maxX - minX) * (maxY - minY)           // 计算循环次数（rs_image.cpp:618）
  → isNeedOffscreen = (maxX - minX >= 1) && loopTime > 5000  //（rs_image.cpp:619）
  → [!hdrImageDraw && REPEAT && isNeedOffscreen] DrawImageRepeatOffScreen()  // 离屏绘制优化（rs_image.cpp:620）
  → [正常绘制] for i in minX..maxX, j in minY..maxY:
       dst_ = Rect(left + i*width, top + j*height, ...)
       RsImageDraw(samplingOptions, canvas, hdrImageDraw)   // 每个 repeat tile 绘制（rs_image.cpp:633）
  → [NO_REPEAT] isDrawn_ = true                        // 标记已绘制（rs_image.cpp:637）
```

**关键分支说明**：

1. **CalcRepeatBounds 边界计算**（rs_image.cpp:698）：
   - 根据 imageRepeat_ 模式计算循环边界
   - NO_REPEAT：边界全为 0，for 循环只执行一次（i=0, j=0）
   - REPEAT_X：计算水平方向 repeat 次数（minX..maxX）
   - REPEAT_Y：计算垂直方向 repeat 次数（minY..maxY）
   - REPEAT：双向 repeat，计算水平和垂直次数
   - rotateDegree 为 90/-90 时：交换 right/bottom 和 repeat_x/repeat_y

2. **离屏绘制优化条件**（rs_image.cpp:619）：
   - **触发条件**：!hdrImageDraw && imageRepeat_ == REPEAT && isNeedOffscreen
   - **isNeedOffscreen**：(maxX - minX >= 1) && loopTime > REPEAT_LOOP_TIME_LIMIT (5000)
   - **优化原理**：DrawImageRepeatOffScreen 先绘制一行到离屏 surface，然后垂直复制，避免嵌套循环次数过多（rs_image.cpp:660）

3. **isDrawn_ 设置**（rs_image.cpp:637）：
   - **NO_REPEAT**：设置 isDrawn_ = true，后续可使用缓存结果
   - **REPEAT_X/REPEAT_Y/REPEAT**：不设置 isDrawn_，因为每帧 frameRect 可能变化，需要重新计算 repeat 边界

#### DrawImageRect / DrawImageOnCanvas 核心绘制

**DrawImageRect 流程**：

```
DrawImageRect(canvas, rect, samplingOptions)
  → AutoCanvasRestore(rotateDegree || isOrientationValid_)  //（rs_image.cpp:308）
  → [rotateDegree != 0] canvas.Rotate + Translate          //（rs_image.cpp:310）
  → [isOrientationValid_] ApplyImageOrientation()          // VERTICAL_FLIP / HORIZONTAL_FLIP（rs_image.cpp:316）
  → GenerateImageShaderForDrawRect()                       // perspective → shader 绘制（rs_image.cpp:318）
     → [返回非空] DrawImageShaderRectOnCanvas() → return    //（rs_image.cpp:320）
  → [fitMatrix skew/perspective] DrawImageWithFirMatrixRotateOnCanvas()  // 加模糊滤镜（rs_image.cpp:328）
  → [ScaleImageAsync] RSImageDetailEnhancerThread::EnhanceImageAsync()   // 异步增强（rs_image.cpp:336）
     → [返回非空] DrawImageRect(dstImage) → return          //（rs_image.cpp:389）
  → canvas.DrawImageRect(*image_, src_, dst_)              // 最终绘制（rs_image.cpp:340）
```

**DrawImageOnCanvas 流程**：

```
DrawImageOnCanvas(canvas, samplingOptions, hdrImageDraw)
  → [perspective] dst_.MakeOutset(1, 1)                    // 扩大 1px 抗锯齿（rs_image.cpp:758）
  → [innerRect] DrawImageNine()                            //（rs_image.cpp:763）
  → [hdrImageDraw] DrawRect(dst_)                          //（rs_image.cpp:765）
  → [else] GenerateImageShaderForDrawRect → DrawImageShaderRectOnCanvas  //（rs_image.cpp:767）
  → [else] DrawImageWithFirMatrixRotateOnCanvas            // fitMatrix 分支（rs_image.cpp:777）
  → [else] ScaleImageAsync → DrawImageRect                 // 异步增强（rs_image.cpp:782）
  → [else] canvas.DrawImageRect()                          // 普通绘制（rs_image.cpp:794）
```

#### 特殊图像处理

**HDR 绘制**（详细流程见 HDR 转换章节）：
- IsHDRUiCapture：判断 RGBA_F16 + DMA_ALLOC + BT2020 色彩空间（rs_image.cpp:209）
- IsEDRSurface：判断 EDR metadata + 非 HDR transfunc（rs_image.cpp:164）
- HDRConvert：创建 image shader + RSColorSpaceConvert 色彩空间转换（rs_image.cpp:95）

**YUV 图像**：

```
ProcessYUVImage(gpuContext, canvas)                        //（rs_image_base.cpp:580）
  → GetRenderDrawingImageCacheByPixelMapId(uniqueId_, threadId)
  → [缓存存在] image_ = cache; return
  → RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage()     // YUV → Drawing::Image
  → SKResourceManager::HoldResource(image_)
  → CacheRenderDrawingImageByPixelMapId()
```

**ASTC 压缩纹理**：

```
SetCompressedDataForASTC()                                 //（rs_image_base.cpp:623）
  → [Vulkan DMA] BuildFromOHNativeBuffer(nativeBuffer, capacity)
  → [else] BuildWithoutCopy(data + ASTC_HEADER_SIZE, capacity - 16)

UploadGpu(canvas)                                          //（rs_image_base.cpp:777）
  → GetRenderDrawingImageCacheByPixelMapId()
  → image->BuildFromCompressed(gpuContext, compressData, width, height, compressedType, colorSpace)
  → CacheRenderDrawingImageByPixelMapId()
```

**DMA PixelMap Vulkan 绑定**：

```
BindPixelMapToDrawingImage(canvas)                         //（rs_image_base.cpp:721）
  → GetRenderDrawingImageCacheByPixelMapId(uniqueId_, threadId)
  → [缓存存在] image_ = imageCache; return
  → MakeFromTextureForVK(canvas, surfaceBuffer)            // SurfaceBuffer → Vulkan texture
    → CreateNativeWindowBufferFromSurfaceBuffer()
    → MakeBackendTextureFromNativeBuffer()
    → image->BuildFromTexture(backendTexture, colorType, colorSpace, DeleteVkImage)
  → SKResourceManager::HoldResource(image_)
  → CacheRenderDrawingImageByPixelMapId()
```

**图片异构增强（ScaleImageAsync）**：

RSImageDetailEnhancerThread 用于异步图像细节增强（放大锐化），仅限 DDGR + Vulkan 环境。

**Enable 条件**（rs_image_detail_enhancer_thread.cpp:512）：
- GpuApiType == DDGR
- ScaleImageAsyncEnabled + MemoryWatermarkEnabled
- isParamValidate_（从 RsCommonHook 获取配置参数）
- IsPidEnabled（pid 在 RsCommonHook::GetImageEnhancePidList 中）

**TypeSupport 条件**（rs_image_detail_enhancer_thread.cpp:178）：
- pixelMap != nullptr && !IsEditable() && !IsHdr()
- GetAllocatorType() == DMA_ALLOC

**SizeSupport 条件**（rs_image_detail_enhancer_thread.cpp:118）：
- dstWidth > 0 && dstHeight > 0 && srcWidth != dstWidth && srcHeight != dstHeight
- 放大：srcWidth * srcHeight <= 1920*1080 (MAX_SCALEUP_SIZE)
- dstWidth/dstHeight/srcWidth/srcHeight 在 params_.minSize..maxSize 范围内
- scaleX/scaleY 在 params_.minScaleRatio..maxScaleRatio 范围内

**流程**：

```
ScaleImageAsync(pixelMap, dst, nodeId, imageId, image)      //（rs_image_detail_enhancer_thread.cpp:337）
  → [检查] IsTypeSupport(pixelMap) && IsSizeSupported(srcWidth, srcHeight, dstWidth, dstHeight)
  → ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId)  // 检查尺寸变化是否超过 200
  → [!GetProcessStatus(imageId)] SetProcessStatus(imageId, true)
  → PostTask(ExecuteTaskAsync)                              // 异步投递到 RSImageDetailEnhancerThread 线程

ExecuteTaskAsync(dst, image, nodeId, imageId, pixelMap)     //（rs_image_detail_enhancer_thread.cpp:267）
  → CreateSurfaceBuffer(pixelMap, dstWidth, dstHeight)      // 创建目标 SurfaceBuffer（DMA 分配）
  → SetMemoryName(dstSurfaceBuffer, "asynscaling_hpae_memory")  // 设置内存名称
  → [scaleRatio > slrParams_.rangeParams.back().rangeMax] ScaleByAAE()  // AAE 算法
  → [else] ScaleByHDSampler()                               // HDSample 算法（SLR/ESR）
  → SetScaledImage(imageId, dstImage)                       // 缓存增强后的图像
  → MarkScaledImageDirty(nodeId)                            // 触发节点重绘（调用 callback_(nodeId)）

ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, image)  //（rs_image_detail_enhancer_thread.cpp:215）
  → InitSurface(dstWidth, dstHeight, dstSurfaceBuffer, image)  // 创建 Vulkan surface
  → [SLR 算法] scaleRatio 在 slrParams_.rangeParams 范围内，dstImageSize 在 slrParams_.minSize..maxSize
     → HDSampleType::SLR，GetSharpness(slrParams_, scaleRatio)
  → [ESR 算法] scaleRatio 在 esrParams_.rangeParams 范围内，dstImageSize 在 esrParams_.minSize..maxSize
     → HDSampleType::ESR，GetSharpness(esrParams_, scaleRatio)
  → CreateHDSampleImageFilter(srcImage, src, dst, info)     // 创建 HDSample filter
  → canvas.DrawImageRect(srcImage, src, dst)                // 绘制到 surface
  → GetImageSnapshot()                                      // 获取增强后的图像

ScaleByAAE(dstSurfaceBuffer, image)                         //（rs_image_detail_enhancer_thread.cpp:188）
  → MakeImageFromSurfaceBuffer(dstSurfaceBuffer, image)     // 创建目标 Drawing::Image
  → Drawing::Image::ScaleImage(srcImage, dstImage, OPTION_AAE_ARSR)  // AAE/ARSR 算法放大
```

**缓存策略**（rs_image_detail_enhancer_thread.cpp:409）：
- LRU 缓存（imageList_ + keyMap_），MAX_IMAGE_CACHE = 150.0f MB
- 按访问顺序排列（splice 到 front），curCache_ >= MAX_IMAGE_CACHE 时从 back 移除
- processStatusMap_：标记是否正在处理中
- processReadyMap_：标记是否处理完成
- ResetStatus：检查 dstImage 尺寸变化是否超过 IMAGE_DIFF_VALUE (200)，变化过大则重置状态

**DrawImageRect / DrawImageOnCanvas 中的触发**：

```
DrawImageRect / DrawImageOnCanvas
  → [条件] RS_ENABLE_IMAGE_DETAIL_ENHANCER && ROSEN_OHOS && RS_ENABLE_VK
  → RSImageParams = {pixelMap_, nodeId_, dst_, uniqueId_, image_, matrixScaleX}
  → EnhanceImageAsync(isScaledImageAsync, RSImageParams)   //（rs_image.cpp:336 / rs_image.cpp:782）
     → GetEnabled() && IsPidEnabled(nodeId_) && TypeSupport && SizeSupport
     → ScaleImageAsync(pixelMap, dst, nodeId, imageId, image)
     → dstImage = GetScaledImage(imageId)
     → [dstImage 存在] 检查 scaleRatio 是否比原图更接近 1.0（更优则使用）
     → [返回 dstImage] DrawImageRect(dstImage, newSrcRect, dst_) → return
  → [未触发或返回 nullptr] canvas.DrawImageRect(*image_, src_, dst_)  // 普通绘制
```

**关键说明**：
- 图片异构增强只在 **首次绘制时触发异步任务**，后续帧从缓存获取增强后的图像
- 异步完成后调用 `MarkScaledImageDirty(nodeId)` 触发节点重绘，下一帧使用增强后的图像
- 增强 algorithm 由配置文件控制（RsCommonHook 获取 SLR/ESR/AAE 参数）
- 线程优先级为 QOS_USER_INTERACTIVE，EventRunner 名为 "RSImageDetailEnhancerThread"

### HDR 转换

#### IsHDRUiCapture 判断逻辑（rs_image.cpp:209）

判断条件：
- PixelMap 格式为 RGBA_F16
- AllocatorType 为 DMA_ALLOC
- 色彩空间为 BT2020 / BT2020_HLG / BT2020_PQ / BT2020_HLG_LIMIT / BT2020_PQ_LIMIT

#### IsEDRSurface 判断逻辑（rs_image.cpp:164）

判断条件：
- dynamicRangeMode_ != STANDARD
- AllocatorType 为 DMA_ALLOC
- 存在 HdrStaticMetadata 且 maxContentLightLevel > 203 nits（DEFAULT_SDR_NITS）
- HDR metadata type 为 CM_METADATA_NONE 或 CM_COMPONENT_EDR
- transfunc 非 PQ/HLG（即不是标准 HDR）

#### HDRConvert 流程（rs_image.cpp:95）

```
HDRConvert(sampling, canvas)
  → [前置判断] pixelMap_->IsHdr() || IsHDRUiCapture() || IsEDRSurface()
  → [Canvas 类型检查] GetDrawingType() == PAINT_FILTER
  → [SurfaceBuffer 检查] surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap_->GetFd())
  → CreateImageShader(image_, matrix)                      // 创建 image shader
  → SetEDRSurface(isEDRSurface)                            // 标记 EDR surface
  → [EDR] MetadataHelper::SetHDRMetadataType(sfBuffer, CM_COMPONENT_EDR)
  → GetColorGamutFromCanvas(canvas)                        // 获取目标色彩空间
  → RSColorSpaceConvert::ColorSpaceConvertor(imageShader, sfBuffer, paint_, targetColorSpace, screenId, dynamicRangeMode, hdrProperties)
  → canvas.AttachPaint(paint_)                             // 附加带 shader 的 paint
  → paint_.SetShaderEffect(nullptr)                        // 避免 cross-thread destruction
  → return true                                            // 后续调用 DrawRect()
```

#### HDR 绘制模式

- **HDR TMO**：hdrOn + GetHdrImageEnabled + 非 multi-screen + 非 SDR capture → 使用 dynamicRangeMode_ 进行 tone mapping
- **SDR Capture**：ScreenshotType 为 SDR_SCREENSHOT / SDR_WINDOWSHOT / SDR_UICAPTURE → 强制 STANDARD 模式

### RSImageCache 缓存机制

RSImageCache 是单例（`static RSImageCache gRSImageCacheInstance`，rs_image_base.cpp:26），管理跨 RSImage 实例的图片资源复用：

**说明**：RSImageCache 仅在 **单进程内使用**，不支持跨进程共享。同一进程内的多个线程通过 mutex 机制同步访问缓存表。

1. **Drawing::Image 缓存**（`drawingImageCache_`）：
   - Key：uniqueId（通常为 PixelMap 的 uniqueId）。
   - Value：`pair<shared_ptr<Drawing::Image>, uint64_t>`（图片 + 引用计数）。
   - Unmarshalling 时引用计数 +1，RSImage 析构时 -1，归零时移除。
   - **线程同步**：由 `mutex_` 保护（rs_image_cache.h:84）。

2. **PixelMap 缓存**（`pixelMapCache_`）：
   - 结构同上，缓存 PixelMap 对象避免重复解码。
   - **线程同步**：由 `mutex_` 保护（rs_image_cache.h:84）。

3. **按 PixelMapId 关联的 Drawing::Image 缓存**（`pixelMapIdRelatedDrawingImageCache_`）：
   - 同一 PixelMap 在不同线程可能生成不同 Drawing::Image（GPU 上下文不同）。
   - 按 `(pixelMapId, tid)` 二级索引。
   - **线程同步**：由 `mapMutex_` 保护（rs_image_cache.h:90）。
   - **DDGR 特例**：GpuApiType::DDGR 时不需要 tid 参数，因为全局共享 GPU context（rs_image_base.cpp:542）。

4. **可编辑 PixelMap 缓存**（`editablePixelMapCache_`）：
   - 独立的缓存表，用于可编辑图片。
   - `DiscardEditablePixelMapCache()` 丢弃但不释放，`ReleaseEditablePixelMapCache()` 释放。
   - **线程同步**：由 `editablePixelMapCacheMutex_` 保护（rs_image_cache.h:93）。

5. **延迟释放队列**（`editablePixelMapCacheToRelease_`）：
   - 存放待释放的可编辑 PixelMap，避免在渲染帧中释放导致 GPU 纹理被销毁。
   - **线程同步**：由 `editablePixelMapCacheToReleaseMutex_` 保护（rs_image_cache.h:95）。

6. **UniqueId 收集列表**（`uniqueIdList_`）：
   - 收集待释放的 uniqueId，批量释放时遍历调用 ReleasePixelMapCache。
   - **线程同步**：由 `uniqueIdListMutex_` 保护（rs_image_cache.h:97）。

**多线程同步机制总结**：

| 缓存表 | 保护 mutex | 典型操作 |
| --- | --- | --- |
| `drawingImageCache_` / `pixelMapCache_` | `mutex_` | CacheDrawingImage/GetDrawingImageCache/ReleaseDrawingImageCache/ReleasePixelMapCache |
| `pixelMapIdRelatedDrawingImageCache_` | `mapMutex_` | CacheRenderDrawingImageByPixelMapId/GetRenderDrawingImageCacheByPixelMapId/ReleaseDrawingImageCacheByPixelMapId |
| `editablePixelMapCache_` | `editablePixelMapCacheMutex_` | CacheEditablePixelMap/DiscardEditablePixelMapCache/DecreaseRefCountAndReleaseEditablePixelMapCache |
| `editablePixelMapCacheToRelease_` | `editablePixelMapCacheToReleaseMutex_` | DiscardEditablePixelMapCache/ReleaseEditablePixelMapCache |
| `uniqueIdList_` | `uniqueIdListMutex_` | CollectUniqueId/ReleaseUniqueIdList/CheckUniqueIdIsEmpty |

**跨进程图片传递说明**：

RSImageCache 不涉及跨进程共享：
- **图片传递路径**：应用侧 PixelMap 通过 IPC（Parcel Marshalling/Unmarshalling）传递到 Render Service。
- **缓存隔离**：每个进程有独立的 RSImageCache 实例，缓存各自进程内的图片资源。
- **数据拷贝**：Parcel 传递 PixelMap 数据时涉及拷贝，目标进程重新创建 PixelMap 对象并缓存到本地 RSImageCache。
- **引用计数独立**：各进程的引用计数独立管理，互不影响。

### 引用计数与生命周期

- RSImage Unmarshalling 时从 RSImageCache 获取或创建 Drawing::Image/PixelMap，引用计数 +1。
- RSImage 析构时调用 `ReleaseDrawingImageCache()` / `ReleasePixelMapCache()`，引用计数 -1。
- 引用计数归零时从缓存表移除，资源可被回收。
- `CheckRefCntAndReleaseImageCache()` 检查并释放多余引用。

### Drawing::Image 释放

- RSImage 关联 `NodeId`，通过 `SetNodeId()` 记录。
- Vulkan 模式下 `RemoveImageMemForWindow()` 在窗口销毁时释放关联的 Drawing::Image。
- `ReserveImageInfo()` 保留图片信息用于后续清理。

## PixelMap / Image / SurfaceBuffer 生命周期

### PixelMap 到 RSImage

PixelMap 常见进入 RS 的路径有两类：

1. 应用侧设置图片属性，随 `RSImage::Marshalling()` 进入 transaction。
2. Render Service 通过 `CreatePixelMapFromSurface()` 从 Surface 或 SurfaceBuffer 回读。

读取链路重点看：

- `RSImage::Unmarshalling()`：读取 uniqueId、尺寸、nodeId 和 PixelMap 数据。
- `RSImageCache::CachePixelMap()` / `GetPixelMapCache()`：按 uniqueId 缓存 PixelMap。
- `RSImageCache::IncreasePixelMapCacheRefCount()`：反序列化复用时增加引用。
- `RSImageCache::ReleasePixelMapCache()`：RSImage 生命周期结束后减少引用。
- `RSImageCache::CheckRefCntAndReleaseImageCache()`：处理缓存引用和输入 PixelMap 的一致性。

修改 PixelMap 传输时，必须确认 uniqueId 不是安全边界。
它只能作为缓存键，不能证明图片内容可信、尺寸合法或调用方有权限。

### Drawing::Image 缓存

`Drawing::Image` 可能来自 PixelMap，也可能来自 GPU texture。缓存有两层语义：

- `drawingImageCache_`：按 uniqueId 共享 `Drawing::Image` 和引用计数。
- `pixelMapIdRelatedDrawingImageCache_`：按 `(pixelMapId, tid)` 缓存渲染线程上的 `Drawing::Image`。

后者存在是因为不同线程或 GPU context 下的 texture 资源不一定可共享。
修改渲染线程、并行渲染、UIFirst 或 Vulkan/GLES 分支时，
要确认缓存键包含正确线程或 context 维度。

### SurfaceBuffer 绘制

SurfaceBuffer 不走普通 PixelMap 引用计数模型，重点是 buffer、fence、回调和释放顺序：

```text
ExtendRecordingCanvas::DrawSurfaceBuffer()
  -> DrawSurfaceBufferOpItem::Marshalling()
  -> CmdListHelper::AddSurfaceBufferEntryToCmdList()
  -> DrawSurfaceBufferOpItem::Playback()
  -> DrawWithVulkan() / DrawWithGles()
  -> OnAfterDraw() / ReleaseBuffer()
```

关键点：

- `Drawing::SurfaceBufferEntry` 同时携带 `SurfaceBuffer` 和 acquire fence。
- `WaitFence()` 只允许有界等待，不能因异常 fence 阻塞渲染线程。
- `DrawSurfaceBufferOpItem::OnAfterDraw()` 会创建或传递 release fence。
- `DrawSurfaceBufferOpItem::ReleaseBuffer()` 触发 finish callback，不能提前释放仍被 GPU 使用的 buffer。
- `DrawHybridPixelMapOpItem` 也会使用 fence，需要覆盖 PixelMap 和 SurfaceBuffer 混合输入。

### 从 Surface 创建 PixelMap

`CreatePixelMapFromSurface()` 跨越 client、service 和 render process：

```text
RSRenderServiceClient::CreatePixelMapFromSurfaceId()
  -> RSClientToServiceConnection::CreatePixelMapFromSurface()
  -> RSServiceToRenderConnection::CreatePixelMapFromSurface()
  -> RSRenderPipelineAgent::CreatePixelMapFromSurface()
  -> Rosen::CreatePixelMapFromSurface()
```

检查点：

- `Surface` 为空、producer 为空、srcRect 非法、transformEnabled 组合都要有明确返回。
- Proxy 写入和 Stub 读取 `srcRect` 字段顺序必须一致。
- 返回的 PixelMap 为空时不要继续访问尺寸、像素地址或 HDR 元数据。
- 真实设备上需要覆盖屏幕旋转、虚拟屏、HDR surface 和 protected layer 场景。

## 缓存修改检查清单

- 新增缓存键时说明维度：uniqueId、nodeId、pid/tid、GPU context、ColorSpace、动态范围。
- 引用计数增加和释放必须成对，失败路径不得泄漏 cache entry。
- 释放 GPU 资源时确认是否在正确线程和正确 graphics context 下执行。
- 可编辑 PixelMap 和不可编辑 PixelMap 不得共用同一缓存表。
- SurfaceBuffer 绘制路径必须覆盖 acquire fence 超时、release fence 创建失败和回调为空。
- HDR/EDR、PixelFormat、ColorSpace、row bytes、stride 不能只按宽高推断。
- 设备截图或 capture 相关改动要补真实设备验证，不能只依赖单元测试。
- 没有真实设备时，记录设备验证缺口，不默认阻塞文档、静态验证或可本地验证的小修。
  但不能把截图、HDR、SurfaceBuffer 或 GPU 资源行为描述为完整设备验证。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 引用计数缓存 | `drawingImageCache_` value 中的 uint64_t refCount | 多个 RSImage 可共享同一 Drawing::Image，避免重复上传 GPU 纹理 |
| 线程级 Drawing::Image 缓存 | `pixelMapIdRelatedDrawingImageCache_` 按 pid 索引 | 不同 GPU 上下文/线程的 Drawing::Image 不可共享，需按线程隔离 |
| 可编辑/不可编辑分离 | `editablePixelMapCache_` vs `pixelMapCache_` | 可编辑 PixelMap 可能被修改，需独立管理避免污染共享缓存 |
| ImageFit 矩阵 | `fitMatrix_` + `ApplyImageFit()` | 统一适配算法，支持 MATRIX 模式自定义变换 |
| NodeId 关联 | `SetNodeId()` | 窗口/节点销毁时批量释放关联图片资源 |
| 延迟释放 | `editablePixelMapCacheToRelease_` | 避免在渲染帧中释放 PixelMap 导致 GPU 使用中的纹理被销毁 |

## 待补充背景

- RSImage 压缩数据的具体格式和解码流程。
- HDR 转换的完整色彩空间映射策略。
