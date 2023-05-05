<script>
  import { PlusSquare, Delete } from 'lucide-svelte';

  let currentPath = '~';

  function closeDialogOnClick(el) {
    return e => {
      if (e.target == el) el.close();
    }
  }

  function openAddDialog() {
    let currentPath = '~';
    addDialog.returnValue = null;
    addDialog.showModal();
  }

  function handleAddDialog(event) {
    const type = event.target.returnValue;
    alert(JSON.stringify({ event, type }, null, 2));
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
      <tr>
        <td>Portable HDD</td>
        <td class="right"><Delete/></td>
      </tr>
      <tr>
        <td>NAS</td>
        <td class="right"><Delete/></td>
      </tr>
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
    <div id=filebrowser>

    </div>
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
  thead {
    border-bottom: 1px solid #FFF2;
  }
  th, td {
    text-align: left;
  }
  th.right, td.right {
    text-align: right;
  }
</style>
