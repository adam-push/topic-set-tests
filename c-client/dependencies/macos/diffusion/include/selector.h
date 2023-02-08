#ifndef _diffusion_selector_h_
#define _diffusion_selector_h_ 1

/*
 * Copyright © 2018 - 2022 Push Technology Ltd., All Rights Reserved.
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
 * @file selector.h
 *
 * Function to evaluate a selector against a topic path.
 */
#include "misc/deprecate.h"

/**
 * @brief Evaluate a selector against a topic path.
 *
 * @param selector   selector expression to be evaluated against.
 * @param topic_name topic path to be evaluated.
 *
 * @return 1, if the selector matches against the topic. 0, otherwise.
 *
 * @deprecated  since 6.7 and will be removed in a future release.
 *              Please use {@link diffusion_topic_selector_create} to create a topic selector
 *              and {@link diffusion_topic_selector_selects} to emulate this behaviour.
 */
DEPRECATED(int selector_match(const char *selector, const char *topic_name));

#endif
