Pebble.addEventListener('showConfiguration', function(e) {
    var options = {
            'temperature_unit': 'Kelvin'
        };
    

  // http://stackoverflow.com/questions/1714786/querystring-encoding-of-a-javascript-object 
  var serialize = function(obj) {
      var str = [];
      for(var p in obj)
        if (obj.hasOwnProperty(p)) {
          str.push(encodeURIComponent(p) + "=" + encodeURIComponent(obj[p]));
      }
    return str.join("&");
  }
    
  var serverURL = 'http://www.andcycle.idv.tw/~andcycle/tmp/dev/PebbleFaceTryout/config_webpage/index.html';
    
  var url = serverURL+'?'+serialize(options);
  
  console.log('Open configuration url: ' + url);
  
  // Show config page
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  /*
    [PHONE] pebble-app.js:?: Configuration window returned: %7B%22temperature_unit%22%3A%22Celcius%22%7D
    [PHONE] pebble-app.js:?: Configuration page returned: {"temperature_unit":"Celcius"}
  */
  console.log('Configuration window returned: ' + e.response);
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

});
