#ifndef _diffusion_h_
#define _diffusion_h_ 1

/*
 * Copyright © 2014 - 2022 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

/**
 * @file diffusion.h
 *
 * Main include file.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "doxygen-defs.h"

#ifndef APR_DECLARE_STATIC
#define APR_DECLARE_STATIC 1
#endif
#ifndef APU_DECLARE_STATIC
#define APU_DECLARE_STATIC 1
#endif

/** The version of Diffusion from which the C API library was built. */
#if defined(__GNUC__)
static const char *DIFFUSION_VERSION __attribute__ ((unused))= "6.9.0#ab21e2b2";
#else
static const char *DIFFUSION_VERSION = "6.9.0#ab21e2b2";
#endif

#include <stdlib.h>
#include <apr.h>

#include "registration.h"

#include "features/authentication-control.h"
#include "features/client-control.h"
#include "features/messaging.h"
#include "features/metrics.h"
#include "features/pings.h"
#include "features/remote-servers.h"
#include "features/security.h"
#include "features/security-control.h"
#include "features/session-trees.h"
#include "features/subscription-control.h"
#include "features/system-authentication-control.h"
#include "features/time-series.h"
#include "features/topic-control.h"
#include "features/topic-notifications.h"
#include "features/topic-update.h"
#include "features/topic-views.h"
#include "features/topics.h"


#include "hash.h"
#include "hash_num.h"
#include "error.h"
#include "diffusion-api-error.h"

#include "conversation.h"
#include "session.h"
#include "topic.h"
#include "security.h"
#include "datatypes.h"
#include "cbor.h"
#include "streams.h"
#include "session-lock.h"
#include "selector.h"
#include "topic-selector.h"
#include "value-stream.h"
#include "topic-specification.h"
#include "update-constraint.h"

#ifdef __cplusplus
}
#endif

#endif
