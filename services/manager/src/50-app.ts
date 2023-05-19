import App from './app.svelte';
import { syncedObject } from './util/storage';

new App({
  target: document.body,
});

(async () => {
  const guiConfig = await syncedObject('gui');

  // Title counter
  let counter = guiConfig.runtime || 0;
  setInterval(async () => {
    // @ts-ignore It exists
    if ('function' === typeof window.wv_title_set) {
      // @ts-ignore It exists
      await window.wv_title_set(`Runtime: ${++counter} seconds`);
      guiConfig.runtime = counter;
    }
  }, 1000);

  const iPort = await http_port();
  console.log(`port: ${iPort}`);
})();
