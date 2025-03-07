// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2013 Oracle and/or its affiliates. All Rights Reserved.
 */

#ifndef LAPI_UTIME_H__
#define LAPI_UTIME_H__

#ifndef UTIME_NOW
# define UTIME_NOW ((1l << 30) - 1l)
#endif

#ifndef UTIME_OMIT
# define UTIME_OMIT ((1l << 30) - 2l)
#endif

#endif /* LAPI_UTIME_H__ */