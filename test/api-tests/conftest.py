# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later
#

import pytest
import time
import os
import subprocess
import digitalrooster

def create_config(filename):
    with open(filename, 'w') as opened_file:
        opened_file.write("""
        {
        "Alarms": [
            {
                "enabled": true,
                "id": "8364287c-3036-4cd0-b243-0a4f5dd863ae",
                "period": "workdays",
                "time": "06:30",
                "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3",
                "volume": 25
            },
            {
                "enabled": false,
                "id": "30df096e-f776-404c-8619-3c4c2c4da212",
                "period": "once",
                "time": "07:00",
                "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3",
                "volume": 40
            },
            {
                "enabled": true,
                "id": "e0fcc26f-fc55-4346-ab08-c0673cbf6184",
                "period": "daily",
                "time": "14:20",
                "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3",
                "volume": 25
            }
        ],
        "InternetRadio": [
            {
                "id": "0a2152ef-da4f-4f26-8a77-078193da536e",
                "name": "BBC Radio 4",
                "url": "http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk"
            },
            {
                "id": "{09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d}",
                "name": "Deutschlandfunk",
                "url": "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3"
            }
        ],
        "Podcasts": [
            {
                "id": "{61f6bcda-0334-49a7-bfe9-b206b9f0bef7}",
                "title": "Alternativlos",
                "updateInterval": 3600,
                "url": "https://alternativlos.org/alternativlos.rss"
            },
            {
                "id": "{6d2ebae6-d961-411d-aecc-7820d1be1650}",
                "title": "Arms Control Wonk",
                "updateInterval": 1200,
                "url": "http://armscontrolwonk.libsyn.com/rss"
            }
        ],
        "Weather": {
            "API-Key": "a904431b4e0eae431bcc1e075c761abb",
            "locationID": "2928751",
            "updateInterval": 3600
        },
        "alarmTimeout": 30,
        "brightnessActive": 19,
        "brightnessStandby": 4,
        "sleepTimeout": 40,
        "version": "0.8.0",
        "volume": 25,
        "wpa_ctrl": "/var/run/wpa_supplicant/wlan0"
        }""")


@pytest.fixture(scope="function")
def api_client():
    conf = digitalrooster.Configuration();
    conf.host="http://localhost:6666/api/1.0"
    digitalrooster_json = '/tmp/api-tests.json'
    create_config(digitalrooster_json)
    os.system("/usr/bin/killall -SIGKILL restserver")
    time.sleep(0.2)
    restserver = subprocess.Popen(args=['restserver', '-c' , digitalrooster_json])
    time.sleep(0.3);
    with digitalrooster.ApiClient(conf) as client:
        yield  client
        # tear down code
        restserver.terminate()
