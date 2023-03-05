<template>
  <div>
    <template v-for="group in pinConfiguration.groupsDigitalInputs" v-bind:key="group.gid">
      <v-card
        class="mx-auto"
        width="400"
        prepend-icon="mdi-home"
      >
        <template v-slot:title>
          {{ group.description }}
        </template>

        <v-card-text>
          
        </v-card-text>

        <v-chip-group >          
          <v-chip :id="getChipPinId(input.pin)" v-for="input in group.digitalInputs" v-bind:key="input.pin">{{input.description}}</v-chip>          
        </v-chip-group>
      </v-card>
    </template>
    
    <template v-for="group in pinConfiguration.groupsDigitalOutputs" v-bind:key="group.gid">
      <v-card
        class="mx-auto"
        width="400"
        prepend-icon="mdi-home"
      >
        <template v-slot:title>
          {{ group.description }}
        </template>

        <v-card-text>
          
        </v-card-text>
        <v-switch 
          :id="getChipPinId(output.pin)" 
          :label="output.description" v-for="output in group.digitalOutputs" 
          v-bind:key="output.pin"
          model.value="switches"
          update:modelValue="manageSwitches"></v-switch>        
      </v-card>
      <v-divider></v-divider>
    </template>
  </div>

</template>

<script>
import { useAppStore } from '../store/app.js'
const appStore=useAppStore();

export default {
    data: () => ({      
      pinConfiguration: {},
      switches: []
    }),
    methods: {
      manageSwitches: () => {
        console.log("Change switch value: ");
      },
      getChipPinId: (pin)=>{
          return "chipInputPin"+pin;
      },      
      setDigitalOutputHigh: (pin) =>{
        var id="chipInputPin"+pin;
        if(document.getElementById(id))
        document.getElementById(id).style.color="red";
      },
      setDigitalOutputLow: (pin) =>{
        var id="chipInputPin"+pin;
        if(document.getElementById(id))
        document.getElementById(id).style.color="red";
      },
    },
    created() {
      const component=this;
      setTimeout(() => {
        console.log("Try to change status...",component);
        this.setDigitalOutputHigh(5);
        this.setDigitalOutputLow(6);
      },5000);

    },

    mounted() {
      const appStore=useAppStore();
      this.pinConfiguration=appStore.pinConfiguration;
      console.log("Loaded Pin Configuration: ", this.pinConfiguration);      
    },
    computed:{
      
    }
    
}
</script>
