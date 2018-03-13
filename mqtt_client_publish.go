package main

import (
	"encoding/json"
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
	return opts
}

func listen(uri *url.URL, topic string) {
	// client := connect("sub", uri)
	// client.Subscribe(topic, 0, func(client mqtt.Client, msg mqtt.Message) {
	// 	fmt.Printf("*Received Message: [%s] %s\n", msg.Topic(), string(msg.Payload()))
	// })
}

type Msg struct {
	Mode        string `json:"mode" bson:"mode"`
	MessageType string `json:"messageType" bson:"messageType"`
	Messages
}

type Messages struct {
	Messages []Message `json:"messages" bson:"messages"`
}

type Message struct {
	Sen_Temperature int `json:"sen_Temperature" bson:"sen_Temperature"`
	Sen_Humidity    int `json:"sen_Humidity" bson:"sen_Humidity"`
	Vol_Fert_Na     int `json:"vol_Fert_Na" bson:"vol_Fert_Na"`
	Vol_Fert_Ka     int `json:"vol_Fert_Ka" bson:"vol_Fert_Ka"`
	Vol_Total       int `json:"vol_Total" bson:"vol_Total"`
	Timestamp       int `json:"timestamp" bson:"timestamp"`
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

	// go listen(uri, topic)

	client := connect("pub", uri)
	timer := time.NewTicker(1 * time.Second)

	for t := range timer.C {

		msg := Msg{
			Mode:        "async",
			MessageType: "66264498cb1cc090fc4b",
			Messages: Messages{
				[]Message{
					{
						Sen_Temperature: 123,
						Sen_Humidity:    19,
						Vol_Fert_Na:     1200,
						Vol_Fert_Ka:     1500,
						Vol_Total:       26,
						Timestamp:       time.Now().Nanosecond(),
					},
				},
			},
		}
		d, err := json.Marshal(msg)
		if err != nil {
			fmt.Println(err)
			return
		}

		fmt.Println(string(d))
		fmt.Println("time to be sent: ", t.String())
		client.Publish(topic, 2, false, d)
	}
}
