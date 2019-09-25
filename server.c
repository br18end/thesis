#include <stdio.h> /* Standard input/output definitions */
#include <stdlib.h> /* Standard General Utilities Library */
#include <fcntl.h> /* File Control Definitions */
#include <unistd.h> /* UNIX Standard Definitions */
#include <errno.h> /* ERROR Number Definitions */
#include <string.h> /* String function definitions */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <time.h> /* Date and time manipulation */
#include <json-c/json.h> /* Parser-JSON */
#include <xbee.h> /* Digi's XBee wireless modules definitions */
#include <MQTTClient.h> /*  Paho MQTT C Client */
#define BUFFER_SIZE 256 // Buffer size
#define SEND_TIME 10 // Send time in sec
#define DEVICE "newDevice@user.user" // altairsmartcore device
#define ADDRESS "mqtt.altairsmartcore.com:1883" // Mqtt client address
#define CLIENTID "streams" // Mqtt client id
#define TOPIC "d93c05fc6da4a2473ab7ca0d5ea15ef7408eea4bcfc4f13853282aa2ee1e6ae8/streams" // Mqtt topic
#define QOS 0 // Quatily of service - At most once (0): A message won’t be acknowledged by the receiver or stored and redelivered by the sender.
#define TIMEOUT 10000L // Mqtt timeout
#define USERNAME "" // Mqtt username
#define PASSWORD "" // Mqtt password

// Declare functions
struct xbee * configure_xbee(struct xbee *xbee, xbee_err ret);
struct xbee_con * connection_xbee(struct xbee *xbee, struct xbee_con *con, xbee_err ret);
void receive_data(struct xbee *xbee, struct xbee_con *con, xbee_err ret);
void send_stream(char* read_buffer);
void callback_function(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **data);
// Create buffer to store the data received
char read_buffer[BUFFER_SIZE];

void main() {
   // Xbee errors variable
    xbee_err ret;
    // Configure xbee
    struct xbee *xbee = configure_xbee(xbee, ret);
    // Create a new connection
    struct xbee_con *con = connection_xbee(xbee, con, ret);
    
    // Receive data from the remote xbee and send it to altairsmartcore
    while(1) {
        receive_data(xbee, con, ret);
        sleep(SEND_TIME);
        send_stream(&read_buffer[0]);
    }
    
    // Shutdown connection
    xbee_conEnd(con);
    // Shutdown libxbee 
    xbee_shutdown(xbee);
}

struct xbee * configure_xbee(struct xbee *xbee, xbee_err ret) {
    // Setup libxbee, using USB port to serial adapter ttyUSBX at 9600 baud and check if errors 
    if((ret = xbee_setup(&xbee, "xbeeZB", "/dev/ttyUSB0", 9600)) == XBEE_ENONE)
        printf("Configuring xbee: OK\n");
    else
        printf("Configuring xbee: %s(Code:%d)\n", xbee_errorToStr(ret), ret);

    // Return xbee
    return xbee;
}

struct xbee_con * connection_xbee(struct xbee *xbee, struct xbee_con *con, xbee_err ret) {
    // Address of the remote xbee
    struct xbee_conAddress address;
    memset(&address, 0, sizeof(address));
    address.addr64_enabled = 1;
    address.addr64[0] = 0x00;
    address.addr64[1] = 0x13;
    address.addr64[2] = 0xA2;
    address.addr64[3] = 0x00;
    address.addr64[4] = 0x41;
    address.addr64[5] = 0x80;
    address.addr64[6] = 0xAD;
    address.addr64[7] = 0xDE;

    // Create a new AT connection to the remote xbee
    if((ret = xbee_conNew(xbee, &con, "Data", &address)) == XBEE_ENONE)
        printf("Connecting xbee: OK\n\n");
    else
        printf("Connecting xbee: %s(Code:%d)\n\n", xbee_errorToStr(ret), ret);

    // Return connection
    return con;
}

void receive_data(struct xbee *xbee, struct xbee_con *con, xbee_err ret) {
    // Associate data with a connection
    if ((ret = xbee_conDataSet(con, xbee, NULL)) != XBEE_ENONE)
        printf("Associating data: OK\n");
    // Configure a callback for the connection, this function is called every time a packet for this connection is received 
    if ((ret = xbee_conCallbackSet(con, callback_function, NULL)) != XBEE_ENONE)
        printf("Configuring callback: %s(Code:%d)\n", xbee_errorToStr(ret), ret);
}

/* Callback function, it will be executed once for each packet that is received on an associated connection */
void callback_function(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **data) {
    // Store data in buffer
    memset(read_buffer, '\0', BUFFER_SIZE);
    strcpy(read_buffer, (*pkt)->data);

    // Get current time and date
    // Initialize date/time struct
    time_t curtime;
    struct tm *timeinfo;
    // Get system current date/time
    time (&curtime);
    // Format date/time variable
    timeinfo = localtime(&curtime);
    // Get current date
    char dateString[12];
    strftime(dateString, sizeof(dateString)-1, "%Y-%m-%d", timeinfo);
    // Get current time
    char timeString[9];
    strftime(timeString, sizeof(timeString), "%H:%M:%S", timeinfo);

    // Create json object
    json_object *jobj = json_object_new_object();
    // Add date/time objects into main json object
    json_object *jdate = json_object_new_string(dateString);
    json_object_object_add(jobj, "Date", jdate);
    json_object *jtime = json_object_new_string(timeString);
    json_object_object_add(jobj, "Time", jtime);

    // Convert the received value to int
    int moisture;
    moisture = atoi(read_buffer);
    // Add the converted value to the json object
    json_object *jmoisture = json_object_new_int(moisture);
    json_object_object_add(jobj,"Moisture", jmoisture);

    // Copy the json string to the buffer
    strcpy(read_buffer, json_object_to_json_string(jobj)); 

    // Transmit a message
    xbee_conTx(con, NULL, "Receiving data: OK");
}

void send_stream(char* read_buffer) {    
    // Create a json object
    json_object *jobj = json_object_new_object();
    // Create a json buffer data array
    json_object *jbuffer = json_object_new_array();
    // Create json strings and add into main json object
    json_object *jtemp;
    jtemp = json_object_new_string("v2");
    json_object_object_add(jobj,"protocol", jtemp);
    jtemp = json_object_new_string("now");
    json_object_object_add(jobj,"at", jtemp);
    jtemp = json_object_new_string(DEVICE);
    json_object_object_add(jobj,"device", jtemp);
    jtemp = json_tokener_parse(read_buffer);
    json_object_object_add(jobj,"data", jtemp);
    
    //Print json object
    printf("JSON STRING: %s\n", json_object_to_json_string(jobj));

    /* Prepare for send stream */
    // Return code
    int rc;
    // Create mqtt client
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    // Create connection to mqtt server
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;    
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    
    // Connect to mqtt server and check if errors
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Connecting to server: Error(Code:%d)\n", rc);
    } else {
        printf("Connecting to server: OK\n");
    }
    
    // Create mqtt message to send
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = (char*)json_object_to_json_string(jobj);
    pubmsg.payloadlen = strlen(json_object_to_json_string(jobj));
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    // Publish message to mqtt server
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n", (int)(TIMEOUT/1000), json_object_to_json_string(jobj), TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n\n", token);
    // Close connections with the server
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
