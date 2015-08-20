// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_CLIST_HELPERS_H_
#define COMMON_UTILS_CLIST_HELPERS_H_

#include "pkgmgr/pkgmgr_parser.h"

/* NOTE: For *_x list types in pkgmgr-info, like privileges_x or privilege_x,
 * this macro moves the current node to the head of the list.
 * This LISTHEAD() macro is defined in pkgmgr_parser.h in pkgmgr-info package.
 */
#define PKGMGR_LIST_MOVE_NODE_TO_HEAD(list, node)                              \
  do {                                                                         \
    if (list) {                                                                \
      LISTHEAD(list, node);                                                    \
    }                                                                          \
  } while (false)                                                              \

/*
 * Calculates size of C style list from any of its point
 */
#define PKGMGR_LIST_LEN(list)                                                  \
  [list]() {                                                                   \
    size_t size = 0;                                                           \
    auto node = list;                                                          \
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(list, node);                                 \
    while (node) {                                                             \
      node = node->next;                                                       \
      ++size;                                                                  \
    }                                                                          \
    return size;                                                               \
  }()                                                                          \

#endif  // COMMON_UTILS_CLIST_HELPERS_H_
