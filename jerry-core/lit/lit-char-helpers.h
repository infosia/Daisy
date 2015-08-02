/* Copyright 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIT_CHAR_HELPERS_H
#define LIT_CHAR_HELPERS_H

#include "lit-globals.h"

/*
 * Format control characters (ECMA-262 v5, Table 1)
 */
#define LIT_CHAR_ZWNJ ((ecma_char_t) 0x200C) /* zero width non-joiner */
#define LIT_CHAR_ZWJ  ((ecma_char_t) 0x200D) /* zero width joiner */
#define LIT_CHAR_BOM  ((ecma_char_t) 0xFEFF) /* byte order mark */

extern bool lit_char_is_format_control (ecma_char_t);

/*
 * Whitespace characters (ECMA-262 v5, Table 2)
 */
#define LIT_CHAR_TAB  ((ecma_char_t) 0x0009) /* tab */
#define LIT_CHAR_VTAB ((ecma_char_t) 0x000B) /* vertical tab */
#define LIT_CHAR_FF   ((ecma_char_t) 0x000C) /* form feed */
#define LIT_CHAR_SP   ((ecma_char_t) 0x0020) /* space */
#define LIT_CHAR_NBSP ((ecma_char_t) 0x00A0) /* no-break space */
/* LIT_CHAR_BOM is defined above */

extern bool lit_char_is_space_separator (ecma_char_t);
extern bool lit_char_is_white_space (ecma_char_t);

/*
 * Line terminator characters (ECMA-262 v5, Table 3)
 */
#define LIT_CHAR_LF ((ecma_char_t) 0x000A) /* line feed */
#define LIT_CHAR_CR ((ecma_char_t) 0x000D) /* carriage return */
#define LIT_CHAR_LS ((ecma_char_t) 0x2028) /* line separator */
#define LIT_CHAR_PS ((ecma_char_t) 0x2029) /* paragraph separator */

extern bool lit_char_is_line_terminator (ecma_char_t);

/*
 * String Single Character Escape Sequences (ECMA-262 v5, Table 4)
 */
#define LIT_CHAR_BS           ((ecma_char_t) 0x0008) /* backspace */
/* LIT_CHAR_TAB is defined above */
/* LIT_CHAR_LF is defined above */
/* LIT_CHAR_VTAB is defined above */
/* LIT_CHAR_FF is defined above */
/* LIT_CHAR_CR is defined above */
#define LIT_CHAR_DOUBLE_QUOTE ((ecma_char_t) '"') /* double quote */
#define LIT_CHAR_SINGLE_QUOTE ((ecma_char_t) '\'') /* single quote */
#define LIT_CHAR_BACKSLASH    ((ecma_char_t) '\\') /* reverse solidus (backslash) */

/*
 * Comment characters (ECMA-262 v5, 7.4)
 */
#define LIT_CHAR_SLASH    ((ecma_char_t) '/') /* solidus */
#define LIT_CHAR_ASTERISK ((ecma_char_t) '*') /* asterisk */

/*
 * Identifier name characters (ECMA-262 v5, 7.6)
 */
#define LIT_CHAR_DOLLAR_SIGN ((ecma_char_t) '$')  /* dollar sign */
#define LIT_CHAR_UNDERSCORE  ((ecma_char_t) '_')  /* low line (underscore) */
/* LIT_CHAR_BACKSLASH defined above */

extern bool lit_char_is_unicode_letter (ecma_char_t);
extern bool lit_char_is_unicode_combining_mark (ecma_char_t);
extern bool lit_char_is_unicode_digit (ecma_char_t);
extern bool lit_char_is_unicode_connector_punctuation (ecma_char_t);

/*
 * Punctuator characters (ECMA-262 v5, 7.7)
 */
#define LIT_CHAR_LEFT_BRACE   ((ecma_char_t) '{') /* left curly bracket */
#define LIT_CHAR_RIGHT_BRACE  ((ecma_char_t) '}') /* right curly bracket */
#define LIT_CHAR_LEFT_PAREN   ((ecma_char_t) '(') /* left parenthesis */
#define LIT_CHAR_RIGHT_PAREN  ((ecma_char_t) ')') /* right parenthesis */
#define LIT_CHAR_LEFT_SQUARE  ((ecma_char_t) '[') /* left square bracket */
#define LIT_CHAR_RIGHT_SQUARE ((ecma_char_t) ']') /* right square bracket */
#define LIT_CHAR_DOT          ((ecma_char_t) '.') /* dot */
#define LIT_CHAR_SEMICOLON    ((ecma_char_t) ';') /* semicolon */
#define LIT_CHAR_COMMA        ((ecma_char_t) ',') /* comma */
#define LIT_CHAR_LESS_THAN    ((ecma_char_t) '<') /* less-than sign */
#define LIT_CHAR_GREATER_THAN ((ecma_char_t) '>') /* greater-than sign */
#define LIT_CHAR_EQUALS       ((ecma_char_t) '=') /* equals sign */
#define LIT_CHAR_PLUS         ((ecma_char_t) '+') /* plus sign */
#define LIT_CHAR_MINUS        ((ecma_char_t) '-') /* hyphen-minus */
/* LIT_CHAR_ASTERISK is defined above */
#define LIT_CHAR_PERCENT      ((ecma_char_t) '%') /* percent sign */
#define LIT_CHAR_AMPERSAND    ((ecma_char_t) '&') /* ampersand */
#define LIT_CHAR_VLINE        ((ecma_char_t) '|') /* vertical line */
#define LIT_CHAR_CIRCUMFLEX   ((ecma_char_t) '^') /* circumflex accent */
#define LIT_CHAR_EXCLAMATION  ((ecma_char_t) '!') /* exclamation mark */
#define LIT_CHAR_TILDE        ((ecma_char_t) '~') /* tilde */
#define LIT_CHAR_QUESTION     ((ecma_char_t) '?') /* question mark */
#define LIT_CHAR_COLON        ((ecma_char_t) ':') /* colon */

