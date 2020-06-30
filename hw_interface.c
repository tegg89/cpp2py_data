// For printf()
#include <stdio.h>
// For EXIT_*
#include <stdlib.h>
// For memcpy()
#include <string.h>
// For sleep()
#include <unistd.h>

#include <zmq.h>

#include "libfancyhw.h"

int main(void)
{
    // define the significant parameters
    const unsigned int INIT_PARAM = 12345;
    const unsigned int REPETITIONS = 10;
    const unsigned int PACKET_SIZE = 16;
    const char *TOPIC = "fancyhw_data";
    
    // initialize random number
    fancyhw_init(INIT_PARAM);

    // define an object that manages all the sockets
    void *context = zmq_ctx_new();

    if (!context)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    // define the socket used to deliver data
    // here, we use a "publish (PUB)" socket
    // it delivers copies of a message to multiple receivers
    void *data_socket = zmq_socket(context, ZMQ_PUB);

    const int rb = zmq_bind(data_socket, "tcp://*:5555");

    if (rb != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    // TOPICs are used by the receivers to filter messages
    const size_t topic_size = strlen(TOPIC);
    const size_t envelope_size = topic_size + 1 + PACKET_SIZE * sizeof(int16_t);

    printf("Topic: %s; topic size: %zu; Envelope size: %zu\n", TOPIC, topic_size, envelope_size);

    // sending messages by repetitions
    for (unsigned int i = 0; i < REPETITIONS; i++)
    {
        // before sending a message, fill a buffer
        int16_t buffer[PACKET_SIZE];

        for (unsigned int j = 0; j < PACKET_SIZE; j++)
        {
            buffer[j] = fancyhw_read_val();
        }

        printf("Read %u data values\n", PACKET_SIZE);
      
        // create a ZeroMQ message
        zmq_msg_t envelope;
        
        // allocate the memory necessary for the message
        const int rmi = zmq_msg_init_size(&envelope, envelope_size);
        if (rmi != 0)
        {
            printf("ERROR: ZeroMQ error occurred during zmq_msg_init_size(): %s\n", zmq_strerror(errno));
   
            zmq_msg_close(&envelope);
   
            break;
        }
       
        // store the data in the ZeroMQ message "envelope"
        memcpy(zmq_msg_data(&envelope), TOPIC, topic_size);
        memcpy((void*)((char*)zmq_msg_data(&envelope) + topic_size), " ", 1);
        memcpy((void*)((char*)zmq_msg_data(&envelope) + 1 + topic_size), buffer, PACKET_SIZE * sizeof(int16_t));
   
        // send the message through the data_socket
        const size_t rs = zmq_msg_send(&envelope, data_socket, 0);
        if (rs != envelope_size)
        {
            printf("ERROR: ZeroMQ error occurred during zmq_msg_send(): %s\n", zmq_strerror(errno));
   
            zmq_msg_close(&envelope);
   
            break;
        }
   
        // dispose of the envelope
        zmq_msg_close(&envelope);

        printf("Message sent; i: %u, topic: %s\n", i, TOPIC);

        sleep(1);
    }

    // garbage collection
    const int rc = zmq_close(data_socket);

    if (rc != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_close(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    const int rd = zmq_ctx_destroy(context);

    if (rd != 0)
    {
        printf("Error occurred during zmq_ctx_destroy(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
