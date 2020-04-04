#!/usr/bin/env python3

import sys
import subprocess
import time
import openapi_client
from openapi_client import configuration
from openapi_client.rest import ApiException
from pprint import pprint

conf = openapi_client.Configuration();
conf.host="http://localhost:6666/api/1.0"
sample = subprocess.Popen("restserver")

time.sleep(1);
with openapi_client.ApiClient(conf) as api_client:
    # Create an instance of the API class
    api_instance = openapi_client.AlarmsApi(api_client)
    alarm = openapi_client.Alarm(time="06:30",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
    try:
        # Create a new alarm entry and add it to the list
        api_instance.alarms_create(alarm)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_create: %s\n" % e)


sample.terminate()
