<script>
  import { PlusSquare, Delete } from 'lucide-svelte';
</script>

<div id=page>
  <h2>Libraries</h2>
  <table>
    <thead>
      <tr>
        <th>Name</th>
        <th class="right"><PlusSquare/></th>
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
