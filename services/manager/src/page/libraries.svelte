<script language="ts">
  import { PlusSquare, Delete, File, Folder, FolderUp } from 'lucide-svelte';
  import { syncedObject } from '../util/storage';

  let libDirectories = [];
  const libraryConfig = syncedObject('library', async () => {
    libDirectories = (await libraryConfig).directories;
  });

  // Housekeeping
  (async () => {
    if (!Array.isArray((await libraryConfig).directories)) (await libraryConfig).directories = [];
    libDirectories = (await libraryConfig).directories;
  })();

  let currentPath      = '~';
  let directoryEntries = [];

  function closeDialogOnClick(el) {
    return e => {
      if (e.target == el) el.close();
    }
  }

  function updateDirectories() {
    storage_readdir(currentPath)
      .then(entries => {
        directoryEntries = entries
          .filter(entry => ((entry.name == '..') || (entry.name.substring(0,1) != '.')))
          .sort((a,b) => (a.name.toLowerCase() > b.name.toLowerCase()) ? 1 : (a.name.toLowerCase() < b.name.toLowerCase()) ? -1 : 0);
      });
  }

  function openAddDialog() {
    currentPath = '~';
    addDialog.returnValue = null;
    addDialog.showModal();
    updateDirectories();
  }

  async function handleAddDialog(event) {
    const type = event.target.returnValue;
    if (type != 'confirm') return;

    (await libraryConfig).directories.push(currentPath);
    // alert(JSON.stringify({ event, type }, null, 2));
  }

  function handleAddDialogNavigate(entry) {
    return () => {
      if (!entry.is_directory) return;
      currentPath = entry.fullpath;
      updateDirectories();
    };
  }

</script>

<div id=page>
  <h2>Libraries</h2>
  <table>
    <thead>
      <tr>
        <th>Name</th>
        <th class="right"><button class="btn-icon" on:click={openAddDialog}><PlusSquare/></button></th>
      </tr>
    </thead>
    <tbody>
      {#each libDirectories as dir}
        <tr>
          <td>{dir}</td>
          <td class="right"><Delete/></td>
        </tr>
      {/each}
    </tbody>
  </table>
</div>

<dialog id=addDialog on:click={closeDialogOnClick(addDialog)} on:close={handleAddDialog}>
  <header>
    Select directory
  </header>
  <form method=dialog>
    <div>
      Path: {currentPath}
    </div>
    <table id=filebrowser>
      <thead>
        <tr>
          <th>Name</th>
        </tr>
      </thead>
      <tbody>
        {#each directoryEntries as entry}
          <tr>
            <td><a on:click={handleAddDialogNavigate(entry)}>{#if entry.name == '..'}<FolderUp/>{:else if entry.is_directory}<Folder/>{:else}<File/>{/if} {entry.name}</a></td>
          </tr>
        {/each}
    </tbody>
    </table>
    <button type=submit value="cancel" >Cancel</button>
    <button type=submit value="confirm">Add</button>
  </form>
</dialog>

<style>
  #page {
    padding: 0 1rem;
  }
  table {
    border-collapse: collapse;
    max-width: 100%;
    width: 40rem;
    margin: 0 auto;
  }
  th, td {
    text-align: left;
  }
  th.right, td.right {
    text-align: right;
  }
  #addDialog tbody td a {
    display: block;
    padding: 0.5em;
  }
</style>