/*
 * Special characters for String.prototype.replace.
 */
#define LIT_CHAR_GRAVE_ACCENT ((ecma_char_t) '`') /* grave accent */

/**
 * Uppercase ASCII letters
 */
#define LIT_CHAR_UPPERCASE_A ((ecma_char_t) 'A')
#define LIT_CHAR_UPPERCASE_B ((ecma_char_t) 'B')
#define LIT_CHAR_UPPERCASE_C ((ecma_char_t) 'C')
#define LIT_CHAR_UPPERCASE_D ((ecma_char_t) 'D')
#define LIT_CHAR_UPPERCASE_E ((ecma_char_t) 'E')
#define LIT_CHAR_UPPERCASE_F ((ecma_char_t) 'F')
#define LIT_CHAR_UPPERCASE_G ((ecma_char_t) 'G')
#define LIT_CHAR_UPPERCASE_H ((ecma_char_t) 'H')
#define LIT_CHAR_UPPERCASE_I ((ecma_char_t) 'I')
#define LIT_CHAR_UPPERCASE_J ((ecma_char_t) 'J')
#define LIT_CHAR_UPPERCASE_K ((ecma_char_t) 'K')
#define LIT_CHAR_UPPERCASE_L ((ecma_char_t) 'L')
#define LIT_CHAR_UPPERCASE_M ((ecma_char_t) 'M')
#define LIT_CHAR_UPPERCASE_N ((ecma_char_t) 'N')
#define LIT_CHAR_UPPERCASE_O ((ecma_char_t) 'O')
#define LIT_CHAR_UPPERCASE_P ((ecma_char_t) 'P')
#define LIT_CHAR_UPPERCASE_Q ((ecma_char_t) 'Q')
#define LIT_CHAR_UPPERCASE_R ((ecma_char_t) 'R')
#define LIT_CHAR_UPPERCASE_S ((ecma_char_t) 'S')
#define LIT_CHAR_UPPERCASE_T ((ecma_char_t) 'T')
#define LIT_CHAR_UPPERCASE_U ((ecma_char_t) 'U')
#define LIT_CHAR_UPPERCASE_V ((ecma_char_t) 'V')
#define LIT_CHAR_UPPERCASE_W ((ecma_char_t) 'W')
#define LIT_CHAR_UPPERCASE_X ((ecma_char_t) 'X')
#define LIT_CHAR_UPPERCASE_Y ((ecma_char_t) 'Y')
#define LIT_CHAR_UPPERCASE_Z ((ecma_char_t) 'Z')

/**
 * Lowercase ASCII letters
 */
#define LIT_CHAR_LOWERCASE_A ((ecma_char_t) 'a')
#define LIT_CHAR_LOWERCASE_B ((ecma_char_t) 'b')
#define LIT_CHAR_LOWERCASE_C ((ecma_char_t) 'c')
#define LIT_CHAR_LOWERCASE_D ((ecma_char_t) 'd')
#define LIT_CHAR_LOWERCASE_E ((ecma_char_t) 'e')
#define LIT_CHAR_LOWERCASE_F ((ecma_char_t) 'f')
#define LIT_CHAR_LOWERCASE_G ((ecma_char_t) 'g')
#define LIT_CHAR_LOWERCASE_H ((ecma_char_t) 'h')
#define LIT_CHAR_LOWERCASE_I ((ecma_char_t) 'i')
#define LIT_CHAR_LOWERCASE_J ((ecma_char_t) 'j')
#define LIT_CHAR_LOWERCASE_K ((ecma_char_t) 'k')
#define LIT_CHAR_LOWERCASE_L ((ecma_char_t) 'l')
#define LIT_CHAR_LOWERCASE_M ((ecma_char_t) 'm')
#define LIT_CHAR_LOWERCASE_N ((ecma_char_t) 'n')
#define LIT_CHAR_LOWERCASE_O ((ecma_char_t) 'o')
#define LIT_CHAR_LOWERCASE_P ((ecma_char_t) 'p')
#define LIT_CHAR_LOWERCASE_Q ((ecma_char_t) 'q')
#define LIT_CHAR_LOWERCASE_R ((ecma_char_t) 'r')
#define LIT_CHAR_LOWERCASE_S ((ecma_char_t) 's')
#define LIT_CHAR_LOWERCASE_T ((ecma_char_t) 't')
#define LIT_CHAR_LOWERCASE_U ((ecma_char_t) 'u')
#define LIT_CHAR_LOWERCASE_V ((ecma_char_t) 'v')
#define LIT_CHAR_LOWERCASE_W ((ecma_char_t) 'w')
#define LIT_CHAR_LOWERCASE_X ((ecma_char_t) 'x')
#define LIT_CHAR_LOWERCASE_Y ((ecma_char_t) 'y')
#define LIT_CHAR_LOWERCASE_Z ((ecma_char_t) 'z')

