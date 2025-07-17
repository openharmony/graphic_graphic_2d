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
     * Add placeholder.
     * @param { PlaceholderSpan } placeholderSpan - Placeholder Span {@link PlaceholderSpan}
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native addPlaceholder(placeholderSpan: PlaceholderSpan): void;

    /**
     * Create paragraph object.
     * @returns { Paragraph } The paragraph value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native build(): Paragraph;

    /**
     * Create LineTypeset object.
     * @returns { LineTypeset } The LineTypeset value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 14
     */
    native buildLineTypeset(): LineTypeset;

    /**
     * Add symbolId.
     * @param { number } symbolId - Symbol Id
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native addSymbol(symbolId: number): void;
  }

  /**
   * A paragraph retains the size and position of each glyph in the text and can be efficiently resized and painted.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Paragraph {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    public nativeParagraphObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.registerCleaner(this.nativeObj);
    };
    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Typography", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    /**
     * Calculates the positioning of all the glyphs.
     * @param { number } width - Control how wide the text is allowed to be.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native layoutSync(width: number): void;

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
     * Get max width of horizontal space this paragraph occupied.
     * @returns { number } Max width of horizontal space.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getMaxWidth(): number;

    /**
     * Get height of horizontal space this paragraph occupies.
     * @returns { number } Height of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getHeight(): number;

    /**
     * Get the longest line of horizontal space this paragraph occupies.
     * @returns { number } The longest line of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLongestLine(): number;

    /**
     * Get the longest line of horizontal space this paragraph occupies, and this horizontal space contains the width
     * of indent.
     * @returns { number } The longest line with indent of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 13
     */
    native getLongestLineWithIndent(): number;

    /**
     * Get the min intrinsic width of horizontal space this paragraph occupies.
     * @returns { number } The min intrinsic width of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getMinIntrinsicWidth(): number;

    /**
     * Get the max intrinsic width.
     * @returns { number } Intrinsic Width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getMaxIntrinsicWidth(): number;

    /**
     * Get the alphabetic baseline.
     * @returns { number } Alphabetic Baseline.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getAlphabeticBaseline(): number;

    /**
     * Get the ideographic baseline.
     * @returns { number } Ideographic Baseline.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getIdeographicBaseline(): number;

    /**
     * Get the rects for range.
     * @param { Range } range - The range to set.
     * @param { RectWidthStyle } widthStyle - Width style to set.
     * @param { RectHeightStyle } heightStyle - Height style to set.
     * @returns { Array<TextBox> } The rects for range.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getRectsForRange(range: Range, widthStyle: RectWidthStyle, heightStyle: RectHeightStyle): Array<TextBox>;

    /**
     * Get the rects for placeholders.
     * @returns { Array<TextBox> } The rects for placeholders.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getRectsForPlaceholders(): Array<TextBox>;

    /**
     * Get the glyph position at coordinate.
     * @param { number } x - the positionX of typography to set.
     * @param { number } y - the positionY of typography to set.
     * @returns { PositionWithAffinity } TextBlob object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getGlyphPositionAtCoordinate(x: number, y: number): PositionWithAffinity;

    /**
     * Find the start and end position of the word containing the glyphs of the given offset.
     * @param { number } offset - offset value
     * @returns { Range } The range value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getWordBoundary(offset: number): Range;

    /**
     * Get line count.
     * @returns { number } The line count value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLineCount(): number;

    /**
     * Get the line height of the specified line.
     * @param { number } line - line number
     * @returns { number } The line height value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLineHeight(line: number): number;

    /**
     * Get the line width of the specified line.
     * @param { number } line - line number
     * @returns { number } The line width value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLineWidth(line: number): number;

    /**
     * Return whether it exceed the maximum lines of typography.
     * @returns { boolean } The true indicates exceeding, the false indicates not exceeding.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native didExceedMaxLines(): boolean;

    /**
     * Get the text lines of paragraph.
     * @returns { Array<TextLine> } the tuple of TextLine.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getTextLines(): Array<TextLine>;

    /**
     * Returns the visible text on the line (excluding a possible ellipsis).
     * @param { number } lineNumber - a line number
     * @param { boolean } includeSpaces - indicates if the whitespaces should be included
     * @returns { Range } The range of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getActualTextRange(lineNumber: number, includeSpaces: boolean): Range;

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

  class TextShadowInternal implements TextShadow {
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

  class TextStyleInternal implements TextStyle {
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

  class LineMetricsInternal implements LineMetrics {
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

  class DecorationInternal implements Decoration {
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
  
  class FontFeatureInternal implements FontFeature {
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

  class RunMetricsInternal implements RunMetrics {
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

  class RectStyleInternal implements RectStyle {
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

  class FontVariationInternal implements FontVariation {
    axis: string;
    value: number;
  }

  /**
   * Box that contain text.
   * @typedef TextBox
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface TextBox {
    /**
     * Rect of text box.
     * @type { common2D.Rect }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    rect: common2D.Rect;

    /**
     * Text direction.
     * @type { TextDirection }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    direction: TextDirection;
  }

  class TextBoxInternal implements TextBox {
    rect: common2D.Rect;
    direction: TextDirection;
  }

  class RectInternal implements common2D.Rect {
    left: number;
    top: number;
    right: number;
    bottom: number;
  }

  /**
   * Provides the definition of the range.
   * @typedef Range
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface Range {
    /**
     * Left index.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    start: number;

    /**
     * Right index.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    end: number;
  }

  class RangeInternal implements Range {
    start: number;
    end: number;
  }

  /**
   * Enumerates rect width style.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum RectWidthStyle {
    /**
     * Tight width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    TIGHT,

    /**
     * Max width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    MAX,
  }

  /**
   * Enumerates rect height style.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum RectHeightStyle {
    /**
     * Provide tight bounding boxes that fit heights per run.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    TIGHT,

    /**
     * The height of the boxes will be the maximum height of all runs in the line. All rects in the same
     * line will be the same height.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    MAX,

    /**
     * The top and bottom of each rect will cover half of the space above and half of the space below the line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INCLUDE_LINE_SPACE_MIDDLE,

    /**
     * The line spacing will be added to the top of the rect.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INCLUDE_LINE_SPACE_TOP,

    /**
     * The line spacing will be added to the bottom of the rect.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INCLUDE_LINE_SPACE_BOTTOM,

    /**
     * The height of the boxes will be calculated by text strut.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    STRUT,
  }

  /**
   * Position and affinity.
   * @typedef PositionWithAffinity
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface PositionWithAffinity {
    /**
     * Position of text.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    position: number;

    /**
     * Affinity of text.
     * @type { Affinity }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    affinity: Affinity;
  }

  class PositionWithAffinityInternal implements PositionWithAffinity {
    position: number;
    affinity: Affinity;
  }

  /**
   * Enumerates text affinity.When a selection range involves line breaks or other special characters, the
   * affinity determines which side of the characters the start and end of the selection range should be
   * closer to.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum Affinity {
    /**
     * The position has affinity for the upstream side of the text position.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */

    UPSTREAM,
    /**
     * The position has affinity for the downstream side of the text position.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DOWNSTREAM,
  }

  /**
   * Offset callback function of caret.
   *
   * @typedef { function } CaretOffsetsCallback
   * @param { number } offset - Character offset is traversed as an argument to the callback function.
   * @param { number } index - Character index is traversed as an argument to the callback function.
   * @param { boolean } leadingEdge - Whether the current offset is at the character front, as an argument to the
   * callback function.
   * @returns { boolean } The return value of the user-defined callback function. If false is returned, the traversal
   * continues. If true is returned, the traversal stops.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 18
   */
  type CaretOffsetsCallback = (offset: number, index: number, leadingEdge: boolean) => boolean;

  /**
   * The structure of text line that provides the basis of paragraph for graphics.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class TextLine {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    public nativeParagraphObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.registerCleaner("TextLine", this.nativeObj);
      this.registerCleaner("Typography", this.nativeParagraphObj);
    };
    registerCleaner(className: string, ptr: long): void {
      this.cleaner = new Cleaner(className , ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    /**
     * Get the count of glyphs.
     * @returns { number } The counts of glyphs.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getGlyphCount(): number;

    /**
     * Get the range of text line.
     * @returns { Range } The range of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getTextRange(): Range;

    /**
     * Get the glyph runs of text line.
     * @returns { Array<Run> } The tuple of glyph runs of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getGlyphRuns(): Array<Run>;

    /**
     * Paint the range of text line.
     * @param { drawing.Canvas } canvas - Canvas.
     * @param { number } x - Represents the X-axis position on the canvas.
     * @param { number } y - Represents the Y-axis position on the canvas.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native paint(canvas: drawing.Canvas, x: number, y: number): void;

    /**
     * Creates a truncated text line object.
     * @param { number } width - The width of the truncated line.
     * @param { EllipsisMode } ellipsisMode - Text ellipsis mode, EllipsisMode:MIDDLE is not supported.
     * @param { string } ellipsis - Text ellipsis.
     * @returns { TextLine } Truncated text line object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native createTruncatedLine(width: number, ellipsisMode: EllipsisMode, ellipsis: string): TextLine;

    /**
     * Gets the text line typographic bounds.
     * @returns { TypographicBounds } The text line of typographic bounds.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getTypographicBounds(): TypographicBounds;

    /**
     * Gets the text line image bounds.
     * @returns { common2D.Rect } Rect of text line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getImageBounds(): common2D.Rect;

    /**
     * Gets the tail space width.
     * @returns { number } The tail space width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getTrailingSpaceWidth(): number;

    /**
     * Gets the string index of the given position.
     * @param { common2D.Point } point - The given position.
     * @returns { number } The string index for a given position.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getStringIndexForPosition(point: common2D.Point): number;

    /**
     * Gets the offset of the given string index.
     * @param { number } index - The given string index.
     * @returns { number } The offset for a given string index.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getOffsetForStringIndex(index: number): number;

    /**
     * Enumerate caret offset and index in text lines.
     * @param { CaretOffsetsCallback } callback - User-defined callback functions.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native enumerateCaretOffsets(callback: CaretOffsetsCallback): void;

    /**
     * Gets the text offset based on the given alignment factor and alignment width.
     * @param { number } alignmentFactor - The coefficients that text needs to be aligned.
     *                                     Less than or equal to 0 is left justified, 0.5 is center justified,
     *                                     and greater than or equal to 1 is right justified.
     * @param { number } alignmentWidth - The width of the text to be aligned.
     *                                    Returns 0 if it is less than the actual width of the text.
     * @returns { number } The offset of the aligned text.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getAlignmentOffset(alignmentFactor: number, alignmentWidth: number): number;
  }

  /**
   * Independent rendering of text layout.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Run {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    public nativeParagraphObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.registerCleaner("Run", this.nativeObj);
      this.registerCleaner("Typography", this.nativeParagraphObj);
    };
    registerCleaner(className: string, ptr: long): void {
      this.cleaner = new Cleaner(className , ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    /**
     * Gets the number of glyph.
     * @returns { number } The number of glyph.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getGlyphCount(): number;

    /**
     * Gets the glyph identifier for each character.
     * @returns { Array<number> } Glyph identifier.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getGlyphs(): Array<number>;

    /**
     * Gets the range glyph identifier for each character.
     * @param { Range } range of run, range.start is the starting index of the run block, starting from 0.
     * range.end is run length, if range.start and range.end are set to 0, then get all of the current run.
     * @returns { Array<number> } Glyph identifier or undefined.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    getGlyphs(range: Range): Array<number> {
      return this.nativeGetGlyphs(range);
    }
    private native nativeGetGlyphs(range: Range): Array<number>;

    /**
     * Gets the font position offset.
     * @returns { Array<common2D.Point> } The position of the font in the layout.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getPositions(): Array<common2D.Point>;

    /**
     * Gets the range font position offset.
     * @param { Range } range of run, range.start is the starting index of the run block, starting from 0.
     * range.end is run length, if range.start and range.end are set to 0, then get all of the current run.
     * @returns { Array<common2D.Point> } The position of the font in the layout or undefined.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    getPositions(range: Range): Array<common2D.Point> {
      return this.nativeGetPositions(range);
    }
    private native nativeGetPositions(range: Range): Array<common2D.Point>;

	  /**
     * Gets the font position offset array.
     * @returns { Array<common2D.Point> } The position offset of the font in the layout.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getOffsets(): Array<common2D.Point>;

    /**
     * Gets the font object instance.
     * @returns { drawing.Font } The font object instance.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getFont(): drawing.Font;

    /**
     * Paint the laid out text onto the supplied canvas at (x, y).
     * @param { drawing.Canvas } canvas - Object.
     * @param { number } x - Represents the X-axis position on the canvas.
     * @param { number } y - Represents the Y-axis position on the canvas.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native paint(canvas: drawing.Canvas, x: number, y: number): void;

    /**
     * Gets the range of run glyph indices, the offset of the indices relative to the entire paragraph.
     * @param { Range } range of run, range.start is the starting index of the run block, starting from 0.
     * range.end is run length, if range.start range.and end are set to 0, then get all of the current run.
     * @returns { Array<number> } The glyph indices or undefined.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getStringIndices(range?: Range): Array<number>;

    /**
     * Gets the run glyph location and length.
     * @returns { Range } The run of glyph location and length, Range.start is location, Range.end is length.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getStringRange(): Range;

    /**
     * Gets the run typographic bounds.
     * @returns { TypographicBounds } The run of typographic bounds.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getTypographicBounds(): TypographicBounds;

    /**
     * Gets the run image bounds.
     * @returns { common2D.Rect } The run rect bounds.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native getImageBounds(): common2D.Rect;
  }

  /**
   * Provide a description of placeholder scope in creating typography.
   * @typedef PlaceholderSpan
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface PlaceholderSpan {
    /**
     * The width of the placeholder.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    width: number;

    /**
     * The height of the placeholder.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    height: number;

    /**
     * Alignment mode of placeholder.
     * @type { PlaceholderAlignment }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    align: PlaceholderAlignment;

    /**
     * Baseline of placeholder.
     * @type { TextBaseline }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baseline: TextBaseline;

    /**
     * Baseline offset of placeholder.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baselineOffset: number;
  }

  /**
   * Where to vertically align the placeholder relative to the surrounding text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum PlaceholderAlignment {
    /**
     * Match the baseline of the placeholder with the baseline.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    OFFSET_AT_BASELINE,

    /**
     * Align the bottom edge of the placeholder with the baseline such that the placeholder
     * sits on top of the baseline.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ABOVE_BASELINE,

    /**
     * Align the top edge of the placeholder with the baseline specified in such that the placeholder
     * hangs below the baseline.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    BELOW_BASELINE,

    /**
     * Align the top edge of the placeholder with the top edge of the font. When the placeholder is very tall,
     * the extra space will hang from the top and extend through the bottom of the line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    TOP_OF_ROW_BOX,

    /**
     * Align the bottom edge of the placeholder with the bottom edge of the text. When the placeholder is very tall,
     * the extra space will rise from the bottom and extend through the top of the line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    BOTTOM_OF_ROW_BOX,

    /**
     * Align the middle of the placeholder with the middle of the text.When the placeholder is very tall,
     * the extra space will grow equally from the top and bottom of the line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    CENTER_OF_ROW_BOX,
  }

  /**
   * Provides the abilities to typeset by line.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 14
   */
  export class LineTypeset {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    constructor() {
      this.registerCleaner(this.nativeObj);
    };
    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("LineTypeset", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }
    /**
     * Calculate the line breakpoint based on the width provided.
     * @param { number } startIndex - The starting point for the line-break calculations.
     * @param { number } width - The requested line-break width.
     * @returns { number } A count of the characters from startIndex that would cause the line break.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 14
     */
    native getLineBreak(startIndex: number, width: number): number;

    /**
     * Creates a text line object based on the text range provided.
     * @param { number } startIndex - The starting index of the text range.
     * @param { number } count - The characters count of the text range.
     * @returns { TextLine } Text line object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 14
     */
    native createLine(startIndex: number, count: number): TextLine;
  }

  /**
   * Provides the definition of the typographic bounds.
   * @typedef TypographicBounds
   * @syscap SystemCapability.Graphics.Drawing
   * @since 18
   */
  export interface TypographicBounds {
    /**
     * Distance Retained Above Baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    ascent: number;

    /**
     * The distance that remains below the baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    descent: number;

    /**
     * Line Spacing.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    leading: number;

    /**
     * The total width of the typesetting border.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    width: number;
  }

  class TypographicBoundsInternal implements TypographicBounds {
    ascent: number;
    descent: number;
    leading: number;
    width: number;
  }

  class PointInternal implements common2D.Point {
    x: number;
    y: number;
  }
}