/* 
    DIT PROGRAMMA TEST DE PAHO LIBRARY
*/
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>

#define ADDRESS     "tcp://192.168.0.117:1883"       // MQTT broker URL
#define CLIENTID    "RaspberryPiClient"             // Unique client ID
#define TOPIC       "test/topic"                    // MQTT topic
#define PAYLOAD     "Hello from Raspberry Pi!"      // Message to send
#define QOS         1                               // Quality of Service level
#define TIMEOUT     10000L                          // Timeout in milliseconds

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    // Initialize the MQTT client
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    // Set connection options
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Connect to the broker
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }

    // Create the message
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    // Publish the message
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    // Disconnect from the broker
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return rc;
}
