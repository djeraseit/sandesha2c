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
 
#include "sandesha2_permanent_sender_mgr.h"
#include "sandesha2_permanent_bean_mgr.h"
#include <sandesha2_sender_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_sender_bean.h>
#include <sandesha2_storage_mgr.h>
#include <axutil_log.h>
#include <axutil_hash.h>
#include <axutil_thread.h>
#include <axutil_property.h>
#include <axutil_types.h>

/** 
 * @brief Sandesha2 Permanent Sender Manager Struct Impl
 *   Sandesha2 Permanent Sender Manager 
 */ 
typedef struct sandesha2_permanent_sender_mgr
{
    sandesha2_sender_mgr_t sender_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
} sandesha2_permanent_sender_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(sender_mgr) \
    ((sandesha2_permanent_sender_mgr_t *) sender_mgr)

static int 
sandesha2_sender_find_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int i = 0;
    sandesha2_sender_bean_t *bean = NULL;
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axutil_env_t *env = args->env;
    axutil_array_list_t *data_list = (axutil_array_list_t *) args->data;
    if(argc < 1)
    {
        args->data = NULL;
        return 0;
    }
    if(!data_list)
    {
        data_list = axutil_array_list_create(env, 0);
        args->data = data_list;
    }
    bean = sandesha2_sender_bean_create(env);
    for(i = 0; i < argc; i++)
    {
        if(0 == axutil_strcmp(col_name[i], "msg_id"))
            sandesha2_sender_bean_set_msg_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "msg_ctx_ref_key"))
            if(argv[i])
                sandesha2_sender_bean_set_msg_ctx_ref_key(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "internal_seq_ID"))
            if(argv[i])
                sandesha2_sender_bean_set_internal_seq_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "sent_count"))
            sandesha2_sender_bean_set_sent_count(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "msg_no"))
            sandesha2_sender_bean_set_msg_no(bean, env, atol(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "send"))
            sandesha2_sender_bean_set_send(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "resend"))
            sandesha2_sender_bean_set_resend(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "time_to_send"))
            sandesha2_sender_bean_set_time_to_send(bean, env, atol(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "msg_type"))
            sandesha2_sender_bean_set_msg_type(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "seq_id"))
            if(argv[i])
                sandesha2_sender_bean_set_seq_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "wsrm_anon_uri"))
            if(argv[i])
                sandesha2_sender_bean_set_wsrm_anon_uri(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "to_address"))
            if(argv[i])
                sandesha2_sender_bean_set_to_address(bean, env, argv[i]);
    }
    axutil_array_list_add(data_list, env, bean);
    return 0;
}

static int 
sandesha2_sender_retrieve_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int i = 0;
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axutil_env_t *env = args->env;
    sandesha2_sender_bean_t *bean = (sandesha2_sender_bean_t *) args->data;
    if(argc < 1)
    {
        args->data = NULL;
        return 0;
    }
    if(!bean)
    {
        bean = sandesha2_sender_bean_create(env);
        args->data = bean;
    }
    for(i = 0; i < argc; i++)
    {
        if(0 == axutil_strcmp(col_name[i], "msg_id"))
            sandesha2_sender_bean_set_msg_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "msg_ctx_ref_key"))
            if(argv[i])
                sandesha2_sender_bean_set_msg_ctx_ref_key(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "internal_seq_ID"))
            if(argv[i])
                sandesha2_sender_bean_set_internal_seq_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "sent_count"))
            sandesha2_sender_bean_set_sent_count(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "msg_no"))
            sandesha2_sender_bean_set_msg_no(bean, env, atol(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "send"))
            sandesha2_sender_bean_set_send(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "resend"))
            sandesha2_sender_bean_set_resend(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "time_to_send"))
            sandesha2_sender_bean_set_time_to_send(bean, env, atol(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "msg_type"))
            sandesha2_sender_bean_set_msg_type(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == axutil_strcmp(col_name[i], "seq_id"))
            if(argv[i])
                sandesha2_sender_bean_set_seq_id(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "wsrm_anon_uri"))
            if(argv[i])
                sandesha2_sender_bean_set_wsrm_anon_uri(bean, env, argv[i]);
        if(0 == axutil_strcmp(col_name[i], "to_address"))
            if(argv[i])
                sandesha2_sender_bean_set_to_address(bean, env, argv[i]);
    }
    return 0;
}

static int 
sandesha2_sender_count_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int *count = (int *) not_used;
    *count = AXIS2_ATOI(argv[0]);
    return 0;
}


