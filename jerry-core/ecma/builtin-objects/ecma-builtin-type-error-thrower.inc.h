/* Copyright 2014 Samsung Electronics Co., Ltd.
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

/*
 * [[ThrowTypeError]] description
 *
 * See also: ECMA-262 v5, 13.2.3
 */

#ifndef OBJECT_ID
# define OBJECT_ID(builtin_object_id)
#endif /* !OBJECT_ID */

#ifndef NUMBER_VALUE
# define NUMBER_VALUE(name, number_value, prop_writable, prop_enumerable, prop_configurable)
#endif /* !NUMBER_VALUE */

/* Object identifier */
OBJECT_ID (ECMA_BUILTIN_ID_TYPE_ERROR_THROWER)

/* Number properties:
 *  (property name, number value, writable, enumerable, configurable) */

NUMBER_VALUE (LIT_MAGIC_STRING_LENGTH,
              0,
              ECMA_PROPERTY_NOT_WRITABLE,
              ECMA_PROPERTY_NOT_ENUMERABLE,
              ECMA_PROPERTY_NOT_CONFIGURABLE)

#undef OBJECT_ID
#undef SIMPLE_VALUE
#undef NUMBER_VALUE
#undef STRING_VALUE
#undef OBJECT_VALUE
#undef CP_UNIMPLEMENTED_VALUE
#undef ROUTINE