/**
 * ASCII decimal digits
 */
#define LIT_CHAR_0    ((ecma_char_t) '0')
#define LIT_CHAR_1    ((ecma_char_t) '1')
#define LIT_CHAR_2    ((ecma_char_t) '2')
#define LIT_CHAR_3    ((ecma_char_t) '3')
#define LIT_CHAR_4    ((ecma_char_t) '4')
#define LIT_CHAR_5    ((ecma_char_t) '5')
#define LIT_CHAR_6    ((ecma_char_t) '6')
#define LIT_CHAR_7    ((ecma_char_t) '7')
#define LIT_CHAR_8    ((ecma_char_t) '8')
#define LIT_CHAR_9    ((ecma_char_t) '9')

/**
 * ASCII character ranges
 */
#define LIT_CHAR_ASCII_UPPERCASE_LETTERS_BEGIN      LIT_CHAR_UPPERCASE_A /* uppercase letters range */
#define LIT_CHAR_ASCII_UPPERCASE_LETTERS_END        LIT_CHAR_UPPERCASE_Z

#define LIT_CHAR_ASCII_LOWERCASE_LETTERS_BEGIN      LIT_CHAR_LOWERCASE_A /* lowercase letters range */
#define LIT_CHAR_ASCII_LOWERCASE_LETTERS_END        LIT_CHAR_LOWERCASE_Z

#define LIT_CHAR_ASCII_UPPERCASE_LETTERS_HEX_BEGIN  LIT_CHAR_UPPERCASE_A /* uppercase letters for
                                                                          * hexadecimal digits range */
#define LIT_CHAR_ASCII_UPPERCASE_LETTERS_HEX_END    LIT_CHAR_UPPERCASE_F

#define LIT_CHAR_ASCII_LOWERCASE_LETTERS_HEX_BEGIN  LIT_CHAR_LOWERCASE_A /* lowercase letters for
                                                                          * hexadecimal digits range */
#define LIT_CHAR_ASCII_LOWERCASE_LETTERS_HEX_END    LIT_CHAR_LOWERCASE_F

#define LIT_CHAR_ASCII_OCTAL_DIGITS_BEGIN           LIT_CHAR_0           /* octal digits range */
#define LIT_CHAR_ASCII_OCTAL_DIGITS_END             LIT_CHAR_7

#define LIT_CHAR_ASCII_DIGITS_BEGIN                 LIT_CHAR_0           /* decimal digits range */
#define LIT_CHAR_ASCII_DIGITS_END                   LIT_CHAR_9

extern bool lit_char_is_octal_digit (ecma_char_t);
extern bool lit_char_is_decimal_digit (ecma_char_t);
extern bool lit_char_is_hex_digit (ecma_char_t);
extern uint32_t lit_char_hex_to_int (ecma_char_t);

/* read a hex encoded code point from a zero terminated buffer */
bool lit_read_code_point_from_hex (const lit_utf8_byte_t *, lit_utf8_size_t, lit_code_point_t *);

/**
 * Null character
 */
#define LIT_CHAR_NULL  ((ecma_char_t) '\0')

/*
 * Part of IsWordChar abstract operation (ECMA-262 v5, 15.10.2.6, step 3)
 */
extern bool lit_char_is_word_char (ecma_char_t);

/*
 * Utility functions for uppercasing / lowercasing
 */

/**
 * Minimum buffer size for lit_char_to_lower_case / lit_char_to_upper_case functions.
 */
#define LIT_MAXIMUM_OTHER_CASE_LENGTH (3)

lit_utf8_size_t lit_char_to_lower_case (ecma_char_t, ecma_char_t *, size_t);
lit_utf8_size_t lit_char_to_upper_case (ecma_char_t, ecma_char_t *, size_t);

#endif /* LIT_CHAR_HELPERS_H */
