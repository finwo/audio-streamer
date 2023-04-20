let counter = 0;
setInterval(() => {
  counter++;

  document.body.innerText = `I've been running for ${counter} seconds!!`;
}, 1000);
