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
 * @file
 * @kit ArkGraphics2D
 */
import { BusinessError } from '@ohos.base';
import type drawing from '@ohos.graphics.drawing';
import type common2D from '@ohos.graphics.common2D';
import { Resource } from 'global.resource';

/**
 * Provides functions such as 2D graphics text paragraphs, text styles.
 *
 * @namespace text
 * @syscap SystemCapability.Graphics.Drawing
 * @since 12
 */
namespace text {
  class Cleaner {
    static { loadLibrary('text_engine_ani.z') }
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

  /**
   * Provides the basis for graphics.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class FontCollection {
    static { loadLibrary('text_engine_ani.z') }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };
    native constructorNative(): void;
    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("FontCollection", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    /**
     * Get global FontCollection instance of the application.
     * @returns { FontCollection } The FontCollection object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native static getGlobalInstance(): FontCollection;

    /**
     * Load font.
     * @param { string } name - the font name.
     * @param { string | Resource } path - the path of the font file.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native loadFontSync(name: string, path: Resource | string): void;

    /**
     * Load font.
     * @param { string } name - The font name.
     * @param { string | Resource } path - The path of the font file.
     * @returns { Promise<void> } The promise returned by the function.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 14
     */
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

    /**
     * Clear font caches.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clearCaches(): void;
  }

  /**
   * Builds a Paragraph containing text with the given styling information.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class ParagraphBuilder {
    static { loadLibrary('text_engine_ani.z') }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    /**
     * Constructor ParagraphBuilder.
     * @param { ParagraphStyle } paragraphStyle - Paragraph style {@link ParagraphStyle}
     * @param { FontCollection } fontCollection - Font collection {@link FontCollection}
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    constructor(paragraphStyle: ParagraphStyle, fontCollection: FontCollection) {
      this.constructorNative(paragraphStyle, fontCollection);
      this.registerCleaner(this.nativeObj);
    };
    native constructorNative(paragraphStyle: ParagraphStyle, fontCollection: FontCollection): void;
    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("ParagraphBuilder", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    /**
     * Push a style to the stack.
     * @param { TextStyle } textStyle - Text style {@link TextStyle}
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native pushStyle(textStyle: TextStyle): void;

    /**
     * Remove a style from the stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native popStyle(): void;

    /**
     * Adds text to the builder.
     * @param { string } text - Text string
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native addText(text: string): void;

    /**
     * Create paragraph object.
     * @returns { Paragraph } The paragraph value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native build(): Paragraph;
  }

  /**
   * A paragraph retains the size and position of each glyph in the text and can be efficiently resized and painted.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Paragraph {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.registerCleaner(this.nativeObj);
    };
    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Paragraph", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    /**
     * Calculates the positioning of all the glyphs.
     * @param { number } width - Control how wide the text is allowed to be.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native layoutSync(width: number): void;

    async layout(width: number): Promise<void> {
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

    /**
     * Paint the laid out text onto the supplied canvas at (x, y).
     * @param { drawing.Canvas } canvas - Object
     * @param { number } x - Represents the X-axis position on the canvas.
     * @param { number } y - Represents the Y-axis position on the canvas.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native paint(canvas: drawing.Canvas, x: number, y: number): void;

    /**
     * Draw the laid out text onto the supplied canvas along the path and offset.
     * @param { drawing.Canvas } canvas - Canvas used to carry the drawn content and drawing status.
     * @param { drawing.Path } path - Path used to determine the position of the text.
     * @param { number } hOffset - Horizontal offset along the path.
     * @param { number } vOffset - Vertical offset along the path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native paintOnPath(canvas: drawing.Canvas, path: drawing.Path, hOffset: number, vOffset: number): void;

    /**
     * Get the longest line of horizontal space this paragraph occupies.
     * @returns { number } The longest line of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLongestLine(): number;

    /**
     * Returns the array of line metrics for a line of text.
     * @returns { Array<LineMetrics> } Array of line metrics.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLineMetrics(): Array<LineMetrics>;

    /**
     * Returns line metrics info for the line.
     * @param { number } lineNumber - a line number
     * @returns { LineMetrics | undefined } line metrics.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    getLineMetrics(lineNumber: number): LineMetrics | undefined {
        return this.nativeGetLineMetricsAt(lineNumber);
    };

    private native nativeGetLineMetricsAt(lineNumber: number): LineMetrics | undefined;
  }

  /**
   * Enumeration of font width of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum FontWidth {
    /**
     * Ultra condensed font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ULTRA_CONDENSED = 1,

    /**
     * Extra condensed font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    EXTRA_CONDENSED = 2,

    /**
     * Condensed font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    CONDENSED = 3,

    /**
     * Semi condensed font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    SEMI_CONDENSED = 4,

    /**
     * Normal font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NORMAL = 5,

    /**
     * Semi expanded font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    SEMI_EXPANDED = 6,

    /**
     * Expanded font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    EXPANDED = 7,

    /**
     * Extra expanded font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    EXTRA_EXPANDED = 8,

    /**
     * Ultra expanded font width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ULTRA_EXPANDED = 9,
  }


  /**
   * Enumerates of height mode of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum TextHeightBehavior {
    /**
     * Both ascend of first row and last row style.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ALL = 0x0,

    /**
     * Forbidding ascend of first row style.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DISABLE_FIRST_ASCENT = 0x1,

    /**
     * Forbidding ascend of last row style.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DISABLE_LAST_ASCENT = 0x2,

    /**
     * Neither ascend of first row nor last row style.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DISABLE_ALL = 0x1 | 0x2,
  }

  /**
   * Refers to how to align the horizontal position of text when displaying text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum TextAlign {
    /**
     * Use the left side of the text as a reference line for alignment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    LEFT = 0,

    /**
     * Use the right side of the text as a reference line for alignment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    RIGHT = 1,

    /**
     * Use the midpoint line the text as a reference line for alignment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    CENTER = 2,

    /**
     * Align the text at the start and end of the line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    JUSTIFY = 3,

    /**
     * Align text from start, based on the direction of text, such as left-to-right or right-to-left.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    START = 4,

    /**
     * Align text from end, based on the direction of text, such as left-to-right or right-to-left, opposite to START.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    END = 5,
  }

  /**
   * Enumerate text runs direction.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum TextDirection {
    /**
     * The text is oriented from right to left.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    RTL,

    /**
     * The text is oriented from left to right.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    LTR,
  }

  /**
   * Enumerate text segmentation strategy.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum BreakStrategy {
    /**
     * The segmentation strategy is greedy.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    GREEDY,

    /**
     * The segmentation strategy is high quality.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    HIGH_QUALITY,

    /**
     * The segmentation strategy is balanced.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    BALANCED,
  }

  /**
   * Enumerate word break strategy.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  enum WordBreak {
    /**
     * Normal word break strategy.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NORMAL,

    /**
     * Breaks word by character.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    BREAK_ALL,

    /**
     * Breaks word by phrase.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    BREAK_WORD,

    /**
     * Breaks word by hyphen.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    BREAK_HYPHEN,
  }

  /**
   * Describes strut style.
   * @typedef StrutStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  interface StrutStyle {
    /**
     * The families of the font to use when calculating the strut.
     * @type { ?Array<string> } fontfamily gather
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontFamilies?: Array<string>;

    /**
     * The font style to use when calculating the strut.
     * @type { ?FontStyle } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontStyle?: FontStyle;

    /**
     * The font width to use when calculating the strut.
     * @type { ?FontWidth } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontWidth?: FontWidth;

    /**
     * The font weight to use when calculating the strut.
     * @type { ?FontWeight } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontWeight?: FontWeight;

    /**
     * The size of the ascent plus descent in logical pixels.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontSize?: number;

    /**
     * The minimum height of the strut, as a multiple of fontSize.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    height?: number;

    /**
     * The additional leading to apply to the strut as a multiple of Size.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    leading?: number;

    /**
     * Whether the strut height should be forced.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    forceHeight?: boolean;

    /**
     * Whether the strut style should be enable.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    enabled?: boolean;

    /**
     * Whether the height is override.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    heightOverride?: boolean;

    /**
     * Whether the half leading is enable.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    halfLeading?: boolean;
  }

  /**
   * Text tab contains alignment type and location in paragraph style.
   * @typedef TextTab
   * @syscap SystemCapability.Graphics.Drawing
   * @since 18
   */
  interface TextTab {
    /**
     * The alignment of tab. Support left alignment right alignment center alignment,
     * other enumeration values are left alignment effect.
     * @type { TextAlign }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    alignment: TextAlign;
    
    /**
     * The position of the tab relative to the start of the line.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    location: number;
  }

  /**
   * Determines the configuration used by ParagraphBuilder to position lines within a Paragraph of text.
   * @typedef ParagraphStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface ParagraphStyle {
    /**
     * Text style of paragraph.
     * @type { ?TextStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textStyle?: TextStyle;

    /**
     * Text runs direction.
     * @type { ?TextDirection }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textDirection?: TextDirection;

    /**
     * Refers to how to align the horizontal position of text when displaying text.
     * @type { ?TextAlign }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    align?: TextAlign;

    /**
     * Word break strategy.
     * @type { ?WordBreak }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    wordBreak?: WordBreak;

    /**
     * Maximum number of lines.
     * @type { ?number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    maxLines?: number;

    /**
     * text segmentation strategy.
     * @type { ?BreakStrategy }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    breakStrategy?: BreakStrategy;

    /**
     * Strut style of paragraph.
     * @type { ?StrutStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    strutStyle?: StrutStyle;

    /**
     * Text height behavior of paragraph.
     * @type { ?TextHeightBehavior }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textHeightBehavior?: TextHeightBehavior;

    /**
     * Text tab of paragraph. Tab alignment does not take effect when text alignment is also set, Or when the ellipsis
     * style is configured. When the tab is not set or the tab's location property is less than or equal to 0,
     * it is the default space effect. And all tabs in the paragraph after the setting are aligned
     * according to this tab effect.
     * @type { ?TextTab }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    tab?: TextTab;
  }

  /**
   * Describes shadow of text.
   * @typedef TextShadow
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  interface TextShadow {
    /**
     * The color of text shadow.
     * @type { ?common2D.Color } The color of text shadow
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    color?: common2D.Color;

    /**
     * The value sets offset of text shadow that based on the original text.
     * @type { ?common2D.Point } The point of shadow
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    point?: common2D.Point;

    /**
     * The value sets special effect radius of blurring text, it default is 0.
     * @type { ?number } The value about radius of blur, it type is "double"
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    blurRadius?: number;
  }

  export class TextShadowInternal implements TextShadow {
    color?: common2D.Color;
    point?: common2D.Point;
    blurRadius?: number;
  }

  /**
   * Describes text style.
   * @typedef TextStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface TextStyle {
    /**
     * Decoration of text.
     * @type { ?Decoration } decoration for text
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    decoration?: Decoration;

    /**
     * Color of text.
     * @type { ?common2D.Color } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    color?: common2D.Color;

    /**
     * Font weight of text.
     * @type { ?FontWeight } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontWeight?: FontWeight;

    /**
     * Font style of text.
     * @type { ?FontStyle } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontStyle?: FontStyle;

    /**
     * Base line of text.
     * @type { ?TextBaseline } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baseline?: TextBaseline;

    /**
     * Font Families of text.
     * @type { ?Array<string> } fontfamily gather
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontFamilies?: Array<string>;

    /**
     * Font size of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontSize?: number;

    /**
     * Letter spacing of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    letterSpacing?: number;

    /**
     * Word spacing of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    wordSpacing?: number;

    /**
     * Height scale of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    heightScale?: number;

    /**
     * Half leading of text.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    halfLeading?: boolean;

    /**
     * Control the height calculation method of font blob, true means calculate the height of the blob by
     * the font size, false means by the line height and leading.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    heightOnly?: boolean;

    /**
     * Text ellipsis.
     * @type { ?string } it is u16string type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ellipsis?: string;

    /**
     * Text ellipsis mode.
     * @type { ?EllipsisMode } Ellipsis mode.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ellipsisMode?: EllipsisMode;

    /**
     * Text locale.
     * @type { ?string } it is string type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    locale?: string;

    /**
     * The offset distance that the underline of text.
     * @type { ?number } it is double type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baselineShift?: number;

    /**
     * Text Style available font features.
     * @type { ?Array<FontFeature> } A collection of font features.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontFeatures?: Array<FontFeature>;

    /**
     * Text shadows of text.
     * @type { ?Array<TextShadow> } textShadow gather.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textShadows?: Array<TextShadow>;

    /**
     * Rect style of text.
     * @type { ?RectStyle } rect style for text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    backgroundRect?: RectStyle;

    /**
     * Text Style available font variations.
     * @type { ?Array<FontVariation> } A collection of font variations.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontVariations?: Array<FontVariation>;
  }

  export class TextStyleInternal implements TextStyle {
    decoration?: Decoration;
    color?: common2D.Color;
    fontWeight?: FontWeight;
    fontStyle?: FontStyle;
    baseline?: TextBaseline;
    fontFamilies?: Array<string>;
    fontSize?: number;
    letterSpacing?: number;
    wordSpacing?: number;
    heightScale?: number;
    halfLeading?: boolean;
    heightOnly?: boolean;
    ellipsis?: string;
    ellipsisMode?: EllipsisMode;
    locale?: string;
    baselineShift?: number;
    fontFeatures?: Array<FontFeature>;
    textShadows?: Array<TextShadow>;
    backgroundRect?: RectStyle;
    fontVariations?: Array<FontVariation>;
  }


  /**
   * Describes the metric information for a line of text in a text layout.
   * @typedef LineMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  interface LineMetrics {
    /**
     * The indexes in the text buffer the line begins.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    startIndex: number;

    /**
     * The indexes in the text buffer the line ends.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    endIndex: number;

    /**
     * The height of the text rise, the distance from the baseline to the top of the character.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ascent: number;

    /**
     * The height of the text drop, the distance from the baseline to the bottom of the character.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    descent: number;

    /**
     * The height of the current line is `round(ascent + descent)`.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    height: number;

    /**
     * Width of the line.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    width: number;

    /**
     * The left edge of the line. The right edge can be obtained with `left + width`.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    left: number;

    /**
     * The y position of the baseline for this line from the top of the paragraph.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baseline: number;

    /**
     * Zero indexed line number.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    lineNumber: number;

    /**
     * Height from the top.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    topHeight: number;

    /**
     * Mapping between text index ranges and the FontMetrics associated with
     * them. The first run will be keyed under start_index. The metrics here.
     * are before layout and are the base values we calculate from.
     * @type { Map<number, RunMetrics> }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    runMetrics: Map<number, RunMetrics>;
  }

  export class LineMetricsInternal implements LineMetrics {
    startIndex: number;
    endIndex: number;
    ascent: number;
    descent: number;
    height: number;
    width: number;
    left: number;
    baseline: number;
    lineNumber: number;
    topHeight: number;
    runMetrics: Map<number, RunMetrics>;
  }

  /**
   * Enumeration of font weight of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum FontWeight {
    /**
     * Thin
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W100,

    /**
     * Extra-light
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W200,

    /**
     * Light
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W300,

    /**
     * Normal/Regular
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W400,

    /**
     * Medium
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W500,

    /**
     * Semi-bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W600,

    /**
     * Bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W700,

    /**
     * Extra-bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W800,

    /**
     * Black
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W900,
  }

  /**
   * Enumeration of font style of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum FontStyle {
    /**
     * Upright font type.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NORMAL,

    /**
     * Slant font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ITALIC,

    /**
     * Oblique font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    OBLIQUE,
  }

  /**
   * Enumeration the type of text baseline.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextBaseline {
    /**
     * The alphabetic baseline, typically used for Latin-based scripts where the baseline aligns
     * with the base of lowercase letters.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ALPHABETIC,

    /**
     * The ideographic baseline, commonly used for ideographic scripts such as Chinese, Japanese, and Korean,
     * where the baseline aligns with the center of characters.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    IDEOGRAPHIC,
  }

  /**
   * Enumerates of ellipsis mode.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum EllipsisMode {
    /**
     * The ellipsis is shown in the start of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    START,

    /**
     * The ellipsis is shown in the middle of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    MIDDLE,

    /**
     * The ellipsis is shown in the end of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    END,
  }

  /**
   * Decoration for text.
   * @typedef Decoration
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface Decoration {
    /**
     * Decorates text by line.
     * @type { ?TextDecorationType }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textDecoration?: TextDecorationType;

    /**
     * Text color.
     * @type { ?common2D.Color }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    color?: common2D.Color;

    /**
     * Text decoration style.
     * @type { ?TextDecorationStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    decorationStyle?: TextDecorationStyle;

    /**
     * The thickness scale of decoration line.
     * @type { ?number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    decorationThicknessScale?: number;
  }

  export class DecorationInternal implements Decoration {
    textDecoration?: TextDecorationType;
    color?: common2D.Color;
    decorationStyle?: TextDecorationStyle;
    decorationThicknessScale?: number;
  }

  /**
   * Describes font feature of text.
   * @typedef FontFeature
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface FontFeature {
    /**
     * The name of font feature.
     * @type { string } feature name
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    name: string;

    /**
     * The value of font feature.
     * @type { number } feature value
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    value: number;
  }
  
  export class FontFeatureInternal implements FontFeature {
    name: string;
    value: number;
  }
  
  /**
   * Enumerates decoration line for text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextDecorationType {
    /**
     * There are no text decoration.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NONE = 0x0,

    /**
     * There is a decoration line below the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    UNDERLINE = 0x1,

    /**
     * There is a decoration line above the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    OVERLINE = 0x2,

    /**
     * There is a decoration line through the middle of the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    LINE_THROUGH = 0x4,
  }

  /**
   * Enumerates decoration line style.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextDecorationStyle {
    /**
     * Decoration line is solid line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    SOLID,

    /**
     * Decoration line is double line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DOUBLE,

    /**
     * Decoration line is dotted line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DOTTED,

    /**
     * Decoration line is dashed line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DASHED,

    /**
     * Decoration line is wavy line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    WAVY,
  }

  /**
   * Describes the layout information and metrics for a continuous piece of text (a run) in a line of text.
   * @typedef RunMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface RunMetrics {
    /**
     * The metrics of an Font.
     * @type { TextStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textStyle: TextStyle;

    /**
     * Describes text style.
     * @type { drawing.FontMetrics }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontMetrics: drawing.FontMetrics;
  }

  export class RunMetricsInternal implements RunMetrics {
    textStyle: TextStyle;
    fontMetrics: drawing.FontMetrics;
  }

  /**
   * Describes rect style of text.
   * @typedef RectStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface RectStyle {
    /**
     * The color of rect style.
     * @type { common2D.Color } The color of rect style
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    color: common2D.Color;

    /**
     * Radius in left top of rect style.
     * @type { number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    leftTopRadius: number;

    /**
     * Radius in right top of rect style.
     * @type { number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    rightTopRadius: number;

    /**
     * Radius in right bottom of rect style.
     * @type { number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    rightBottomRadius: number;

    /**
     * Radius in left bottom of rect style.
     * @type { number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    leftBottomRadius: number;
  }

  export class RectStyleInternal implements RectStyle {
    color: common2D.Color;
    leftTopRadius: number;
    rightTopRadius: number;
    rightBottomRadius: number;
    leftBottomRadius: number;
  }

  /**
   * Describes font variation of text.
   * @typedef FontVariation
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface FontVariation {
    /**
     * The axis of font variation.
     * @type { string } variation axis
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    axis: string;

    /**
     * The value of font variation.
     * @type { number } variation value
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    value: number;
  }

  export class FontVariationInternal implements FontVariation {
    axis: string;
    value: number;
  }
}