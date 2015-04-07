// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef PARSER_MACROS_H_
#define PARSER_MACROS_H_

#define DISALLOW_COPY_AND_ASSIGN(CLASS)            \
  CLASS(const CLASS&) = delete;                    \
  CLASS& operator=(const CLASS&) = delete          \

#endif  // PARSER_MACROS_H_
