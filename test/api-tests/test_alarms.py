
import pytest
import digitalrooster
from digitalrooster import configuration
from digitalrooster.rest import ApiException

def test_create_alarm_ok(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     alarm = digitalrooster.Alarm(time="06:30",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
     assert c.alarms_create(alarm) == None


def test_create_alarm_wo_time_raises(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     alarm = digitalrooster.Alarm(time="",
                                  period="once",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
     with pytest.raises(ApiException) as apiexc:
         c.alarms_create(alarm)   
     assert apiexc.value.status == 400
     assert str(apiexc.value.body) == "Alarm Time invalid!"
     