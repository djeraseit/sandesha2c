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

#include "echo_util.h"
#include <axis2_util.h>
#include <axiom_soap.h>
#include <axis2_client.h>
#include <axis2_svc_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_op_client.h>
#include <axis2_listener_manager.h>
#include <axis2_callback_recv.h>
#include <axis2_svc_client.h>
#include <sandesha2_client_constants.h>
#include <sandesha2_constants.h>
#include <sandesha2_client.h>
#include <platforms/axutil_platform_auto_sense.h>
#include <ctype.h>

#define SANDESHA2_MAX_COUNT 8

/* on_complete callback function */
axis2_status_t AXIS2_CALL
rm_echo_callback_on_complete(
    struct axis2_callback *callback,
    const axutil_env_t *env);

/* on_error callback function */
axis2_status_t AXIS2_CALL
rm_echo_callback_on_error(
    struct axis2_callback *callback,
    const axutil_env_t *env,
    int exception);

void wait_on_callback(
    const axutil_env_t *env,
    axis2_callback_t *callback);

static void 
usage(
    axis2_char_t *prog_name);

int main(int argc, char** argv)
{
    const axutil_env_t *env = NULL;
    const axis2_char_t *address = NULL;
    const axis2_char_t *to = NULL;
    axis2_endpoint_ref_t* endpoint_ref = NULL;
    axis2_endpoint_ref_t* target_epr = NULL;
    axis2_endpoint_ref_t* reply_to = NULL;
    axis2_options_t *options = NULL;
    const axis2_char_t *client_home = NULL;
    axis2_svc_client_t* svc_client = NULL;
    axiom_node_t *payload = NULL;
    /*axis2_callback_t *callback1 = NULL;
    axis2_callback_t *callback2 = NULL;*/
    axis2_callback_t *callback3 = NULL;
    axutil_property_t *property = NULL;
    axis2_listener_manager_t *listener_manager = NULL;
    axutil_string_t *soap_action = NULL;
    axis2_char_t *seq_key = NULL;
    int c;
   
    /* Set up the environment */
    env = axutil_env_create_all("rm_echo_1_0.log", 
            AXIS2_LOG_LEVEL_TRACE);

    /* Set end point reference of echo service */
    to = "http://127.0.0.1:8888/axis2/services/RMSampleService";
    while ((c = AXIS2_GETOPT(argc, argv, ":a:")) != -1)
    {

        switch (c)
        {
            case 'a':
                address = optarg;
                break;
            case ':':
                fprintf(stderr, "\nOption -%c requires an operand\n", optopt);
                usage(argv[0]);
                return -1;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr, "\nUnknown option `-%c'.\n", optopt);
                usage(argv[0]);
                return -1;
        }
    }
    if (axutil_strcmp(address, "-h") == 0)
    {
        printf("Usage : %s [endpoint_url] [offer]\n", argv[0]);
        printf("use -h for help\n");
        return 0;
    }
    printf ("Using endpoint : %s\n", address);
    
    /* Create EPR with given address */
    if(to)
        endpoint_ref = axis2_endpoint_ref_create(env, to);
    if(address)
        target_epr = axis2_endpoint_ref_create(env, address);

    /* Setup options */
    options = axis2_options_create(env);
    if(endpoint_ref)
        axis2_options_set_to(options, env, endpoint_ref);
    if(target_epr)
    {
        property = axutil_property_create_with_args(env, 0, 0, 0, target_epr);
        axis2_options_set_property(options, env, AXIS2_TARGET_EPR, property);
    }
    axis2_options_set_use_separate_listener(options, env, AXIS2_TRUE);
    
    /* Separate listner needs addressing, hence addressing stuff in options */
    soap_action = axutil_string_create(env, "urn:wsrm:EchoString");
    axis2_options_set_soap_action(options, env, soap_action);
    axis2_options_set_action(options, env, "urn:wsrm:EchoString");
    reply_to = axis2_endpoint_ref_create(env, 
        "http://localhost:7777/axis2/services/__ANONYMOUS_SERVICE__/"\
            "__OPERATION_OUT_IN__");

    axis2_options_set_reply_to(options, env, reply_to);

    /* Set up deploy folder. It is from the deploy folder, the configuration is 
     * picked up using the axis2.xml file.
     * In this sample client_home points to the Axis2/C default deploy folder. 
     * The client_home can be different from this folder on your system. For 
     * example, you may have a different folder (say, my_client_folder) with its 
     * own axis2.xml file. my_client_folder/modules will have the modules that 
     * the client uses
     */
    client_home = AXIS2_GETENV("AXIS2C_HOME");
    if (!client_home)
        client_home = "../../deploy";

    /* Create service client */
    svc_client = axis2_svc_client_create(env, client_home);
    if (!svc_client)
    {
        printf("Error creating service client\n");
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
            " %d :: %s", env->error->error_number,
            AXIS2_ERROR_GET_MESSAGE(env->error));
        return -1;
    }
    axis2_options_set_soap_version(options, env, AXIOM_SOAP11);
    /* RM Version 1.0 */
    property = axutil_property_create_with_args(env, 3, 0, 0, 
        SANDESHA2_SPEC_VERSION_1_0);
    if(property)
    {
        axis2_options_set_property(options, env, 
            SANDESHA2_CLIENT_RM_SPEC_VERSION, property);
    }
    /* Set service client options */
    axis2_svc_client_set_options(svc_client, env, options);    
    
    axis2_svc_client_engage_module(svc_client, env, AXIS2_MODULE_ADDRESSING);  
    axis2_svc_client_engage_module(svc_client, env, "sandesha2");

    listener_manager = axis2_listener_manager_create(env);
    if (!listener_manager)
    {
        return AXIS2_FAILURE;
    }
    seq_key = axutil_uuid_gen(env);
    property = axutil_property_create_with_args(env, 0, 0, 0, seq_key);
    if(property)
    {
        axis2_options_set_property(options, env, SANDESHA2_CLIENT_SEQ_KEY, 
            property);
    }
    /*payload = build_om_payload_for_echo_svc(env, "echo1", seq_key);
    callback1 = axis2_callback_create(env);
    axis2_callback_set_on_complete(callback1, rm_echo_callback_on_complete);
    axis2_callback_set_on_error(callback1, rm_echo_callback_on_error);
    axis2_svc_client_send_receive_NON_BLOCKING(svc_client, env, payload, callback1);
    wait_on_callback(env, callback1);

    payload = build_om_payload_for_echo_svc(env, "echo2", seq_key);
    callback2 = axis2_callback_create(env);
    axis2_callback_set_on_complete(callback2, rm_echo_callback_on_complete);
    axis2_callback_set_on_error(callback2, rm_echo_callback_on_error);
    axis2_svc_client_send_receive_NON_BLOCKING(svc_client, env, payload, callback2);
    wait_on_callback(env, callback2);
    */
    property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
    axis2_options_set_property(options, env, "Sandesha2LastMessage", property);
    payload = build_om_payload_for_echo_svc(env, "echo3", seq_key);
    callback3 = axis2_callback_create(env);
    axis2_callback_set_on_complete(callback3, rm_echo_callback_on_complete);
    axis2_callback_set_on_error(callback3, rm_echo_callback_on_error);
    axis2_svc_client_send_receive_non_blocking(svc_client, env, payload, callback3);
    wait_on_callback(env, callback3);
    AXIS2_SLEEP(SANDESHA2_MAX_COUNT);
    AXIS2_FREE(env->allocator, seq_key);
    if (svc_client)
    {
        /*axis2_svc_client_free(svc_client, env);*/
        svc_client = NULL;
    }
    
    return 0;
}

