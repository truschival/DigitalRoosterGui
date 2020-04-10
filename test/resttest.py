#!/usr/bin/env python3

import sys
import subprocess
import time
import digitalrooster
from digitalrooster import configuration
from digitalrooster.rest import ApiException
from pprint import pprint

conf = digitalrooster.Configuration();
conf.host="http://localhost:6666/api/1.0"
sample = subprocess.Popen("restserver")

time.sleep(1);
with digitalrooster.ApiClient(conf) as api_client:
    # Create an instance of the API class
    api_instance = digitalrooster.AlarmsApi(api_client)
    alarm = digitalrooster.Alarm(time="06:30",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
    try:
        # Create a new alarm entry and add it to the list
        api_instance.alarms_create(alarm)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_create: %s\n" % e)


sample.terminate()
