<script language="ts">
  import * as theme from './config/theme';

  import SidebarComponent from './component/sidebar.svelte';

  import DevicesPage   from './page/devices.svelte';
  import LibrariesPage from './page/libraries.svelte';
  import PlaylistsPage from './page/playlists.svelte';
  import SecurityPage  from './page/security.svelte';

  export let route = 'libraries';
  export let dat   = null;

  // Storage tester
  (async () => {
    if ('function' === typeof window.storage_get) {
      const value = await storage_get('key');
      dat = JSON.stringify(value);
    }
    if ('function' === typeof window.storage_set) {
      try {
        alert(JSON.stringify(await storage_set('key', {
          pizza: 'calzone'
        })));
      } catch(e) {
        alert('ERR: ' + e.message);
      }
    }
  })();

</script>

<div id="layout">

  <aside>
    <SidebarComponent bind:route={route}/>
  </aside>

  <main>
    {#if route == "devices"}
      <DevicesPage/>
    {:else if route == "libraries"}
      <LibrariesPage/>
    {:else if route == "playlists"}
      <PlaylistsPage/>
    {:else if route == "security"}
      <SecurityPage/>
    {:else}
      4-oh-4
    {/if}
  </main>

  <footer>
    footer {dat}
  </footer>
</div>

<style>
  #layout {
    display: flex;
    flex-direction: row;
    height: 100%;
  }

  aside {
    background: #333333;
    box-shadow: 0 0 1em #0008;
    -webkit-box-shadow: 0 0 1em #0008;
    -moz-box-shadow: 0 0 1em #0008;
  }

  main {
    flex-grow: 1;
    position: relative;
  }

  footer {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    min-height: 5rem;
    background: #333333;
    border: 1px solid #FFF;
  }
</style>
