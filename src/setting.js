Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://rawgit.com/AndCycle/PebbleFaceTryout/master/config_webpage/index.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('Configuration window returned: ' + e.response);
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

});
