// Utilities
import { defineStore } from 'pinia'
import { ref, computed } from 'vue';

export const useAppStore = defineStore('app', () =>{
  
  var appName=ref("RemoteMonitor");
  var pinConfiguration=ref({});
  
  return {appName, pinConfiguration};
  
})
