<template>
  <div>
    <v-snackbar
      v-model="successAlert"
      :timeout="3000"
      color="green"
    >
      Operazione eseguita senza errori.
    </v-snackbar>
    

    <div v-if="bShowBoardParameters">
      <v-sheet width="300" class="mx-auto">
        <v-form @submit.prevent>
          <v-text-field
            v-model="boardIp"            
            label="Board IP"
          ></v-text-field>
          <v-text-field
            v-model="boardPort"            
            label="Board Port"
          ></v-text-field>
          <v-btn type="button" block class="mt-2" @click="connectBoard"
            >Connect</v-btn
          >
        </v-form>
      </v-sheet>
    </div>
    <div v-if="bShowBoardSettings">
      
      <template
        v-for="group in pinConfiguration.groupsDigitalInputs"
        v-bind:key="group.gid"
      >
        <v-card class="mx-auto" width="400" prepend-icon="mdi-home">
          <template v-slot:title>
            {{ group.description }}
          </template>

          <v-card-text> </v-card-text>

          <v-chip-group>
            <v-chip
              :id="getChipPinId(input.pin)"
              v-for="input in group.digitalInputs"
              v-bind:key="input.pin"
              >{{ input.description }}</v-chip
            >
          </v-chip-group>
        </v-card>
      </template>

      <template
        v-for="group in pinConfiguration.groupsDigitalOutputs"
        v-bind:key="group.gid"
      >
        <v-card class="mx-auto" width="400" prepend-icon="mdi-home">
          <template v-slot:title>
            {{ group.description }}
          </template>

          <v-card-text> </v-card-text>
          <v-switch
            v-for="output in group.digitalOutputs"
            :id="getSwitchPinId(output.pin)"
            :label="output.description"            
            v-bind:key="output.pin"
            v-model="switches"
            :value="output.pin"
            color="primary"
            @change="manageSwitches"
          ></v-switch>
        </v-card>
        <v-divider></v-divider>       
      </template>
        
          <v-btn type="button" block class="mt-2" @click="disconnectBoard"
            >Disconnect</v-btn
          >

      
    </div>
  </div>
</template>

<script>
import { useAppStore } from "../store/app.js";
const appStore = useAppStore();

