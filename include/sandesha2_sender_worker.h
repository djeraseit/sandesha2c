/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SANDESHA2_SENDER_WORKER_H
#define SANDESHA2_SENDER_WORKER_H

/**
 * @file sandesha2_sender_worker.h
 * @brief Sandesha Sender Interface
 */

#include <axutil_allocator.h>
#include <axutil_env.h>
#include <axutil_error.h>
#include <axutil_string.h>
#include <axutil_utils.h>
#include <axis2_conf_ctx.h>
#include <sandesha2_storage_mgr.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct sandesha2_seq_property_mgr;
struct sandesha2_create_seq_mgr;
struct sandesha2_sender_mgr;
typedef struct sandesha2_sender_worker_t sandesha2_sender_worker_t;

AXIS2_EXTERN sandesha2_sender_worker_t* AXIS2_CALL
sandesha2_sender_worker_create(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id);

AXIS2_EXTERN sandesha2_sender_worker_t* AXIS2_CALL
sandesha2_sender_worker_create_with_msg_ctx(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_msg_ctx_t *msg_ctx);
 
/**
 * Frees the sender_worker given as a void pointer. This method would cast the 
 * void parameter to an sender_worker pointer and then call free method.
 * @param sender_worker pointer to sender_worker as a void pointer
 * @param env pointer to environment struct
 * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_sender_worker_free_void_arg(
    void *sender_worker,
    const axutil_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_free(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env);

axis2_status_t
sandesha2_sender_worker_send(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_bool_t persistent_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    struct sandesha2_seq_property_mgr *seq_prop_mgr,
    struct sandesha2_create_seq_mgr *create_seq_mgr,
    struct sandesha2_sender_mgr *sender_mgr);

void sandesha2_sender_worker_set_transport_out(
    sandesha2_sender_worker_t *sender_worker,
    const axutil_env_t *env,
    axis2_transport_out_desc_t *transport_out);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SENDER_WORKER_H */