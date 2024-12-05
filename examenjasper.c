#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "settings.h" //bestand met settings

int extract_input(char *input, double inputdata[], int max_elem){
    int index = 0;
    char *token = strtok(input, ";");
    
    while (token != NULL && index < max_elem){
        inputdata[index++] = atof(token);
        token = strtok(NULL, ";");
    }
    return index;

}

double calc_max(double inputdata[], int size){
    double max = inputdata[0];
    for (int i = 1; i < size; i++){
        if(inputdata[i] > max){
            max = inputdata[i];
        }
    }
    return max;
}

double calc_min(double inputdata[], int size){
    double min = inputdata[0];
    for (int i = 1; i < size; i++){
        if(inputdata[i] < min){
            min = inputdata[i];
        }
    }
    return min;
}

double calc_avg(double inputdata[], int size){
    double sum = 0.0;
    for(int i = 0; i < size ; i++){
        sum += inputdata[i];
    } 
    double avg = (sum/size);
    return avg;
}

void write_file(double max, double min, double avg){
    FILE*file = fopen(filename, "w");
    if(file == NULL){
        printf("ERROR : could not open file %s .",filename);
        return;
    }
    fprintf(file, "Max: %.2f\n", max);
    fprintf(file, "Min: %.2f\n", min);
    fprintf(file, "Avg: %.2f\n", avg);
    fclose(file);

}

//functie word aangeroepen waneer bericht is verzonden op broker_publish
void delivered(void *context, MQTTClient_deliveryToken dt){
    deliveredtoken = dt;
}

//functie word aangeroepen waneer bericht is aangekomen op broker_read
int message_arrived(void* context, char* topicname, int topicLen, MQTTClient_message *message){
    
    //inlezen bericht broker_read
    char* input = ((char*)message->payload);
    printf("Message arrived on topic: %s\n", topicname);
    printf("INPUT MSG : <%s> \n", input);
    
    //splitsen string
    double inputdata[MAX_ELEM];
    int tempindex = extract_input(input, inputdata , MAX_ELEM);

    //bereken gegevens en (print)
    double max = calc_max(inputdata, tempindex);
    double min = calc_min(inputdata, tempindex);
    double avg = calc_avg(inputdata, tempindex);

    printf("Max: %.2f\n",max);
    printf("Min: %.2f\n",min);
    printf("Avg: %.2f\n",avg);

    write_file(max, min, avg);

    //opstellen output MSG
    char outputstr[OUTPUTSTR_LEN];
    strcpy(outputstr, input);
    printf("OUTPUT MSG : <%s> \n",outputstr);
   
    //versturen outputstring broker_pub
    MQTTClient client = (MQTTClient)context;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = outputstr;
    pubmsg.payloadlen = strlen( outputstr );
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, TOPIC_PUB, &pubmsg, &token);
    
    //leegmaken geheugen
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicname);

    return (1);

}

//functie word aangeroepen waneer verbinding met broker verbreekt
void connection_lost(void *context, char *cause){
    printf("Connection lost.");
}


int main(){

    //MQTT OPTIES
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    //aanmaken MQTT Client + instellen connectie opties
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    //calback functie
    MQTTClient_setCallbacks(client, client, connection_lost, message_arrived, delivered);

    //verbinden met broker
    if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the broker. \n");

    //Subscribe op de topic
    MQTTClient_subscribe(client, TOPIC_READ, QOS);
    printf("Subscribed to topic: %s\n", TOPIC_READ);
    printf("Waiting for messages.... \n");

    //wachten op messages
    while(1){}

    //afsluiten MQTT Client
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;

    return(0);
}