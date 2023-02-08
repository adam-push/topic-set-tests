#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "diffusion.h"

#include "test_add_and_set.h"


void usage(char *executable_name)
{
    printf("%s <url> <iterations> <test_number> [<total_unique_topic_values> [<topic_value_size> [<message_queue_size> [<total_topics>]]]]\n", executable_name);
    printf("Available test numbers:\n");
    printf("    2 - Add and set all topics\n");
    printf("\n");
}


long get_required_parameter_long(
    int position,
    char *parameter_name,
    char *command_line_parameters[])
{
    long result = strtol(command_line_parameters[position], (char **)NULL, 10);
    if (errno == EINVAL) {
        printf("Invalid %s: [%s]\n", parameter_name, command_line_parameters[position]);
        exit(2);
    }
    return result;
}


long get_optional_parameter_long(
    int position,
    char *parameter_name,
    long default_value,
    int command_line_parameters_length,
    char *command_line_parameters[])
{
    if (position >= command_line_parameters_length) {
        printf("Using default value for %s (%ld).\n", parameter_name, default_value);
        return default_value;
    }
    long result = strtol(command_line_parameters[position], (char **)NULL, 10);
    if (errno == EINVAL) {
        printf("Invalid value for [%s] (%s).\n", parameter_name, command_line_parameters[position]);
        printf("Using default value for %s (%ld).\n", parameter_name, default_value);
        return default_value;
    }
    return result;
}


unsigned char *generate_random_binary_data(long size) {
    unsigned char *result = calloc(size, sizeof(char));
    for (long i = 0; i < size; i++) {
        result[i] = rand();
    }
    return result;
}


int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Not enough arguments for minimal execution.\n");
        usage(argv[0]);
        exit(1);
    }

    // required parameters
    const char *url = argv[1];
    const long iterations = get_required_parameter_long(2, "iterations", argv);
    const long test_number = get_required_parameter_long(3, "test_number", argv);

    // optional parameters
    const long total_unique_topic_values = get_optional_parameter_long(4, "total_unique_topic_values", 1, argc, argv);
    const long topic_value_size = get_optional_parameter_long(5, "topic_value_size", 250, argc, argv);
    const long message_queue_size = get_optional_parameter_long(6, "message_queue_size", 10000, argc, argv);
    const long total_topics = get_optional_parameter_long(7, "total_topics", 100, argc, argv);

    // Print parameters
    printf("Parameters used in test:\n");
    printf("- url: %s\n", url);
    printf("- iterations: %ld\n", iterations);
    printf("- test number: %ld\n", test_number);
    printf("- total unique topic values: %ld\n", total_unique_topic_values);
    printf("- topic value size: %ld\n", topic_value_size);
    printf("- message queue size: %ld\n", message_queue_size);
    printf("- total topics: %ld\n", total_topics);

    //
    char *root_topic_path = "test/set/c";

    // generate random topic values
    srand(12345); // define a seed for reproducibility
    LIST_T *topic_values = list_create();
    unsigned char **topic_data = calloc(total_unique_topic_values, sizeof(char *));
    for (long i = 0; i < total_unique_topic_values; i++) {
        unsigned char *random_value = generate_random_binary_data(topic_value_size);
        BUF_T *value_buf = buf_create();
        write_diffusion_binary_value(random_value, value_buf, topic_value_size);
        list_append_last(topic_values, value_buf);
    }

    // create session
    const char *principal = "admin";
    CREDENTIALS_T *credentials = credentials_create_password("password");

    DIFFUSION_SESSION_FACTORY_T *session_factory = diffusion_session_factory_init();
    diffusion_session_factory_principal(session_factory, principal);
    diffusion_session_factory_credentials(session_factory, credentials);
    diffusion_session_factory_maximum_queue_size(session_factory, message_queue_size);

    SESSION_T *session = session_create_with_session_factory(session_factory, url);
    if (session == NULL) {
        printf("Error while establishing session.\n");
        exit(3);
    }

    char *sid_str = session_id_to_string(session->id);
    printf("Session created (state=%d, id=%s)\n",
    session_state_get(session), sid_str);
    free(sid_str);

    long time_taken_ms = 0;

    switch(test_number) {
        case 2:
        {
            time_taken_ms = test_add_and_set(
                session,
                root_topic_path,
                iterations,
                total_topics,
                total_unique_topic_values,
                topic_values
            );
            break;
        }
        default:
        {
            printf("Test number %ld has not been implemented.", test_number);
            break;
        }
    }

    if (time_taken_ms < 0) {
        printf("Test was aborted or encountered an error.\n");
    }
    else {
        double updates_per_second = (double) (iterations * 1000) / (double) time_taken_ms;
        long rounded_update_rate = lround(updates_per_second);
        printf("Test took %ld ms.\n", time_taken_ms);
        printf("Average Update Rate = %ld updates/s\n", rounded_update_rate);
    }

    // cleanup
    session_close(session, NULL);
    session_free(session);

    diffusion_session_factory_free(session_factory);
    credentials_free(credentials);
    list_free(topic_values, (void (*) (void *)) buf_free);

    printf("Done\n");
    exit(0);
}