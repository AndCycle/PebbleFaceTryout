module.exports = [
  {
    "type": "heading",
    "defaultValue": "time 4391 Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "toggle",
        "messageKey": "SecondHand",
        "label": "Enable Seconds Hand",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "DateTextBox",
        "label": "Show Date Text Box",
        "defaultValue": true
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Tap Activate"
      },
      {
        "type": "slider",
        "messageKey": "TapTimeout",
        "label": "Timeout",
        "defaultValue": 30
      },
      {
        "type": "toggle",
        "messageKey": "TapSecondHand",
        "label": "Seconds Hand",
        "defaultValue": false
      },
    ]

  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather",
      },
      {
        "type": "toggle",
        "label": "Show Weather Info",
        "messageKey": "ShowWeather",
        "defaultValue": false,
      },
      {
        "type": "radiogroup",
        "messageKey": "TemperatureUnit",
        "label": "Temperature Unit",
        "defaultValue": "Celsius",
        "options": [
          {
            "label": "Celsius",
            "value": "Celsius"
          },
          {
            "label": "Fahrenheit",
            "value": "Fahrenheit",
          },
//          {
//            "label": "Kelvin",
//            "value": "Kelvin",
//          }
        ]
      },
      {
        "type": "input",
        "label": "API key",
        "messageKey": "OpenWeatherMapAPPID",
      },
      {
        "type": "text",
        "defaultValue": "Provide by OpenWeatherMap"
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];