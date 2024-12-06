#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // For sleep function
#include "MQTTClient.h"

#define DEBUG

#define FILE_NAME		"/home/jasper/examen-jasper/digitalemeter_waarden.txt"
#define MAX_LINE_LEN	2048

#define ADDRESS     "tcp://192.168.0.131:1883"  // Local RP MQTT broker address
#define CLIENTID    "CFileReaderClient"
#define TOPIC       "P1/MD2"            // Replace with your topic
#define QOS         1
#define TIMEOUT     10000L

int main( int argc, char *argv[]) {
    FILE *file = NULL;
    char fname[ 255 ] = "";
    char line[MAX_LINE_LEN] = "";
    
    if ( argc != 2 )
        strcpy( fname, FILE_NAME ); // set file_name to default if no input param is provided
    else  
        strcpy( fname, argv[1] );  // set file_name to input param if it is provided

    #ifdef DEBUG
        printf( "Input file = <%s>\n", fname );
    #endif

    // MQTT setup
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect to MQTT broker, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    printf("Connected to MQTT broker at %s\n", ADDRESS);

    // Open the file
    file = fopen(fname, "r");
    if (file == NULL) {
        fprintf( stdout, "Unable to open FILE: %s - error = %s\n", FILE_NAME, strerror( errno ));
        MQTTClient_disconnect(client, TIMEOUT);
        MQTTClient_destroy(&client);
        return EXIT_FAILURE;
    }

    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Publish the line to the MQTT topic
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        pubmsg.payload = line;
        pubmsg.payloadlen = (int)strlen(line);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_deliveryToken token;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        #ifdef DEBUG
            printf("Published message: %s\n", line);
        #endif    

        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        #ifdef DEBUG
            printf("Message delivered with token %d\n", token);
        #endif    
        
        // Add a delay of 50 milisecond
        usleep(50000);
    }

    // Clean up
    fclose(file);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);

    return EXIT_SUCCESS;
}
