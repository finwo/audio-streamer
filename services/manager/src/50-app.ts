import App from './app.svelte';

new App({
  target: document.body,
});

// Title counter
let counter = 0;
setInterval(async () => {
  // @ts-ignore It exists
  if ('function' === typeof window.wv_title_set) {
    // @ts-ignore It exists
    await window.wv_title_set(`Runtime: ${++counter} seconds`);
  }
}, 1000);
