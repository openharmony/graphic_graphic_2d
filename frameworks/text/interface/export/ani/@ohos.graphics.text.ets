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

/**
 * @file The ets file is used to compile and generate abc files,
 *    which have object relationships with the sdk js repository @ohs.graphics.text.d.ts file
 * @kit ArkGraphics2D
 */
import { BusinessError } from '@ohos.base';
import type drawing from '@ohos.graphics.drawing';
import type common2D from '@ohos.graphics.common2D';
import { Resource } from 'global.resource';
import hilog from '@ohos.hilog';
const TEXT_LOG_DOMAIN = 0xD001408;
const TEXT_LOG_TAG = "Text";

namespace text {
  loadLibrary('text_engine_ani.z');
  const ERROR_INVALID_PARAM = 401;

  class Cleaner {
    static { loadLibraryWithPermissionCheck('text_engine_ani.z', "@ohos.graphics.text") }
    private ptr: long = 0;
    private className: string;
    constructor(className: string, ptr: long) {
      this.ptr = ptr;
      this.className = className;
    }
    native clean(): void;
  }

  let destroyRegister = new FinalizationRegistry<Cleaner>((cleaner: Cleaner)=>{
    cleaner.clean();
  });
  let unregisterToken = new object();

  export class FontCollection {
    static { loadLibraryWithPermissionCheck('text_engine_ani.z', "@ohos.graphics.text") }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.constructorNative();
    };
    native constructorNative(): void;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("FontCollection", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    native static getGlobalInstance(): FontCollection;
    native loadFontSync(name: string, path: Resource | string): void;
    async loadFont(name: string, path: string | Resource): Promise<void> {
      await new Promise<void>((resolve, reject) => {
        try {
          taskpool.execute((): void => {
            this.loadFontSync(name, path);
          }).then(() => {
            resolve(undefined);
          })
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }
    native unloadFontSync(name: string): void;
    async unloadFont(name: string): Promise<void> {
      await new Promise<void>((resolve, reject) => {
        try {
          taskpool.execute((): void => {
            this.unloadFontSync(name);
          }).then(() => {
            resolve(undefined);
          })
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }
    native clearCaches(): void;
  }

  export class ParagraphBuilder {
    static { loadLibraryWithPermissionCheck('text_engine_ani.z', "@ohos.graphics.text") }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;
    constructor(paragraphStyle: ParagraphStyle, fontCollection: FontCollection) {
      this.constructorNative(paragraphStyle, fontCollection);
    };
    native constructorNative(paragraphStyle: ParagraphStyle, fontCollection: FontCollection): void;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("ParagraphBuilder", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    native pushStyle(textStyle: TextStyle): void;
    native popStyle(): void;
    native addText(text: string): void;
    native addPlaceholder(placeholderSpan: PlaceholderSpan): void;
    native build(): Paragraph;
    native buildLineTypeset(): LineTypeset;
    native addSymbol(symbolId: int): void;
  }

  export class Paragraph {
    static { loadLibraryWithPermissionCheck('text_engine_ani.z', "@ohos.graphics.text") }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("Paragraph", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    native layoutSync(width: double): void;
    async layout(width: double): Promise<void> {
      await new Promise<void>((resolve, reject) => {
        try {
          taskpool.execute((): void => {
            this.layoutSync(width);
          }).then(() => {
            resolve(undefined);
          })
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }
    native paint(canvas: drawing.Canvas, x: double, y: double): void;
    native paintOnPath(canvas: drawing.Canvas, path: drawing.Path, hOffset: double, vOffset: number): void;
    native getMaxWidth(): double;
    native getHeight(): double;
    native getLongestLine(): double;
    native getLongestLineWithIndent(): double;
    native getMinIntrinsicWidth(): double;
    native getMaxIntrinsicWidth(): double;
    native getAlphabeticBaseline(): double;
    native getIdeographicBaseline(): double;
    native getRectsForRange(range: Range, widthStyle: RectWidthStyle, heightStyle: RectHeightStyle): Array<TextBox>;
    native getRectsForPlaceholders(): Array<TextBox>;
    native getGlyphPositionAtCoordinate(x: double, y: double): PositionWithAffinity;
    native getWordBoundary(offset: int): Range;
    native getLineCount(): int;
    native getLineHeight(line: int): double;
    native getLineWidth(line: int): double;
    native didExceedMaxLines(): boolean;
    native getTextLines(): Array<TextLine>;
    native getActualTextRange(lineNumber: int, includeSpaces: boolean): Range;
    native getLineMetrics(): Array<LineMetrics>;
    getLineMetrics(lineNumber: int): LineMetrics | undefined {
        return this.nativeGetLineMetricsAt(lineNumber);
    };

    private native nativeGetLineMetricsAt(lineNumber: int): LineMetrics | undefined;
    native updateColor(color: common2D.Color): void;
    native updateDecoration(decoration: Decoration): void;
  }

  enum FontWidth {
    ULTRA_CONDENSED = 1,
    EXTRA_CONDENSED = 2,
    CONDENSED = 3,
    SEMI_CONDENSED = 4,
    NORMAL = 5,
    SEMI_EXPANDED = 6,
    EXPANDED = 7,
    EXTRA_EXPANDED = 8,
    ULTRA_EXPANDED = 9,
  }

  enum TextHeightBehavior {
    ALL = 0x0,
    DISABLE_FIRST_ASCENT = 0x1,
    DISABLE_LAST_ASCENT = 0x2,
    DISABLE_ALL = 0x1 | 0x2,
  }

  enum TextAlign {
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2,
    JUSTIFY = 3,
    START = 4,
    END = 5,
  }

  enum TextVerticalAlign {
    BASELINE = 0,
    BOTTOM = 1,
    CENTER = 2,
    TOP = 3
  }

  enum TextDirection {
    RTL,
    LTR,
  }

  enum BreakStrategy {
    GREEDY,
    HIGH_QUALITY,
    BALANCED,
  }

  enum WordBreak {
    NORMAL,
    BREAK_ALL,
    BREAK_WORD,
    BREAK_HYPHEN,
  }

  interface StrutStyle {
    fontFamilies?: Array<string>;
    fontStyle?: FontStyle;
    fontWidth?: FontWidth;
    fontWeight?: FontWeight;
    fontSize?: double;
    height?: double;
    leading?: double;
    forceHeight?: boolean;
    enabled?: boolean;
    heightOverride?: boolean;
    halfLeading?: boolean;
  }

  interface TextTab {
    alignment: TextAlign;
    location: double;
  }

  export interface ParagraphStyle {
    textStyle?: TextStyle;
    textDirection?: TextDirection;
    align?: TextAlign;
    wordBreak?: WordBreak;
    maxLines?: int;
    breakStrategy?: BreakStrategy;
    strutStyle?: StrutStyle;
    textHeightBehavior?: TextHeightBehavior;
    tab?: TextTab;
    trailingSpaceOptimized?: boolean;
    autoSpace?: boolean;
    verticalAlign?: TextVerticalAlign;
  }

  interface TextShadow {
    color?: common2D.Color;
    point?: common2D.Point;
    blurRadius?: double;
  }

  class TextShadowInternal implements TextShadow {
    color?: common2D.Color;
    point?: common2D.Point;
    blurRadius?: double;
    constructor(
      color: common2D.Color,
      point: common2D.Point,
      blurRadius: double
    ) {
      this.color = color;
      this.point = point;
      this.blurRadius = blurRadius;
    }
  }

  export interface TextStyle {
    decoration?: Decoration;
    color?: common2D.Color;
    fontWeight?: FontWeight;
    fontStyle?: FontStyle;
    baseline?: TextBaseline;
    fontFamilies?: Array<string>;
    fontSize?: double;
    letterSpacing?: double;
    wordSpacing?: double;
    heightScale?: double;
    halfLeading?: boolean;
    heightOnly?: boolean;
    ellipsis?: string;
    ellipsisMode?: EllipsisMode;
    locale?: string;
    baselineShift?: double;
    fontFeatures?: Array<FontFeature>;
    textShadows?: Array<TextShadow>;
    backgroundRect?: RectStyle;
    fontVariations?: Array<FontVariation>;
    badgeType?: TextBadgeType;
  }

  class TextStyleInternal implements TextStyle {
    decoration?: Decoration;
    color?: common2D.Color;
    fontWeight?: FontWeight;
    fontStyle?: FontStyle;
    baseline?: TextBaseline;
    fontFamilies?: Array<string>;
    fontSize?: double;
    letterSpacing?: double;
    wordSpacing?: double;
    heightScale?: double;
    halfLeading?: boolean;
    heightOnly?: boolean;
    ellipsis?: string;
    ellipsisMode?: EllipsisMode;
    locale?: string;
    baselineShift?: double;
    fontFeatures?: Array<FontFeature>;
    textShadows?: Array<TextShadow>;
    backgroundRect?: RectStyle;
    fontVariations?: Array<FontVariation>;
    constructor(
      decoration: Decoration,
      color: common2D.Color,
      fontWeight: FontWeight,
      fontStyle: FontStyle,
      baseline: TextBaseline,
      fontFamilies: Array<string>,
      fontSize: double,
      letterSpacing: double,
      wordSpacing: double,
      heightScale: double,
      halfLeading: boolean,
      heightOnly: boolean,
      ellipsis: string,
      ellipsisMode: EllipsisMode,
      locale: string,
      baselineShift: double,
      fontFeatures: Array<FontFeature>,
      textShadows: Array<TextShadow>,
      backgroundRect: RectStyle
    ) {
      this.decoration = decoration;
      this.color = color;
      this.fontWeight = fontWeight;
      this.fontStyle = fontStyle;
      this.baseline = baseline;
      this.fontFamilies = fontFamilies;
      this.fontSize = fontSize;
      this.letterSpacing = letterSpacing;
      this.wordSpacing = wordSpacing;
      this.heightScale = heightScale;
      this.halfLeading = halfLeading;
      this.heightOnly = heightOnly;
      this.ellipsis = ellipsis;
      this.ellipsisMode = ellipsisMode;
      this.locale = locale;
      this.baselineShift = baselineShift;
      this.fontFeatures = fontFeatures;
      this.textShadows = textShadows;
      this.backgroundRect = backgroundRect;
    }
  }

  interface LineMetrics {
    startIndex: int;
    endIndex: int;
    ascent: double;
    descent: double;
    height: double;
    width: double;
    left: double;
    baseline: double;
    lineNumber: int;
    topHeight: double;
    runMetrics: Map<int, RunMetrics>;
  }

  class LineMetricsInternal implements LineMetrics {
    startIndex: int;
    endIndex: int;
    ascent: double;
    descent: double;
    height: double;
    width: double;
    left: double;
    baseline: double;
    lineNumber: int;
    topHeight: double;
    runMetrics: Map<int, RunMetrics>;
    constructor(
      startIndex: int,
      endIndex: int,
      ascent: double,
      descent: double,
      height: double,
      width: double,
      left: double,
      baseline: double,
      lineNumber: int,
      topHeight: double,
      runMetrics: Map<int, RunMetrics>
    ) {
      this.startIndex = startIndex;
      this.endIndex = endIndex;
      this.ascent = ascent;
      this.descent = descent;
      this.height = height;
      this.width = width;
      this.left = left;
      this.baseline = baseline;
      this.lineNumber = lineNumber;
      this.topHeight = topHeight;
      this.runMetrics = runMetrics;
    }
  }

  export enum FontWeight {
    W100,
    W200,
    W300,
    W400,
    W500,
    W600,
    W700,
    W800,
    W900,
  }

  export enum FontStyle {
    NORMAL,
    ITALIC,
    OBLIQUE,
  }

  export enum TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
  }

  export enum EllipsisMode {
    START,
    MIDDLE,
    END,
  }

  export interface Decoration {
    textDecoration?: TextDecorationType;
    color?: common2D.Color;
    decorationStyle?: TextDecorationStyle;
    decorationThicknessScale?: double;
  }

  class DecorationInternal implements Decoration {
    textDecoration?: TextDecorationType;
    color?: common2D.Color;
    decorationStyle?: TextDecorationStyle;
    decorationThicknessScale?: double;
    constructor(
      textDecoration: TextDecorationType,
      color: common2D.Color,
      decorationStyle: TextDecorationStyle,
      decorationThicknessScale: double
    ) {
      this.textDecoration = textDecoration;
      this.color = color;
      this.decorationStyle = decorationStyle;
      this.decorationThicknessScale = decorationThicknessScale;
    }
  }

  export interface FontFeature {
    name: string;
    value: int;
  }
  
  class FontFeatureInternal implements FontFeature {
    name: string;
    value: int;
    constructor(
      name: string,
      value: int
    ) {
      this.name = name;
      this.value = value;
    }
  }

  export enum TextDecorationType {
    NONE = 0x0,
    UNDERLINE = 0x1,
    OVERLINE = 0x2,
    LINE_THROUGH = 0x4,
  }

  export enum TextDecorationStyle {
    SOLID,
    DOUBLE,
    DOTTED,
    DASHED,
    WAVY,
  }

  export interface RunMetrics {
    textStyle: TextStyle;
    fontMetrics: drawing.FontMetrics;
  }

  class RunMetricsInternal implements RunMetrics {
    textStyle: TextStyle;
    fontMetrics: drawing.FontMetrics;
    constructor(
      textStyle: TextStyle,
      fontMetrics: drawing.FontMetrics
    ) {
      this.textStyle = textStyle;
      this.fontMetrics = fontMetrics;
    }
  }

  export interface RectStyle {
    color: common2D.Color;
    leftTopRadius: double;
    rightTopRadius: double;
    rightBottomRadius: double;
    leftBottomRadius: double;
  }

  class RectStyleInternal implements RectStyle {
    color: common2D.Color;
    leftTopRadius: double;
    rightTopRadius: double;
    rightBottomRadius: double;
    leftBottomRadius: double;
    constructor(
      color: common2D.Color,
      leftTopRadius: double,
      rightTopRadius: double,
      rightBottomRadius: double,
      leftBottomRadius: double,
    ) {
      this.color = color;
      this.leftTopRadius = leftTopRadius;
      this.rightTopRadius = rightTopRadius;
      this.rightBottomRadius = rightBottomRadius;
      this.leftBottomRadius = leftBottomRadius;
    }
  }

  export interface FontVariation {
    axis: string;
    value: double;
  }

  class FontVariationInternal implements FontVariation {
    axis: string;
    value: double;
  }

  export interface TextBox {
    rect: common2D.Rect;
    direction: TextDirection;
  }

  class TextBoxInternal implements TextBox {
    rect: common2D.Rect;
    direction: TextDirection;
    constructor(
      rect: common2D.Rect,
      direction: TextDirection
    ) {
      this.rect = rect;
      this.direction = direction;
    }
  }

  export interface Range {
    start: int;
    end: int;
  }

  class RangeInternal implements Range {
    start: int;
    end: int;
    constructor(
      start: int,
      end: int
    ) {
      this.start = start;
      this.end = end;
    }
  }

  export enum RectWidthStyle {
    TIGHT,
    MAX,
  }

  export enum RectHeightStyle {
    TIGHT,
    MAX,
    INCLUDE_LINE_SPACE_MIDDLE,
    INCLUDE_LINE_SPACE_TOP,
    INCLUDE_LINE_SPACE_BOTTOM,
    STRUT,
  }

  export interface PositionWithAffinity {
    position: int;
    affinity: Affinity;
  }

  class PositionWithAffinityInternal implements PositionWithAffinity {
    position: int;
    affinity: Affinity;
    constructor(
      position: int,
      affinity: Affinity
    ) {
      this.position = position;
      this.affinity = affinity;
    }
  }

  export enum Affinity {
    UPSTREAM,
    DOWNSTREAM,
  }

  type CaretOffsetsCallback = (offset: int, index: int, leadingEdge: boolean) => boolean;

  export class TextLine {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("TextLine", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    native getGlyphCount(): int;
    native getTextRange(): Range;
    native getGlyphRuns(): Array<Run>;
    native paint(canvas: drawing.Canvas, x: double, y: double): void;
    native createTruncatedLine(width: double, ellipsisMode: EllipsisMode, ellipsis: string): TextLine | undefined;
    native getTypographicBounds(): TypographicBounds;
    native getImageBounds(): common2D.Rect;
    native getTrailingSpaceWidth(): double;
    native getStringIndexForPosition(point: common2D.Point): int;
    native getOffsetForStringIndex(index: int): double;
    native enumerateCaretOffsets(callback: CaretOffsetsCallback): void;
    native getAlignmentOffset(alignmentFactor: double, alignmentWidth: double): double;
  }

  export class Run {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("Run", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    native getGlyphCount(): int;
    native getGlyphs(): Array<int>;
    getGlyphs(range: Range): Array<int> | undefined {
      return this.nativeGetGlyphs(range);
    }
    private native nativeGetGlyphs(range: Range): Array<int>;
    native getPositions(): Array<common2D.Point>;
    getPositions(range: Range): Array<common2D.Point> | undefined {
      return this.nativeGetPositions(range);
    }
    private native nativeGetPositions(range: Range): Array<common2D.Point>;
    native getOffsets(): Array<common2D.Point>;
    native getFont(): drawing.Font;
    native paint(canvas: drawing.Canvas, x: double, y: double): void;
    native getStringIndices(range?: Range): Array<int> | undefined;
    native getStringRange(): Range;
    native getTypographicBounds(): TypographicBounds;
    native getImageBounds(): common2D.Rect;
    native getTextDirection(): TextDirection;
    native getAdvances(range: Range): Array<common2D.Point>;
  }

  export interface PlaceholderSpan {
    width: double;
    height: double;
    align: PlaceholderAlignment;
    baseline: TextBaseline;
    baselineOffset: double;
  }

  export enum PlaceholderAlignment {
    OFFSET_AT_BASELINE,
    ABOVE_BASELINE,
    BELOW_BASELINE,
    TOP_OF_ROW_BOX,
    BOTTOM_OF_ROW_BOX,
    CENTER_OF_ROW_BOX,
    FOLLOW_PARAGRAPH,
  }

  export class LineTypeset {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;
    bindNative(ptr: long): void {
      this.nativeObj = ptr;
      this.cleaner = new Cleaner("LineTypeset", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    native getLineBreak(startIndex: int, width: double): int;
    native createLine(startIndex: int, count: int): TextLine;
  }

  export interface TypographicBounds {
    ascent: double;
    descent: double;
    leading: double;
    width: double;
  }

  class TypographicBoundsInternal implements TypographicBounds {
    ascent: double;
    descent: double;
    leading: double;
    width: double;
    constructor(
      ascent: double,
      descent: double,
      leading: double,
      width: double,
    ) {
      this.ascent = ascent;
      this.descent = descent;
      this.leading = leading;
      this.width = width;
    }
  }

  native function getSystemFontFullNamesByTypeSync(fontType: SystemFontType): Array<string>;

  export async function getSystemFontFullNamesByType(fontType: SystemFontType): Promise<Array<string>> {
    return await new Promise<Array<string>>((resolve, reject) => {
      try {
        taskpool.execute((): Array<string> => {
          return getSystemFontFullNamesByTypeSync(fontType);
        }).then((result) => {
          resolve(result);
        })
      } catch (err) {
        reject(err as BusinessError);
      }
    });
  }

  native function getFontDescriptorByFullNameSync(fullName: string, fontType: SystemFontType): FontDescriptor;

  export async function getFontDescriptorByFullName(fullName: string, fontType: SystemFontType): Promise<FontDescriptor> {
    return await new Promise<FontDescriptor>((resolve, reject) => {
      try {
        taskpool.execute((): FontDescriptor => {
          return getFontDescriptorByFullNameSync(fullName, fontType);
        }).then((result) => {
          resolve(result);
        })
      } catch (err) {
        reject(err as BusinessError);
      }
    });
  }

  native function matchFontDescriptorsSync(desc: FontDescriptor): Array<FontDescriptor>;

  export async function matchFontDescriptors(desc: FontDescriptor): Promise<Array<FontDescriptor>> {
    return await new Promise<Array<FontDescriptor>>((resolve, reject) => {
      try {
        taskpool.execute((): Array<FontDescriptor> => {
          return matchFontDescriptorsSync(desc);
        }).then((result) => {
          resolve(result);
        })
      } catch (err) {
        reject(err as BusinessError);
      }
    });
  }

  export enum SystemFontType {
    ALL = 1 << 0,
    GENERIC = 1 << 1,
    STYLISH = 1 << 2,
    INSTALLED = 1 << 3,
    CUSTOMIZED = 1 << 4,
  }

  export interface FontDescriptor {
    path?: string;
    postScriptName?: string;
    fullName?: string;
    fontFamily?: string;
    fontSubfamily?: string;
    weight?: FontWeight;
    width?: int;
    italic?: int;
    monoSpace?: boolean;
    symbolic?: boolean;
  }

  class FontDescriptorInternal implements FontDescriptor {
    path?: string;
    postScriptName?: string;
    fullName?: string;
    fontFamily?: string;
    fontSubfamily?: string;
    weight?: FontWeight;
    width?: int;
    italic?: int;
    monoSpace?: boolean;
    symbolic?: boolean;
    constructor(
      path: string,
      postScriptName: string,
      fullName: string,
      fontFamily: string,
      fontSubfamily: string,
      weight: FontWeight,
      width: int,
      italic: int,
      monoSpace: boolean,
      symbolic: boolean,
    ) {
      this.path = path;
      this.postScriptName = postScriptName;
      this.fullName = fullName;
      this.fontFamily = fontFamily;
      this.fontSubfamily = fontSubfamily;
      this.weight = weight;
      this.width = width;
      this.italic = italic;
      this.monoSpace = monoSpace;
      this.symbolic = symbolic;
    }
  }

  enum TextBadgeType {
    TEXT_BADGE_NONE,
    TEXT_SUPERSCRIPT,
    TEXT_SUBSCRIPT,
  }

  enum TextHighContrast {
    TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST,
    TEXT_APP_DISABLE_HIGH_CONTRAST,
    TEXT_APP_ENABLE_HIGH_CONTRAST,
  }

  native function setTextHighContrast(action: TextHighContrast): void;

  enum TextUndefinedGlyphDisplay {
    USE_DEFAULT,
    USE_TOFU,
  }

  native function setTextUndefinedGlyphDisplay(noGlyphShow: TextUndefinedGlyphDisplay): void;
}