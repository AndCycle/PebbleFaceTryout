var myAPIKey = '6d6966de2aa0baf93f6d06d7ef536798'; // I know it's a bad practice to put key here, but I haven't finish config page

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  //var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
  //    pos.coords.latitude + "&lon=" + pos.coords.longitude + '&appid=' + myAPIKey;

  var url = "http://api.openweathermap.org/data/2.5/forecast?" + 
			"lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude + 
			"&cnt=1" +
			'&appid=' + myAPIKey;
	
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.list[0].main.temp - 273.15);
      console.log("Temperature is " + temperature);
			
			var temperature_min = Math.round(json.list[0].main.temp_min - 273.15);
			var temperature_max = Math.round(json.list[0].main.temp_max - 273.15);

      // Conditions
      //var conditions = json.list[0].weather[0].main; 
			var conditions = json.list[0].weather[0].id;
      console.log("Conditions are " + conditions);
      
			var icon = json.list[0].weather[0].icon;
			console.log("Icon are " + icon);
			
      // Assemble dictionary using our keys
      var dictionary = {
				"KEY_WEATHER": true,
        "KEY_WEATHER_TEMPERATURE": temperature,
				"KEY_WEATHER_TEMPERATURE_MIN": temperature_min,
				"KEY_WEATHER_TEMPERATURE_MAX": temperature_max,
        "KEY_WEATHER_CONDITIONS_ID": conditions,
				"KEY_WEATHER_ICON_ID": icon
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
		{timeout: 60*1000, maximumAge: 60*60*1000, enableHighAccuracy: false}
  );
}

function announce_ready(delay_ms) {
	
	delay_ms = typeof delay_ms !== 'undefined' ? delay_ms : 1000;


	// Assemble dictionary using our keys
	var dictionary = {
		"KEY_JS_READY": true,
	};

	// Send to Pebble
	Pebble.sendAppMessage(dictionary,
												function(e) {
													console.log("announce js ready sent to Pebble successfully!");
												},
												function(e) {
													console.log("Error sending announce js ready to Pebble!");
													
													delay_ms = delay_ms*2;
													if (delay_ms > 3600*1000) {
														delay_ms = 3600*1000;
													}
													setTimeout(announce_ready, delay_ms, delay_ms); //try again
												}
											 );
    // Get the initial weather
    //getWeather();
	
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
		announce_ready();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
		console.log("AppMessage received!");
		if(e.payload.KEY_WEATHER) {
			console.log("js got weather call");
			getWeather();
		}
    
    
  }                     
);
