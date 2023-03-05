<template>
  <v-app-bar :elevation="6">    
    <template v-slot:prepend>
          <v-app-bar-nav-icon></v-app-bar-nav-icon>
    </template>

      <v-app-bar-title>IOT Manager</v-app-bar-title>
      <v-navigation-drawer
        v-model="drawer"
        location="bottom"
        temporary
      >
        <v-list
          :items="items"
        ></v-list>
      </v-navigation-drawer>

      <v-spacer></v-spacer>

      <v-btn icon>
        <v-icon>mdi-magnify</v-icon>
      </v-btn>

      <v-btn icon>
        <v-icon>mdi-dots-vertical</v-icon>
      </v-btn>
  </v-app-bar>
</template>

<script>
  export default {
    data: () => ({
      drawer: false,
      group: null,
      items: [
        {
          title: 'Home',
          value: 'Home',
        },
        {
          title: 'Settings',
          value: 'settings',
        },
      ],
    }),

    watch: {
      group () {
        this.drawer = false
      },
    },
  }
</script>
