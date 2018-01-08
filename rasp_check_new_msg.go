package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"time"
)

func main() {

	http.HandleFunc("/", handleRoot)         // set router
	err := http.ListenAndServe(":9090", nil) // set listen port
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}

type ResultItem struct {
	Metadata__ struct {
		Id   string `json:"id"`
		Uri  string `json:"uri"`
		Type string `json:"type"`
	} `json:"__metadata"`

	G_DEVICE, G_CREATED, C_MESSAGES string
}

type Mssg struct {
	D struct {
		Results []ResultItem `json:"results"`
	} `json:"d"`
}

func handleRoot(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Server started!")

	timer := time.NewTicker(5 * time.Second)
	for _ = range timer.C {
		sendRequest()
	}

}

func sendRequest() {
	table := "NEO_8HK0EWACP8ZRUNSO5815EQ13Q.T_IOT_HTTP_PUSH"
	formatType := "?$format=json"
	uri := "https://iotmmsp2000064899trial.hanatrial.ondemand.com/com.sap.iotservices.mms/v1/api/http/app.svc/" + table + formatType
	req, err := http.NewRequest("GET", uri, nil)
	req.Header.Add("Content-Type", `application/json`)
	req.Header.Add("Authorization", `Basic c2Fwdm5pb3RAZ21haWwuY29tOkFiY2QwMTIzNA==`)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		// handle error
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Body read error!", err)
		return
	}

	if len(body) == 0 {
		fmt.Println("Error, empty body!")
		return
	}

	var mssg Mssg

	err = json.Unmarshal(body, &mssg)
	if err != nil {
		fmt.Println("Error parsing JSON:", err)
		return
	}

	results := mssg.D.Results

	fmt.Println(len(results))
	fmt.Println(results[0].C_MESSAGES)

	//
	//if len(results) == 10 {
		uri1 := "http://node1.local/triggerWater?val=1"
		req1, err := http.NewRequest("GET", uri, nil)

		client1 := &http.Client{}
		resp1, err := client1.Do(req1)
		if err != nil {
			// handle error
		}
		fmt.Println(uri1)

		defer resp1.Body.Close()
		body1, err := ioutil.ReadAll(resp1.Body)
		if err != nil {
			fmt.Println("Body read error!", err)
			return
		}
		fmt.Println(string(body1))
	//}
}
