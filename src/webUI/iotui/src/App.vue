<template>
  <router-view />
</template>

<script setup>

import { mapState, mapActions } from 'pinia'
import {useAppStore} from "./store/app.js"
  
function setCorsAccess() {
  var all_app_domains =
		" https://ssl.gstatic.com https://fonts.googleapis.com/css/ \
                https://netdna.bootstrapcdn.com/font-awesome/4.0.3/css/ \
                https://www.paypal.com/sdk/ \
                https://www.sandbox.paypal.com/xoplatform/logger/api/logger \
                https://use.fontawesome.com/ \
                https://apis.google.com/ \
                https://www.google.com/recaptcha/ \
                https://www.gstatic.com/		\
                https://fonts.gstatic.com/ \
                https://fonts.googleapis.com \
                https://iot.wifinetcom.net \
                http://192.168.1.104:80  ws://192.168.1.104:81 wss://192.168.1.104:81;";
  

	const default_src = "default-src *  android-webview-video-poster: blob: data: gap: " + all_app_domains;
	const style_src = "  style-src  'self' 'unsafe-inline' 'unsafe-eval' " + all_app_domains;
	const script_src = "  script-src  'self' 'unsafe-inline' 'unsafe-eval' " + all_app_domains;
	const img_src =
		"  img-src 'self' blob: data: * android-webview-video-poster: gap:  'unsafe-eval' 'unsafe-inline' " + all_app_domains;
	const connect_src = "  connect-src 'self' file://* cvdfile://* " + all_app_domains;
	const media_src = "  media-src  " + all_app_domains;

	var meta = document.createElement("meta");
	meta.httpEquiv = "Content-Security-Policy";
	meta.content = default_src + style_src + script_src + img_src + media_src + connect_src;

	const headerPage=document.head;
	headerPage.append(meta);
}

var pinConfig={
        "digitalOutputs": [
            {"pin":1, "description": "Stato prisma 1"},
            {"pin":2, "description": "Stato prisma 2"},
            {"pin":3, "description": "Stato prisma 3"},
            {"pin":4, "description": "Stato prisma 4"}
        ],
        "digitalInput": [
            {"pin":30, "description": "Switching Prismi Ottici"},
            {"pin":31, "description": "Interrompi Alimentazione A"},
            {"pin":32, "description": "Interrompi Alimentazione B"}
        ]
    };

    function readConfiguration() {
        const appStore=useAppStore();
                
        fetch('./config.json')
            .then((response) => response.json())
            .then((json) => {
                pinConfig= json; 
                console.log("Config loaded is: ");
                console.log(pinConfig);
                appStore.pinConfiguration=pinConfig;                
            });
    }

    function InitWebSocket() {

        // Let us open a web socket
        var ws = new WebSocket('ws://' + ip + ':81/');

        ws.onopen = function () {
            console.log("Connected.");
            ws.send("Ciao bello");
        };

        ws.onmessage = function (evt) {
            var received_msg = evt.data;
            console.log(received_msg);
        };

        ws.onclose = function () {
            console.log("Disconencted.");
        };

    }

var ip = location.hostname;
ip = "192.168.1.104";
    

setCorsAccess();
readConfiguration();
InitWebSocket();
</script>
