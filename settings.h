// MQTT SETTINGS
#define ADDRESS     "tcp://192.168.0.117:1883"
#define CLIENTID    "RPI-Jasper"
#define TOPIC_READ  "readjasper/topic"
#define TOPIC_PUB   "pubjasper/topic"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

//definieren lengten
#define OUTPUTSTR_LEN   256
#define MAX_ELEM        256
#define filename "output.txt"