# pytest fixture for setup and teardown 

import pytest
import time
import sys
import subprocess
import digitalrooster
from digitalrooster import configuration
from digitalrooster.rest import ApiException

@pytest.fixture(scope="module")
def api_client():
    conf = digitalrooster.Configuration();
    conf.host="http://localhost:6666/api/1.0"
    sample = subprocess.Popen("restserver")
    time.sleep(0.1);
    with digitalrooster.ApiClient(conf) as client:
        yield  client
        # tear down code
        sample.terminate()
        
