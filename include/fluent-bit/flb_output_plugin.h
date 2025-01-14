/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2022 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_OUTPUT_PLUGIN_H
#define FLB_OUTPUT_PLUGIN_H

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_output.h>
#include <fluent-bit/flb_log.h>
#include <cmetrics/cmetrics.h>
#include <cmetrics/cmt_encode_text.h>
#include <cmetrics/cmt_encode_prometheus.h>

#include <stdarg.h>

static inline int flb_output_plugin_log_suppress_check(struct flb_output_instance *ins, const char *fmt, ...)
{
    int ret;
    size_t size;
    va_list args;
    char buf[4096];
    struct flb_worker *w;

    if (ins->log_suppress_interval <= 0) {
        return FLB_FALSE;
    }

    va_start(args, fmt);
    size = vsnprintf(buf, sizeof(buf) - 1, fmt, args);
    va_end(args);

    if (size == -1) {
        return FLB_FALSE;
    }

    w = flb_worker_get();
    if (!w) {
        return FLB_FALSE;
    }

    ret = flb_log_cache_check_suppress(w->log_cache, buf, size);
    return ret;
}

#define flb_plg_error(ctx, fmt, ...)                                    \
    if (flb_log_check_level(ctx->log_level, FLB_LOG_ERROR))             \
        if (flb_output_plugin_log_suppress_check(ctx, fmt, ##__VA_ARGS__) == FLB_FALSE) \
            flb_log_print(FLB_LOG_ERROR, NULL, 0, "[output:%s:%s] " fmt,     \
                          ctx->p->name, flb_output_name(ctx), ##__VA_ARGS__)

#define flb_plg_warn(ctx, fmt, ...)                                     \
    if (flb_log_check_level(ctx->log_level, FLB_LOG_WARN))              \
        if (flb_output_plugin_log_suppress_check(ctx, fmt, ##__VA_ARGS__) == FLB_FALSE) \
            flb_log_print(FLB_LOG_WARN, NULL, 0, "[output:%s:%s] " fmt,     \
                          ctx->p->name, flb_output_name(ctx), ##__VA_ARGS__)

#define flb_plg_info(ctx, fmt, ...)                                     \
    if (flb_log_check_level(ctx->log_level, FLB_LOG_INFO))              \
        if (flb_output_plugin_log_suppress_check(ctx, fmt, ##__VA_ARGS__) == FLB_FALSE) \
            flb_log_print(FLB_LOG_INFO, NULL, 0, "[output:%s:%s] " fmt,     \
                          ctx->p->name, flb_output_name(ctx), ##__VA_ARGS__)

#define flb_plg_debug(ctx, fmt, ...)                                    \
    if (flb_log_check_level(ctx->log_level, FLB_LOG_DEBUG))             \
        if (flb_output_plugin_log_suppress_check(ctx, fmt, ##__VA_ARGS__) == FLB_FALSE) \
            flb_log_print(FLB_LOG_DEBUG, NULL, 0, "[output:%s:%s] " fmt,    \
                          ctx->p->name, flb_output_name(ctx), ##__VA_ARGS__)

#define flb_plg_trace(ctx, fmt, ...)                                    \
    if (flb_log_check_level(ctx->log_level, FLB_LOG_TRACE))             \
        if (flb_output_plugin_log_suppress_check(ctx, fmt, ##__VA_ARGS__) == FLB_FALSE) \
            flb_log_print(FLB_LOG_TRACE, NULL, 0,                           \
                          "[output:%s:%s at %s:%i] " fmt,                   \
                          ctx->p->name, flb_output_name(ctx), __FLB_FILENAME__, \
                          __LINE__, ##__VA_ARGS__)
#endif
