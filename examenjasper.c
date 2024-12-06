
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "settings.h" //bestand met settings


void delivered(void *context, MQTTClient_deliveryToken dt){
    deliveredtoken = dt;
}

void writefile(char *input){
    FILE *fptr;

// Open a file in writing mode
fptr = fopen(filename3, "w");

// Write some text to the file
fprintf(fptr,"%s\n", input);

// Close the file
fclose(fptr);
return;
}


void beginwaarde(){
      FILE* file = fopen(filename, "r");
      char buffer[1000];
    
     if (file == NULL) {
        printf("Unable to open file: %s\n", filename);
        return;
    }
  
  if (fgets(buffer, sizeof(buffer), file) != NULL) {
       // printf("First line: %s", buffer);

printf("STARTWAARDE \n");
printf(" \n");
   char* split = strtok(buffer, ";");
   printf("DATUM - TIJD :%s\n",split);
    split = strtok(NULL, ";");
    //printf("%s\n",split);
    split = strtok(NULL, ";");
    //printf("%s\n",split);
    split = strtok(NULL, ";");
    printf(" OBRENGST Dag : %s\n",split);
    split = strtok(NULL, ";");
    printf("DAG TOTAAL VERBRUIK: %s\n",split);
    split = strtok(NULL, ";");
    printf("Nacht Totaal Verbruik :%s\n",split);
    split = strtok(NULL, ";");
    printf("Obrengst nacHT :%s\n",split);
    split = strtok(NULL, ";");
    //printf("%s\n",split);
    split = strtok(NULL, ";");
   // printf("%s\n",split);
    split = strtok(NULL, ";");
    printf("Gas totaal :%s\n",split);
    return;
    } 
    
    else {
        printf("Error reading from file.\n");
    }

        
   fclose(file);
    return;
    }

void print_end(){
    printf("++++++++++++++++++++++++++++ \n");
    printf("ELEKTRICITEIT + GAS VERBRUIK \n");
    printf("++++++++++++++++++++++++++++ \n");
    beginwaarde();


      printf("++++++++++++++++++++++++++++ \n");
    printf("EINDE RAPORT \n");
    printf("++++++++++++++++++++++++++++ \n");

    return;
}
//functie word aangeroepen waneer bericht is verzonden op broker_publish


//functie word aangeroepen waneer bericht is aangekomen op broker_read
int message_arrived(void* context, char* topicname, int topicLen, MQTTClient_message *message){
    
    //inlezen bericht broker_read
    char* input = ((char*)message->payload);
    printf("Message arrived on topic: %s\n", topicname);
    printf("INPUT MSG : <%s> \n", input);
    
    writefile(input);
      if (strcmp(input, END_MSG) == 0) {
            printf("end! \n");
            print_end();
            return 0;

        }

    //write_file(max, min, avg);

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
}