void AXIS2_CALL
sandesha2_permanent_sender_mgr_free(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_insert(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_remove(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *msg_id);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_retrieve(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *msg_id);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_update(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean);

axutil_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_internal_seq_id(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id);

axutil_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_sender_bean(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_unique(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_match(
    sandesha2_permanent_bean_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    const axis2_char_t *seq_id);

static const sandesha2_sender_mgr_ops_t sender_mgr_ops = 
{
    sandesha2_permanent_sender_mgr_free,
    sandesha2_permanent_sender_mgr_insert,
    sandesha2_permanent_sender_mgr_remove,
    sandesha2_permanent_sender_mgr_retrieve,
    sandesha2_permanent_sender_mgr_update,
    sandesha2_permanent_sender_mgr_find_by_internal_seq_id,
    sandesha2_permanent_sender_mgr_find_by_sender_bean,
    sandesha2_permanent_sender_mgr_find_unique,
    sandesha2_permanent_sender_mgr_get_next_msg_to_send,
};

AXIS2_EXTERN sandesha2_sender_mgr_t * AXIS2_CALL
sandesha2_permanent_sender_mgr_create(
    const axutil_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_conf_ctx_t *ctx)
{
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_sender_mgr_t));

    sender_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_RETRANSMITTER);
    sender_mgr_impl->bean_mgr->ops.match = sandesha2_permanent_sender_mgr_match;
    sender_mgr_impl->sender_mgr.ops = sender_mgr_ops;
    return &(sender_mgr_impl->sender_mgr);
}

void AXIS2_CALL
sandesha2_permanent_sender_mgr_free(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env)
{
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    if(sender_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(sender_mgr_impl->bean_mgr, env);
        sender_mgr_impl->bean_mgr = NULL;
    }
    if(sender_mgr_impl)
    {
        AXIS2_FREE(env->allocator, sender_mgr_impl);
        sender_mgr_impl = NULL;
    }
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_insert(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t sql_insert[1024];
    axis2_char_t sql_retrieve[256];
    axis2_char_t sql_update[1024];
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;

    axis2_char_t *msg_id = sandesha2_sender_bean_get_msg_id((sandesha2_rm_bean_t *) bean, 
        env);
    axis2_char_t *msg_ctx_ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(bean, env);
    axis2_char_t *internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(bean, env);
    int sent_count = sandesha2_sender_bean_get_sent_count(bean, env);
    long msg_no = sandesha2_sender_bean_get_msg_no(bean, env);
    axis2_bool_t send = sandesha2_sender_bean_is_send(bean, env);
    axis2_bool_t resend = sandesha2_sender_bean_is_resend(bean, env);
    long time_to_send = sandesha2_sender_bean_get_time_to_send(bean, env);
    int msg_type = sandesha2_sender_bean_get_msg_type(bean, env);
    axis2_char_t *seq_id = sandesha2_sender_bean_get_seq_id(bean, env);
    axis2_char_t *wsrm_anon_uri = sandesha2_sender_bean_get_wsrm_anon_uri(bean, env);
    axis2_char_t *to_address = sandesha2_sender_bean_get_to_address(bean, env);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_insert");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    sprintf(sql_retrieve, "select msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send, "\
        "msg_type, seq_id, wsrm_anon_uri, to_address from sender "\
        "where msg_id='%s'", msg_id);

    sprintf(sql_update, "update sender set msg_ctx_ref_key='%s'"\
        ", internal_seq_id='%s', sent_count=%d, msg_no=%ld, send=%d"\
        ", resend=%d, time_to_send=%ld, msg_type=%d, seq_id='%s'"\
        ", wsrm_anon_uri='%s', to_address='%s' where msg_id='%s';",
        msg_ctx_ref_key, internal_seq_id, sent_count, msg_no, send, resend,
        time_to_send, msg_type, seq_id, wsrm_anon_uri, to_address, msg_id);

    sprintf(sql_insert, "insert into sender(msg_id, msg_ctx_ref_key,"\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send,"\
        "msg_type, seq_id, wsrm_anon_uri, to_address) values('%s', '%s', '%s',"\
        "%d, %ld, %d, %d, %ld, %d, '%s', '%s', '%s');", msg_id, msg_ctx_ref_key, 
        internal_seq_id, sent_count, msg_no, send, resend, time_to_send, 
        msg_type, seq_id, wsrm_anon_uri, to_address);

    ret = sandesha2_permanent_bean_mgr_insert(sender_mgr_impl->bean_mgr, env,
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_retrieve_callback, 
        sql_retrieve, sql_update, sql_insert);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_insert:return:%d", ret);
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_remove(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *msg_id)
{
    axis2_char_t sql_retrieve[256];
    axis2_char_t sql_remove[256];
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_remove");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sprintf(sql_remove, "delete from sender where msg_id='%s'", msg_id);
    sprintf(sql_retrieve, "select msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send, "\
        "msg_type, seq_id, wsrm_anon_uri, to_address from sender "\
        "where msg_id='%s'", msg_id);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_remove");
    return sandesha2_permanent_bean_mgr_remove(sender_mgr_impl->bean_mgr, env,
        sandesha2_sender_retrieve_callback, sql_retrieve, sql_remove);
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_retrieve(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *msg_id)
{
    axis2_char_t sql_retrieve[256];
    sandesha2_sender_bean_t *ret = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_retrieve");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    sprintf(sql_retrieve, "select msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send, "\
        "msg_type, seq_id, wsrm_anon_uri, to_address from sender "\
        "where msg_id='%s'", msg_id);
    ret = (sandesha2_sender_bean_t *) sandesha2_permanent_bean_mgr_retrieve(
        sender_mgr_impl->bean_mgr, env, sandesha2_sender_retrieve_callback, 
        sql_retrieve);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_retrieve");
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_update(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t sql_retrieve[256];
    axis2_char_t sql_update[1024];
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;

    axis2_char_t *msg_id = sandesha2_sender_bean_get_msg_id((sandesha2_rm_bean_t *) bean, 
        env);
    axis2_char_t *msg_ctx_ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(bean, env);
    axis2_char_t *internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(bean, env);
    int sent_count = sandesha2_sender_bean_get_sent_count(bean, env);
    long msg_no = sandesha2_sender_bean_get_msg_no(bean, env);
    axis2_bool_t send = sandesha2_sender_bean_is_send(bean, env);
    axis2_bool_t resend = sandesha2_sender_bean_is_resend(bean, env);
    long time_to_send = sandesha2_sender_bean_get_time_to_send(bean, env);
    int msg_type = sandesha2_sender_bean_get_msg_type(bean, env);
    axis2_char_t *seq_id = sandesha2_sender_bean_get_seq_id(bean, env);
    axis2_char_t *wsrm_anon_uri = sandesha2_sender_bean_get_wsrm_anon_uri(bean, env);
    axis2_char_t *to_address = sandesha2_sender_bean_get_to_address(bean, env);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_update");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    sprintf(sql_retrieve, "select msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send, "\
        "msg_type, seq_id, wsrm_anon_uri, to_address from sender "\
        "where msg_id='%s'", msg_id);

    sprintf(sql_update, "update sender set msg_ctx_ref_key='%s'"\
        ", internal_seq_id='%s', sent_count=%d, msg_no=%ld, send=%d"\
        ", resend=%d, time_to_send=%ld, msg_type=%d, seq_id='%s'"\
        ", wsrm_anon_uri='%s', to_address='%s' where msg_id='%s';",
        msg_ctx_ref_key, internal_seq_id, sent_count, msg_no, send, resend,
        time_to_send, msg_type, seq_id, wsrm_anon_uri, to_address, msg_id);

    ret = sandesha2_permanent_bean_mgr_update(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_retrieve_callback, sql_retrieve, sql_update);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_invoker_mgr_update:return:%d", ret);
    return ret;

    return AXIS2_SUCCESS;
}

axutil_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_internal_seq_id(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_sender_bean_t *bean = NULL;
    axutil_array_list_t *ret = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_find_by_internal_seq_id");
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    
    bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(bean, env, 
        internal_seq_id);
    sql_find = "select msg_id, msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;";
    sql_count = "select count(*) as no_recs from sender;";
    ret = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);
    if(bean)
        sandesha2_sender_bean_free((sandesha2_rm_bean_t *) bean, env);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_find_by_internal_seq_id");
    return ret;
}

axutil_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_sender_bean(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    axutil_array_list_t *ret = NULL;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_find_by_sender_bean");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sql_find = "select msg_id, msg_ctx_ref_key, internal_seq_id,"\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;";
    sql_count = "select count(*) as no_recs from sender;";
    ret = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_find_by_sender_bean");
    return ret;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_unique(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sql_find = "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;";
    sql_count = "select count(*) as no_recs from sender;";
    return (sandesha2_sender_bean_t *) sandesha2_permanent_bean_mgr_find(
        sender_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_sender_find_callback, sandesha2_sender_count_callback, 
        sql_find, sql_count);
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_match(
    sandesha2_permanent_bean_mgr_t *sender_mgr,
    const axutil_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    axis2_bool_t add = AXIS2_TRUE;
    axis2_char_t *ref_key = NULL;
    axis2_char_t *temp_ref_key = NULL;
    long time_to_send = 0;
    long temp_time_to_send = 0;
    axis2_char_t *msg_id = NULL;
    axis2_char_t *temp_msg_id = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *temp_internal_seq_id = NULL;
    long msg_no = 0;
    long temp_msg_no = 0;
    int msg_type = 0;
    int temp_msg_type = 0;
    axis2_bool_t is_send = AXIS2_FALSE;
    axis2_bool_t temp_is_send = AXIS2_FALSE;
    
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_permanent_sender_mgr_match");
    ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
        (sandesha2_sender_bean_t *) bean, env);
    temp_ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
        (sandesha2_sender_bean_t *) candidate, env);
    if(ref_key && temp_ref_key && 0 != axutil_strcmp(ref_key, temp_ref_key))
    {
        add = AXIS2_FALSE;
    }
    time_to_send = sandesha2_sender_bean_get_time_to_send(
        (sandesha2_sender_bean_t *) bean, env);
    temp_time_to_send = sandesha2_sender_bean_get_time_to_send(
        (sandesha2_sender_bean_t *) candidate, env);
    /*if(time_to_send > 0 && (time_to_send != temp_time_to_send))*/
    if(time_to_send > 0 && (time_to_send < temp_time_to_send))
    {
        add = AXIS2_FALSE;
    }
    msg_id = sandesha2_sender_bean_get_msg_id(bean, env);
    temp_msg_id = sandesha2_sender_bean_get_msg_id(candidate, env);
    if(msg_id && temp_msg_id && 0 != axutil_strcmp(msg_id, temp_msg_id))
    {
        add = AXIS2_FALSE;
    }
    internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(
        (sandesha2_sender_bean_t *) bean, env);
    temp_internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(
        (sandesha2_sender_bean_t *) candidate, 
            env);
    if(internal_seq_id && temp_internal_seq_id && 0 != axutil_strcmp(
                internal_seq_id, temp_internal_seq_id))
    {
        add = AXIS2_FALSE;
    }
    msg_no = sandesha2_sender_bean_get_msg_no(
        (sandesha2_sender_bean_t *) bean, env);
    temp_msg_no = sandesha2_sender_bean_get_msg_no(
        (sandesha2_sender_bean_t *) candidate, env);
    if(msg_no > 0 && (msg_no != temp_msg_no))
    {
        add = AXIS2_FALSE;
    }
    msg_type = sandesha2_sender_bean_get_msg_type(
        (sandesha2_sender_bean_t *) bean, env);
    temp_msg_type = sandesha2_sender_bean_get_msg_type(
        (sandesha2_sender_bean_t *) candidate, env);
    if(msg_type != SANDESHA2_MSG_TYPE_UNKNOWN  && (msg_type != temp_msg_type))
    {
        add = AXIS2_FALSE;
    }
    is_send = sandesha2_sender_bean_is_send(
        (sandesha2_sender_bean_t *) bean, env);
    temp_is_send = sandesha2_sender_bean_is_send(
        (sandesha2_sender_bean_t *) candidate, env);
    if(is_send != temp_is_send)
    {
        add = AXIS2_FALSE;
    }
    /* Do not use the is_resend flag to match messages, as it can stop us from
     * detecting RM messages during 'get_next_msg_to_send'*/
    /*is_resend = sandesha2_sender_bean_is_resend(
        (sandesha2_sender_bean_t *) bean, env);
    temp_is_resend = sandesha2_sender_bean_is_resend(
        (sandesha2_sender_bean_t *) candidate, env);
    if(is_resend != temp_is_resend)
    {
        add = AXIS2_FALSE;
    }*/
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_permanent_sender_mgr_match:add:%d", 
            add);
    return add;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender_mgr,
    const axutil_env_t *env,
    const axis2_char_t *seq_id)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_sender_bean_t *matcher = sandesha2_sender_bean_create(env);
    long time_now = 0;
    int i = 0, size = 0;
    axutil_array_list_t *match_list = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    sandesha2_sender_bean_t *result = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    sandesha2_sender_bean_set_send(matcher, env, AXIS2_TRUE);
    time_now = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(matcher, env, time_now);
    sandesha2_sender_bean_set_internal_seq_id(matcher, env, 
        (axis2_char_t *) seq_id);
    sql_find = "select msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, "\
        "time_to_send, msg_type, seq_id, wsrm_anon_uri, "\
        "to_address from sender;";
    sql_count = "select count(*) as no_recs from sender;";
    match_list = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) matcher, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);

    /*
     * We either return an application message or an RM message. If we find
     * an application message first then we carry on through the list to be
     * sure that we send the lowest app message avaliable. If we hit a RM
     * message first then we are done.
     */
    if(match_list)
        size = axutil_array_list_size(match_list, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_sender_bean_t *bean = NULL;
        int msg_type = -1;
        bean = (sandesha2_sender_bean_t *) axutil_array_list_get(match_list, 
            env, i);
        msg_type = sandesha2_sender_bean_get_msg_type(bean, env);
        if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
        {
            long msg_no = sandesha2_sender_bean_get_msg_no(bean, env);
            long result_msg_no = -1;
            if(result)
                result_msg_no = sandesha2_sender_bean_get_msg_no(result, env);
            if(result == NULL || result_msg_no > msg_no)
                result = bean;
        }
        else if(!result)
        {
            result = bean;
            break;
        }
    }
    return result;
}

