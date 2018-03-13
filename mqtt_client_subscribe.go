package main

import (
	"fmt"
	"log"
	"net/url"
	"os"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func connect(clientId string, uri *url.URL) mqtt.Client {
	opts := createClientOptions(clientId, uri)
	client := mqtt.NewClient(opts)
	token := client.Connect()
	for !token.WaitTimeout(3 * time.Second) {
	}
	if err := token.Error(); err != nil {
		log.Fatal(err)
	}
	return client
}

func createClientOptions(clientId string, uri *url.URL) *mqtt.ClientOptions {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(fmt.Sprintf("tcp://%s", uri.Host))
	opts.SetUsername(uri.User.Username())
	password, _ := uri.User.Password()
	opts.SetPassword(password)
	opts.SetClientID(clientId)
	opts.SetCleanSession(false)
	return opts
}

func listen(uri *url.URL, topic string) {
	client := connect("sub", uri)
	client.Subscribe(topic, 0, func(client mqtt.Client, msg mqtt.Message) {
		fmt.Printf("* [%s] %s\n", msg.Topic(), string(msg.Payload()))
		//check msg type dda3d08ee92d04098686
		//get device id, req_type, water_volumn, device_target_id
		//trigger corresponding device through local network if necessary
	})

}

func main() {
	uri, err := url.Parse(os.Getenv("CLOUDMQTT_URL"))
	if uri.String() == "" {
		uri, _ = url.Parse("mqtt://knztzyag:z2jFAMh_JYyU@m14.cloudmqtt.com:11543/iot/data/iotmmsp2000064899trial/v1/45edbcf6-a158-4dd2-a53b-17c1b380a51c")
	}
	fmt.Println("url: ", uri)
	if err != nil {
		log.Fatal(err)
	}
	topic := uri.Path[1:len(uri.Path)]
	if topic == "" {
		topic = "test"
	}
	listen(uri, topic)

	// client := connect("pub", uri)
	timer := time.NewTicker(1 * time.Second)

	for _ = range timer.C {
	}
}
