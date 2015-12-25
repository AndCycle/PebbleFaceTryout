Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://my-website.com/config-page.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('Configuration window returned: ' + e.response);
});
