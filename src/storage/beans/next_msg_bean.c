/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sandesha2_next_msg_bean.h>
#include <sandesha2_rm_bean.h>
#include <sandesha2_transaction.h>
#include <string.h>
#include <axutil_string.h>
#include <axutil_utils.h>

/* next_msg_bean struct */
typedef struct sandesha2_next_msg_bean_impl
{
    sandesha2_next_msg_bean_t next_msg_bean;
    sandesha2_rm_bean_t *rm_bean_impl;
	axis2_char_t *seq_id;
	axis2_char_t *internal_seq_id;
    axis2_char_t *ref_msg_key;
    axis2_bool_t polling_mode;
	long msg_no;
} sandesha2_next_msg_bean_impl_t;

#define SANDESHA2_INTF_TO_IMPL(next_msg_bean) \
    ((sandesha2_next_msg_bean_impl_t *) next_msg_bean)

static const sandesha2_rm_bean_ops_t rm_bean_ops =
{
    sandesha2_next_msg_bean_free,
    sandesha2_next_msg_bean_get_base,
    sandesha2_next_msg_bean_set_id,
    sandesha2_next_msg_bean_get_id,
    sandesha2_next_msg_bean_set_transaction,
    sandesha2_next_msg_bean_get_transaction,
    sandesha2_next_msg_bean_get_seq_id
};

AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
sandesha2_next_msg_bean_create(
    const axutil_env_t *env)
{
	sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
    next_msg_bean_impl = (sandesha2_next_msg_bean_impl_t *)AXIS2_MALLOC(
        env->allocator, sizeof(sandesha2_next_msg_bean_impl_t));
	if(!next_msg_bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}
	/* init the properties. */
	next_msg_bean_impl->seq_id = NULL;
	next_msg_bean_impl->internal_seq_id = NULL;
    next_msg_bean_impl->ref_msg_key = NULL;
	next_msg_bean_impl->msg_no = -1;
    next_msg_bean_impl->polling_mode = AXIS2_FALSE;
    next_msg_bean_impl->rm_bean_impl = sandesha2_rm_bean_create(env);
    next_msg_bean_impl->next_msg_bean.rm_bean.ops = rm_bean_ops;
	return &(next_msg_bean_impl->next_msg_bean);
}

AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
sandesha2_next_msg_bean_create_with_data(
    const axutil_env_t *env,
	axis2_char_t *seq_id,
	long msg_no)
{
	sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	next_msg_bean_impl = (sandesha2_next_msg_bean_impl_t *)AXIS2_MALLOC(
        env->allocator, sizeof(sandesha2_next_msg_bean_impl_t));
	if(!next_msg_bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}
	/* init the properties. */
	next_msg_bean_impl->seq_id = (axis2_char_t*)axutil_strdup(env, seq_id);
	next_msg_bean_impl->internal_seq_id = NULL;
    next_msg_bean_impl->ref_msg_key = NULL;
	next_msg_bean_impl->msg_no = msg_no;
    next_msg_bean_impl->polling_mode = AXIS2_FALSE;
    next_msg_bean_impl->rm_bean_impl = sandesha2_rm_bean_create(env);
    next_msg_bean_impl->next_msg_bean.rm_bean.ops = rm_bean_ops;
	return &(next_msg_bean_impl->next_msg_bean);
}

void AXIS2_CALL
sandesha2_next_msg_bean_free (
    sandesha2_rm_bean_t *next_msg_bean,
	const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	if(next_msg_bean_impl->rm_bean_impl)
	{
		sandesha2_rm_bean_free(next_msg_bean_impl->rm_bean_impl, env);
		next_msg_bean_impl->rm_bean_impl= NULL;
	}
	if(next_msg_bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean_impl->seq_id);
		next_msg_bean_impl->seq_id= NULL;
	}
	if(next_msg_bean_impl->internal_seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean_impl->internal_seq_id);
		next_msg_bean_impl->internal_seq_id= NULL;
	}
	if(next_msg_bean_impl->ref_msg_key)
	{
		AXIS2_FREE(env->allocator, next_msg_bean_impl->ref_msg_key);
		next_msg_bean_impl->ref_msg_key= NULL;
	}
    if(next_msg_bean_impl)
    {
        AXIS2_FREE(env->allocator, next_msg_bean_impl);
        next_msg_bean_impl = NULL;
    }
}

