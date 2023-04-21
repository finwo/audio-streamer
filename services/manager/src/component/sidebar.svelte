<script>
  export let route = 'libraries';
</script>

<div>
  <div id=search>
    <input name="search"/>
  </div>
  <div id=menu>
    <a on:click={() => route = 'libraries'}>
      <span>Libraries</span>
    </a>
    <a on:click={() => route = 'playlists'}>
      <span>Playlists</span>
    </a>
    <a on:click={() => route = 'devices'}>
      <span>Devices</span>
    </a>
  </div>
</div>

<style>
  #search {
    padding: 1rem;
  }
  #search > input {
    color: inherit;
    height: 2em;
    border: none;
    border-radius: 1em;
    padding: 0.25em 0.5em;
    background: rgba(255, 255, 255, 0.2);
    font-weight: 700;
    width: 12rem;
  }
  #search > input:focus {
    background: rgba(255, 255, 255, 0.3);
  }

  #menu > a {
    display: block;
    width: 14rem;
    padding: 0 1rem;
    cursor: pointer;
  }
  #menu > a:hover {
    background: #FFF1;
  }
  #menu > a > span {
    display: block;
    padding: 1rem 0;
    border-top: solid 1px #EEE8;
  }
</style>