axis2_status_t AXIS2_CALL
rm_echo_callback_on_complete(
    struct axis2_callback *callback,
    const axutil_env_t *env)
{
   /** SOAP response has arrived here; get the soap envelope 
     from the callback object and do whatever you want to do with it */
   
    axiom_soap_envelope_t *soap_envelope = NULL;
    axiom_node_t *ret_node = NULL;
    axis2_status_t status = AXIS2_SUCCESS;
   
    printf("inside on_complete_callback function\n");
   
    soap_envelope = axis2_callback_get_envelope(callback, env);
   
    if (!soap_envelope)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
        " %d :: %s", env->error->error_number,
        AXIS2_ERROR_GET_MESSAGE(env->error));
        printf("echo stub invoke FAILED!\n");
        status = AXIS2_FAILURE;
    }
    else
    {
        ret_node = axiom_soap_envelope_get_base_node(soap_envelope, env);
    
        if(!ret_node)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "Stub invoke FAILED: Error code:%d :: %s", 
                env->error->error_number, 
                AXIS2_ERROR_GET_MESSAGE(env->error));
            printf("echo stub invoke FAILED!\n");
            status = AXIS2_FAILURE;
        }
        else
        {
            axis2_char_t *om_str = NULL;
            om_str = axiom_node_to_string(ret_node, env);
            if (om_str)
                printf("\nReceived OM : %s\n", om_str);
            printf("\necho client invoke SUCCESSFUL!\n");
        }
    }    
    return status;
}

axis2_status_t AXIS2_CALL
rm_echo_callback_on_error(
    struct axis2_callback *callback,
    const axutil_env_t *env,
    int exception)
{
   /** take necessary action on error */
   printf("\nEcho client invoke FAILED. Error code:%d ::%s", exception, 
         AXIS2_ERROR_GET_MESSAGE(env->error));
   return AXIS2_SUCCESS;
}

void wait_on_callback(
    const axutil_env_t *env,
    axis2_callback_t *callback)
{
    /** Wait till callback is complete. Simply keep the parent thread running
       until our on_complete or on_error is invoked */
    while(1)
    {
        if (axis2_callback_get_complete(callback, env))
        {
            /* We are done with the callback */
            break;
        }
    }
    return;
}

static void 
usage(
    axis2_char_t *prog_name)
{
    fprintf(stdout, "\n Usage : %s", prog_name);
    fprintf(stdout, " [-a ADDRESS]");
    fprintf(stdout, " Options :\n");
    fprintf(stdout, "\t-a ADDRESS \t endpoint address.. The" \
        " default is http://127.0.0.1:8888/axis2/services/RMSampleService \n");
    fprintf(stdout, " Help :\n\t-h \t display this help screen.\n\n");
}


