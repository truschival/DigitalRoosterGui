# Runtime User configuration

## Command line options

DigitalRooster accepts some command line arguments for its runtime behaviour.

-   `-s, --stdout`               log to stdout
-   `-c, --confpath <confpath>`  path of[configuration file](#configuration-file)
-   `-l, --logfile <logfile> `   path of application [log file](#logging-configuration)
-   `-d, --cachedir <cachedir>`  application cache directory
-   `-h, --help`                 displays this help.
-   `-v, --version`              displays version information.

## Configuration file

DigitalRooster runs from any directory and generates on the first start a
default configuration is generated if no config is found.
**Note: The generated config does not include Weather locationId and apiKey**

The configuration path is derived from
[QStandardPaths::ConfigLocation](http://doc.qt.io/qt-5/qstandardpaths.html)
i.e.:
-   On Linux :  `~/.config/DigitalRooster/digitalrooster.json`
-   On Windows:  `%LOCALAPPDATA%/DigitalRooster/digitalrooster.json`

You can update the file with a text editor upon save the configuration will
be reloaded automatically. There is no need to restart the program.

Using the REST API you can configure alarms, podcasts and radio stations.
For more details refer to [documentation/rest.md](rest.md)

### Global configuration and common properties of objects

-   `id` of the objects is auto generated if not present.

-   `name` is updated according to infromation form RSS (for podcasts)
            or shoutcast information for radio streams (if available)

-   `alarmTimeout` time in minutes an alarm should play until it is
                     automatically stopped.

-   `brightnessActive` is the display background when active (0-100%)

-   `brightnessStandby` is the display background in standby mode (0-100%)

-   `autoBacklight` if `true` the backlight is controlled reading the value 
     from sensor

-   `volume` is the default volume

-   `version` project version for this config file
               (upgrades and backward compatibility not yet implemented)

-   `sleepTimeout` time in minutes after which standby is activated

### Alarm objects
`Alarms` is an array of alarm objects.

-   `id` unique identifier - auto generated if not present

-   `enabled` enabled/disables triggering of alarm

-   `url` stream uri to play for this alarm

-   `time` Time of day when to trigger the alarm

-   `period` frequency when to trigger alarm.
              Possible values are `workdays`, `weekend`, `daily`

-   `volume` volume to set for playing alarm

If an alarm is triggered and the stream source is unavailable or has
errors a fallback sound will be played.

### Podcast Source objects
`Podcasts` is an array containing individual RSS sources for podcasts.
The only mandatory property is `url` others are optional:
-   `id` unique identifier - auto generated if not present
-   `title` human readable identifier, will be updated according to RSS XML
-   `url` RSS url

### Internet Stream objects
`InternetRadio` is an array containing individual stream source configurations.
The only mandatory property is `url` others are optional:
-   `id` unique identifier - auto generated if not present

-   `name` human readable identifier, updated according to shoutcast
    information when played (if available)

-   `url` stream url

### Weather
The `Weather` object configures the displayed weather information
form [openweathermap.org](https://api.openweathermap.org)

-   `locationId` identifier for the geographic location, see [http://bulk.openweathermap.org/sample/city.list.json.gz](http://bulk.openweathermap.org/sample/city.list.json.gz)
     e.g. Esslingen: `"locationId" = "2928751"` or Porto Alegre: `"locationId" = "3452925"`

-   `apiKey` access token to the openweather api.  **Get your personal key**
     by signing up to [https://openweathermap.org](https://home.openweathermap.org/users/sign_up)

### Example configuration file

```JSON
{
    "Alarms": [
        {
            "enabled": true,
            "id": "1f2e926d-c4c1-4434-b130-a3fec3412352",
            "period": "workdays",
            "time": "06:30",
            "url": "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3",
            "volume": 30
        },
        {
            "enabled": false,
            "id": "c61eb7bb-94f4-4619-bc89-a22016786899",
            "period": "weekend",
            "time": "09:00",
            "url": "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3",
            "volume": 30
        }
    ],
    "InternetRadio": [
        {
            "id": "7fccbecd-c3f0-47cf-a69c-993a12b630a6",
            "name": "BBC World Service News",
            "url": "http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-einws"
        },
        {
            "id": "5f55f5b0-1439-4ebe-99b7-581bbcc810d4",
            "name": "Deutschlandfunk",
            "url": "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3"
        },
        {
            "id": "7cfb3f7c-24c3-4741-89e3-97cff2df71d0",
            "name": "FM4",
            "url": "https://fm4shoutcast.sf.apa.at"
        }
    ],
    "Podcasts": [
        {
            "id": "b890ca9b-30af-4d42-8d51-24fcc7b8feeb",
            "title": "Arms Control Wonk",
            "updateInterval": 3600,
            "url": "http://armscontrolwonk.libsyn.com/rss"
        },
        {
            "id": "19d2dcee-ec2f-4489-b2f7-461274c7644f",
            "title": "My Dad Wrote A Porno",
            "updateInterval": 3600,
            "url": "https://rss.acast.com/mydadwroteaporno"
        },
        {
            "id": "51e245e9-df28-4d72-bb40-5cba688c1b2e",
            "title": "Alternativlos",
            "updateInterval": 3600,
            "url": "https://alternativlos.org/alternativlos.rss"
        }
    ],
    "Weather": {
        "apiKey": "__PROVIDE_YOUR_OWN_API_KEY__",
        "locationId": "__PROVIDE_YOUR_LOCATION_ID__HERE__",
        "updateInterval": 3600
    },
    "alarmTimeout": 30,
    "brightnessActive": 25,
    "brightnessStandby": 25,
    "sleepTimeout": 60,
    "version": "0.10",
    "volume": 30,
    "wpa_ctrl": "/var/run/wpa_supplicant/wlp2s0"
}

```

## Environment Variables

This part is only works on the embedded linux build. There are a few
environment variables that allow for configuration of the actually used
input event interfaces or backlight control.

Usually you don't need to set any of these variables if you use the device
tree provided during build with buildroot_extenal. For development purposes
the following variables are provided:

Setting the input event interfaces directly:
-   ``DR_PUSH_EVENT_PATH``  input device for push button events
     e.g. ``'/dev/input/event1'``

-   ``DR_ROTARY_EVENT_PATH``  input device for rotary encoder events
     e.g. ``'/dev/input/event2'`

If ``DR_PUSH_EVENT_PATH`` or ``DR_ROTARY_EVENT_PATH`` are set this
value is taken regardless if the device node exists or if it is
the *correct* device.

### Backlight control

The backlight control on a embedded device using DigitalRooster-Audio-Mk3
is implemented on an external TI TLC59208F LED driver chip. 
The backlight channel is created at ``/sys/class/leds/bl/brightness``.

Using the environment variable ``BACKLIGHT_PATH`` the path can be adjusted.
Make sure you include the full path to the device

If a APDS9960 is connected the backlight can be controlled automatically 
accoring to ambient luminosity.
The sensor value is read per default from: 
 `/sys/bus/iio/devices/iio:device0/in_intensity_clear_raw`

Using the environment variable ``ALS_PATH`` the path to the value can be 
adjusted.


## Logging configuration

Digitalrooster supports dynamic logging configuration using
[QLoggingCategory](http://doc.qt.io/qt-5/qloggingcategory.html) i.e.:
-   On Linux:   `~/.config/QtProject/qtlogging.ini`
-   On Windows: `%LOCALAPPDATA%/Temp/Digitalrooster.log`

You can specify the log file using the `-l ` [command line option](#command-line-options).
The default runtime logfile is created in:
`QStandardPaths::TempLocation/Digitalrooster.log` i.e.:
-   On Linux:   `/tmp/Digitalrooster.log`
-   On Windows: `%LOCALAPPDATA%/Temp/Digitalrooster.log`

### Logging example configuration

All debug messages except for `HttpClient` and `AlarmMonitor` are disabled

```INI
[Rules]
*.debug=false
DigitalRooster.AlarmMonitor.debug=true
DigitalRooster.HttpClient.debug=true
```
