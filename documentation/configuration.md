# Runtime User configuration

## Command line options

DigitalRooster accepts some command line arguments for its runtime behaviour.

-   `-s, --stdout`                 log to stdout
-   `-c, --confpath <confpath>`  configuration file path see [Configuration file](#configuration-file)
-   `-l, --logfile <logfile> `   application log <file> see [Log file](#logging-configuration)
-   `-d, --cachedir <cachedir>`  application cache <directory>
-   `-h, --help`                   Displays this help.
-   `-v, --version`                Displays version information.

## Configuration file

DigitalRooster runs from any directory and generates on the first start a
default configuration is generated if no config is found.

The configuration path is derived from
[QStandardPaths::ConfigLocation](http://doc.qt.io/qt-5/qstandardpaths.html)
i.e.:
-   On Linux :  `~/.config/DigitalRooster/digitalrooster.json`
-   On Windows:  `%LOCALAPPDATA%/DigitalRooster/digitalrooster.json`

You can update the file with a text editor upon save the configuration will
be reloaded automatically. There is no need to restart the program.

Using the REST API you can configure alarms, podcasts and radio stations.
For more details refer to [documentation/rest.md](./documentation/rest.md)

### Global configuration and common properties of objects

-   `id` of the objects is auto generated if not present.

-   `name` is updated according to infromation form RSS (for podcasts)
            or shoutcast information for radio streams (if available)

-   `AlarmTimeout` time in minutes an alarm should play until it is
                     automatically stopped.

-   `SleepTimeout` is not yet implemented.

-   `brightnessActive` is the display background when active (0-100%)

-   `brightnessStandby` is the display background in standby mode (0-100%)

-   `volume` is the default volume

-   `Version` project version for this config file
               (upgrades and backward compatibility not yet implemented)

-   `SleepTimeout` time in minutes after which standby is activated

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
-   `title` human readable identifier, updated according to RSS XML
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

-   `LocationId` identifier for the geographic location, see [http://bulk.openweathermap.org/sample/city.list.json.gz](http://bulk.openweathermap.org/sample/city.list.json.gz)
     e.g. Esslingen: `"LocationId" = "2928751"` or Porto Alegre: `"LocationID" = "3452925"`

-   `apiKey` access token to the openweather api. You have to get your own key from [https://openweathermap.org](https://openweathermap.org)


### Example configuration file
```JSON
{
    "AlarmTimeout": 15,
    "Alarms": [
        {
            "enabled": true,
            "id": "{43eac57e-2c63-45f6-9748-b18e7d7a8666}",
            "period": "workdays",
            "time": "17:58",
            "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3",
            "volume": 30
        },
        {
            "enabled": true,
            "id": "{455c0cb6-291f-4326-ba97-cd0e0d5adbf6}",
            "period": "weekend",
            "time": "18:04",
            "url": "http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk",
            "volume": 30
        }
    ],
    "InternetRadio": [
        {
            "id": "{a258d2f2-f36e-4620-9e1f-9d5f7875a747}",
            "name": "Deutschlandfunk Nova",
            "url": "http://st03.dlf.de/dlf/03/104/ogg/stream.ogg"
        },
        {
            "id": "{de2c79da-c250-4c78-a2db-5db398c0cbd9}",
            "name": "Radio FM4",
            "url": "https://fm4shoutcast.sf.apa.at"
        },
        {
            "id": "{0bad5cdd-4b4f-411b-929c-be9d634ba76a}",
            "name": "BBC Service",
            "url": "http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk"
        }
    ],
    "Podcasts": [
        {
            "id": "{b10f3ffc-51cb-4935-ae89-39a8dfb01ddd}",
            "title": "Alternativlos",
            "url": "https://alternativlos.org/alternativlos.rss"
        },
        {
            "id": "{e153f4b9-13a1-4313-a6f2-ed97cdce88a4}",
            "title": "Arms Control Wonk",
            "url": "http://armscontrolwonk.libsyn.com/rss"
        }
    ],
    "SleepTimeout": 60,
    "Version": "0.9.0",
    "Weather": {
        "apiKey": "xxx",
        "LocationId": "2928751"
    },
    "brightnessActive": 60,
    "brightnessStandby": 15,
    "volume": 30
}

```

## Environment Variables

This part is only works on the embedded linux build. There are a few
environment variables that allow for configuration of the actually used
input event interfaces.

Usually you don't need to set any of these variables if you use the device
tree provided during build with buildroot_extenal. For development purposes
the following variables are provided:

Setting the input event interfaces directly:
-   ``DR_PUSH_EVENT_PATH``  input device for push button events
     e.g. ``'/dev/input/event1'``

-   ``DR_ROTARY_EVENT_PATH``  input device for rotary encoder events
     e.g. ``'/dev/input/event2'`

Setting the input device names to look for using ``ioctl(EVIOCGNAME)``:
-   ``DR_PUSH_EVENT_NAME``  name of gpio-keys in device tree
     e.g. ``'gpio-keys'``
-   ``DR_ROTARY_EVENT_NAME`` name of rotary encoder in device tree
     e.g. ``'rotary_abs'``

If ``DR_PUSH_EVENT_PATH`` or ``DR_ROTARY_EVENT_PATH`` are set this
value is taken regardless if the device node exists or if it is
the *correct* device.

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
