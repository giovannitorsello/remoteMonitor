<template>
  <router-view />
</template>

<script setup>

import { mapState, mapActions } from 'pinia'
import {useAppStore} from "./store/app.js"
  
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

    
</script>
