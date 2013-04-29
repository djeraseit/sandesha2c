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

#ifndef SANDESHA2_POLLING_MGR_H
#define SANDESHA2_POLLING_MGR_H

/**
 * @file sandesha2_polling_mgr.h
 * @brief Sandesha Polling Manager Interface
 * This class is responsible for sending MakeConnection requests. This is a 
 * separate thread that keeps running. Will do MakeConnection based on the 
 * request queue or randomly.
 */

#include <axutil_allocator.h>
#include <axutil_env.h>
#include <axutil_error.h>
#include <axutil_string.h>
#include <axutil_utils.h>
#include <axis2_conf_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_polling_mgr_t sandesha2_polling_mgr_t;

AXIS2_EXTERN sandesha2_polling_mgr_t * AXIS2_CALL
sandesha2_polling_mgr_create(
    const axutil_env_t *env);
 
/**
 * Frees the polling_mgr given as a void pointer. This method would cast the 
 * void parameter to an polling_mgr pointer and then call free method.
 * @param polling_mgr pointer to polling_mgr as a void pointer
 * @param env pointer to environment struct
 * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_polling_mgr_free_void_arg(
    void *polling_mgr,
    const axutil_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_free(
    sandesha2_polling_mgr_t *polling_mgr, 
    const axutil_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_stop_polling (
    sandesha2_polling_mgr_t *polling_mgr,
    const axutil_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_start (
    sandesha2_polling_mgr_t *polling_mgr, 
    const axutil_env_t *env, 
    axis2_conf_ctx_t *conf_ctx,
    const axis2_char_t *internal_seq_id);
            
void AXIS2_CALL
sandesha2_polling_mgr_set_poll(
    sandesha2_polling_mgr_t *polling_mgr,
    const axutil_env_t *env,
    axis2_bool_t poll);

axis2_bool_t AXIS2_CALL
sandesha2_polling_mgr_is_poll(
    sandesha2_polling_mgr_t *polling_mgr,
    const axutil_env_t *env);

void AXIS2_CALL
sandesha2_polling_mgr_schedule_polling_request(
    sandesha2_polling_mgr_t *polling_mgr,
    const axutil_env_t *env,
    const axis2_char_t *internal_seq_id);
                   
/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_POLLING_MGR_H */
