#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>

#define ADDRESS     "mqtt.carriots.com:1883"
#define CLIENTID    "streams"
#define TOPIC       "d93c05fc6da4a2473ab7ca0d5ea15ef7408eea4bcfc4f13853282aa2ee1e6ae8/streams"
#define PAYLOAD     "{\"protocol\": \"v2\", \"at\": \"now\", \"device\": \"newDevice@Agroiotec.Agroiotec\", \"data\": null }"
#define QOS         0
#define TIMEOUT     10000L
#define USERNAME "d93c05fc6da4a2473ab7ca0d5ea15ef7408eea4bcfc4f13853282aa2ee1e6ae8"
#define PASSWORD ""

int main(int argc, char* argv[]){
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);/*
    char* payload = PAYLOAD;
    int payloadlen = strlen(PAYLOAD);
    int qos = QOS;
    int retained = 0;
    MQTTClient_publish(client, TOPIC, payloadlen, payload, qos, retained, &token);*/
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
