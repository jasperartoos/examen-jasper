// MQTT SETTINGS
#define ADDRESS     "tcp://192.168.0.131:1883"
#define CLIENTID    "RPI-Jasper"
#define TOPIC_READ  "P1/MD2"
#define TOPIC_PUB   "pubjasper/topic"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

//definieren lengten
#define OUTPUTSTR_LEN   256
#define MAX_ELEM        256
#define filename "digitalemeter_waarden.txt"
#define filename3 "outputlog.txt"
#define END_MSG "00.00.00-00:00:00;0;0.000000;000.000000;0000;000000;0000.000000;000.000000;000.000000;00.00.00-00:00:00;0000.000000"