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
 
#ifndef SANDESHA2_TRANSPORT_SENDER_H
#define SANDESHA2_TRANSPORT_SENDER_H

/**
  * @file sandesha2_transport_sender.h
  * @brief 
  */

#include <axutil_utils_defines.h>
#include <axutil_env.h>
#include <axis2_transport_out_desc.h>
#include <axis2_transport_sender.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_transport_sender
 * @ingroup sandesha2_transport
 * @{
 */

AXIS2_EXTERN axis2_transport_sender_t* AXIS2_CALL
sandesha2_transport_sender_create(
    const axutil_env_t *env);
                        
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_TRANSPORT_SENDER_H */

