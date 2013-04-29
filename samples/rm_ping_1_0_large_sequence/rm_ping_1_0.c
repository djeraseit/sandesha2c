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

#include <stdio.h>
#include <axiom.h>
#include <axis2_util.h>
#include <axiom_soap.h>
#include <axis2_client.h>
#include <sandesha2_client_constants.h>
#include <sandesha2_constants.h>
#include <ctype.h>

#define SANDESHA2_SLEEP 8

axiom_node_t *
build_om_programatically(
    const axutil_env_t *env,
    axis2_char_t *text,
    axis2_char_t *seq_key);

static void 
usage(
    axis2_char_t *prog_name);

int main(int argc, char** argv)
{
	int optopt;
	char *optarg;
    const axutil_env_t *env = NULL;
    const axis2_char_t *address = NULL;
    axis2_endpoint_ref_t* endpoint_ref = NULL;
    axis2_options_t *options = NULL;
    const axis2_char_t *client_home = NULL;
    axis2_svc_client_t* svc_client = NULL;
    axiom_node_t *payload = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axutil_property_t *property = NULL;
    int c;
    int i = 0;
    axis2_char_t *seq_key = NULL;
   
    /* Set up the environment */
    env = axutil_env_create_all("rm_ping.log", AXIS2_LOG_LEVEL_TRACE);

    /* Set end point reference of echo service */
    address = "http://127.0.0.1:9090/axis2/services/RMSampleService";
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

    printf ("Using endpoint : %s\n", address);
    
    /* Create EPR with given address */
    endpoint_ref = axis2_endpoint_ref_create(env, address);

    /* Setup options */
    options = axis2_options_create(env);
    axis2_options_set_xml_parser_reset(options, env, AXIS2_FALSE);
    if(endpoint_ref)
        axis2_options_set_to(options, env, endpoint_ref);
    /*axis2_options_set_action(options, env, "urn:wsrm:Ping");*/

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
    }

    /* Set service client options */
    axis2_svc_client_set_options(svc_client, env, options);    
    
    /* Engage addressing module */
    axis2_svc_client_engage_module(svc_client, env, AXIS2_MODULE_ADDRESSING);
    
    /* Build the SOAP request message payload using OM API.*/
    axis2_svc_client_engage_module(svc_client, env, "sandesha2");
    /* RM Version 1.0 */
    property = axutil_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_SPEC_VERSION_1_0);
    if(property)
    {
        axis2_options_set_property(options, env, 
            SANDESHA2_CLIENT_RM_SPEC_VERSION, property);
    }

    seq_key = axutil_uuid_gen(env);
    property = axutil_property_create_with_args(env, 0, 0, 0, seq_key);
    if(property)
    {
        axis2_options_set_property(options, env, SANDESHA2_CLIENT_SEQ_KEY,
            property);
    }
    
    /* Send request */
    for(i = 1; i < 100; i++)
    {
        axis2_char_t ping_str[7];
        sprintf(ping_str, "%s%d", "ping", i);
        payload = build_om_programatically(env, ping_str, seq_key);
        status = axis2_svc_client_send_robust(svc_client, env, payload);
        if(status)
            printf("\nping client invoke SUCCESSFUL!\n");
        payload = NULL;
    }
    property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
    axis2_options_set_property(options, env, "Sandesha2LastMessage", 
        property);
    payload = build_om_programatically(env, "ping100", seq_key);
    status = axis2_svc_client_send_robust(svc_client, env, payload);
    if(status)
        printf("\nping client invoke SUCCESSFUL!\n");
    sleep(64); 
     /** Wait till callback is complete. Simply keep the parent thread running
       until our on_complete or on_error is invoked */

    /*This sleep is for wait the main thread until sandesha sends the terminatesequence 
     *messages. */

    AXIS2_SLEEP(SANDESHA2_SLEEP);
   
    if (svc_client)
    {
        axis2_svc_client_free(svc_client, env);
        svc_client = NULL;
    }
    return 0;
}

/* build SOAP request message content using OM */
axiom_node_t *
build_om_programatically(
    const axutil_env_t *env,
    axis2_char_t *text,
    axis2_char_t *seq)
{
    axiom_node_t *ping_om_node = NULL;
    axiom_element_t* ping_om_ele = NULL;
    axiom_node_t *text_om_node = NULL;
    axiom_element_t* text_om_ele = NULL;
    axiom_namespace_t *ns1 = NULL;
    axis2_char_t *buffer = NULL;
    axiom_node_t* seq_om_node = NULL;
    axiom_element_t * seq_om_ele = NULL;

    ns1 = axiom_namespace_create (env, "http://tempuri.org/", "ns1");
    ping_om_ele = axiom_element_create(env, NULL, "ping", ns1, &ping_om_node);
    text_om_ele = axiom_element_create(env, ping_om_node, "Text", ns1, &text_om_node);
    seq_om_ele = axiom_element_create(env, ping_om_node, "Sequence", ns1, &seq_om_node);
    axiom_element_set_text(text_om_ele, env, text, text_om_node);
    axiom_element_set_text(text_om_ele, env, seq, seq_om_node);
    
    buffer = axiom_node_to_string(ping_om_node, env);
    if(buffer)
    {
        printf("\nSending OM node in XML : %s \n",  buffer);
        AXIS2_FREE(env->allocator, buffer);
    }
    return ping_om_node;
}

static void 
usage(
    axis2_char_t *prog_name)
{
    fprintf(stdout, "\n Usage : %s", prog_name);
    fprintf(stdout, " [-a ADDRESS]");
    fprintf(stdout, " Options :\n");
    fprintf(stdout, "\t-a ADDRESS \t endpoint address.. The"
            " default is http://127.0.0.1:9090/axis2/services/RMSampleService ../\n");
    fprintf(stdout, " Help :\n\t-h \t display this help screen.\n\n");
}