export default {
  data: () => ({
    bShowBoardParameters: true,
    bShowBoardSettings: false,
    boardIp: "192.168.1.11",
    boardPort: "81",
    pinConfig: {},
    pinConfiguration: {},
    switches: [],
    ws: {},
    successAlert: false,
  }),
  methods: {
    parseWebSocketMessage(wsMsg) {
      console.log(wsMsg);
      if(wsMsg.status==='ok' && wsMsg.cmd==="connect") {
        this.readConfiguration();
        this.bShowBoardParameters=false; 
        this.bShowBoardSettings=true;
      }
      if(wsMsg.cmd==="pinState") {
        let pin=wsMsg.pin;
        let pinState=wsMsg.state;
        if(pinState==1) this.setDigitalOutputHigh(pin);
        if(pinState==0) this.setDigitalOutputLow(pin);
      }

      if(wsMsg.cmd==="setOnPin" && wsMsg.status==="ok") {
        this.successAlert=true;
        setTimeout(()=>{this.successAlert=false;},3000);
      }
      if(wsMsg.cmd==="setOffPin" && wsMsg.status==="ok") {
        this.successAlert=true;
        setTimeout(()=>{this.successAlert=false;},3000);
      }
      this.ws.send({cmd: "", status: "ok"});
    },
    readConfiguration() {
      fetch("./"+this.boardIp+".json")
        .then((response) => response.json())
        .then((json) => {
          this.pinConfiguration = json;
          console.log("Config loaded is: ");
          console.log(this.pinConfig);
          appStore.pinConfiguration = this.pinConfiguration;
        });
    },
    initWebSocket() {
      let componentUI=this;
      // Let us open a web socket
      var webSocketAddress="ws://" + this.boardIp + ":"+this.boardPort+"/";
      componentUI.ws = new WebSocket(webSocketAddress);

      componentUI.ws.onopen = function () {
        console.log("Connected.");
        componentUI.ws.send("connect");
      };

      componentUI.ws.onmessage = function (evt) {
        var received_msg = evt.data;        
        //console.log(received_msg);
        var msgObj=JSON.parse(received_msg);
        componentUI.parseWebSocketMessage(msgObj);
      };

      componentUI.ws.onclose = function () {
        console.log("Disconnected.");
      };
    },
    setCorsAccess() {
      var webSocketAddress="ws://" + this.boardIp + ":"+this.boardPort+"/";
      var webBoardAddress="http://" + this.boardIp + ":"+this.boardPort+"/";
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
                      "+webSocketAddress+" "+webBoardAddress+ ";";

      const default_src =
        "default-src *  android-webview-video-poster: blob: data: gap: " +
        all_app_domains;
      const style_src =
        "  style-src  'self' 'unsafe-inline' 'unsafe-eval' " + all_app_domains;
      const script_src =
        "  script-src  'self' 'unsafe-inline' 'unsafe-eval' " + all_app_domains;
      const img_src =
        "  img-src 'self' blob: data: * android-webview-video-poster: gap:  'unsafe-eval' 'unsafe-inline' " +
        all_app_domains;
      const connect_src =
        "  connect-src 'self' file://* cvdfile://* " + all_app_domains;
      const media_src = "  media-src  " + all_app_domains;

      var meta = document.createElement("meta");
      meta.httpEquiv = "Content-Security-Policy";
      meta.content =
        default_src +
        style_src +
        script_src +
        img_src +
        media_src +
        connect_src;

      const headerPage = document.head;
      headerPage.append(meta);
    },
    manageSwitches(elem) {                 
      let idElement=elem.srcElement.id;
      let pin=idElement.replace("switchInputPin","");
      
      let bValuePin=this.switches.find(element => element == pin);
      if(bValuePin) {
        this.ws.send("setOnPin_"+pin);
        console.log("Switch pin "+ pin + " in state on");
      }
      if(!bValuePin) {
        this.ws.send("setOffPin_"+pin);      
        console.log("Switch pin "+ pin + " in state off");
      }
    },
    getChipPinId(pin) {
      return "chipInputPin" + pin;
    },
    getSwitchPinId(pin) {
      return "switchInputPin" + pin;
    },
    setDigitalOutputHigh(pin) {
      var id = "chipInputPin" + pin;
      if (document.getElementById(id))
        document.getElementById(id).style.color = "green";
    },
    setDigitalOutputLow(pin) {
      var id = "chipInputPin" + pin;
      if (document.getElementById(id))
        document.getElementById(id).style.color = "red";
    },
    connectBoard() {
      this.setCorsAccess();
      this.initWebSocket();
    },
    disconnectBoard() {
      this.ws.close();
      this.bShowBoardParameters=true; 
      this.bShowBoardSettings=false;
    },
  },
  created() {
    /*const component = this;
    setInterval(() => {
      console.log("Try to change status...", component);
      this.setDigitalOutputHigh(5);
      this.setDigitalOutputLow(6);
      this.setDigitalOutputHigh(12);
      this.setDigitalOutputLow(13);
    }, 5000);
    setInterval(() => {
      console.log("Try to change status...", component);
      this.setDigitalOutputHigh(6);
      this.setDigitalOutputLow(5);
      this.setDigitalOutputHigh(13);
      this.setDigitalOutputLow(12);
    }, 7000);*/
  },
  mounted() {
    const appStore = useAppStore();
    this.pinConfiguration = appStore.pinConfiguration;
    console.log("Loaded Pin Configuration: ", this.pinConfiguration);
  },
  computed: {},
    
};
</script>