sandesha2_rm_bean_t * AXIS2_CALL
sandesha2_next_msg_bean_get_base( 
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	return next_msg_bean_impl->rm_bean_impl;
}	

void AXIS2_CALL
sandesha2_next_msg_bean_set_base (
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env, 
    sandesha2_rm_bean_t* rm_bean)

{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	next_msg_bean_impl->rm_bean_impl = rm_bean;
}

axis2_char_t* AXIS2_CALL
sandesha2_next_msg_bean_get_seq_id(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	return next_msg_bean_impl->seq_id;
}


void AXIS2_CALL
sandesha2_next_msg_bean_set_seq_id(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env, 
    axis2_char_t *seq_id)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	if(next_msg_bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean_impl->seq_id);
		next_msg_bean_impl->seq_id = NULL;
	}

	next_msg_bean_impl->seq_id = (axis2_char_t*) axutil_strdup(env, seq_id); 
}

axis2_char_t* AXIS2_CALL
sandesha2_next_msg_bean_get_internal_seq_id(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	return next_msg_bean_impl->internal_seq_id;
}


void AXIS2_CALL
sandesha2_next_msg_bean_set_internal_seq_id(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env, 
    axis2_char_t *internal_seq_id)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	if(next_msg_bean_impl->internal_seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean_impl->internal_seq_id);
		next_msg_bean_impl->internal_seq_id = NULL;
	}

	next_msg_bean_impl->internal_seq_id = (axis2_char_t*) axutil_strdup(env, 
        internal_seq_id); 
}

long AXIS2_CALL
sandesha2_next_msg_bean_get_next_msg_no_to_process(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	return next_msg_bean_impl->msg_no;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_next_msg_no_to_process(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env, 
    long next_msg_no)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
	next_msg_bean_impl->msg_no = next_msg_no;
}

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_bean_is_polling_mode(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env) 
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    return next_msg_bean_impl->polling_mode;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_polling_mode(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env,
    axis2_bool_t polling_mode) 
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    next_msg_bean_impl->polling_mode = polling_mode;
}

axis2_char_t *AXIS2_CALL
sandesha2_next_msg_bean_get_ref_msg_key(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env) 
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    return next_msg_bean_impl->ref_msg_key;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_ref_msg_key(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axutil_env_t *env,
    axis2_char_t *ref_msg_key) 
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    next_msg_bean_impl->ref_msg_key = axutil_strdup(env, ref_msg_key);
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_id(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env,
    long id)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    sandesha2_rm_bean_set_id(next_msg_bean_impl->rm_bean_impl, env, id);
}

long AXIS2_CALL
sandesha2_next_msg_bean_get_id(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    return sandesha2_rm_bean_get_id(next_msg_bean_impl->rm_bean_impl, env);
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_transaction(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env,
    sandesha2_transaction_t *transaction)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    sandesha2_rm_bean_set_transaction(next_msg_bean_impl->rm_bean_impl, env,
        transaction);
}

sandesha2_transaction_t *AXIS2_CALL
sandesha2_next_msg_bean_get_transaction(
    sandesha2_rm_bean_t *next_msg_bean,
    const axutil_env_t *env)
{
    sandesha2_next_msg_bean_impl_t *next_msg_bean_impl = NULL;
    next_msg_bean_impl = SANDESHA2_INTF_TO_IMPL(next_msg_bean);
    return sandesha2_rm_bean_get_transaction(next_msg_bean_impl->rm_bean_impl,
        env);
}

