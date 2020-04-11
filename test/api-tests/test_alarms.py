
import pytest
import digitalrooster
from digitalrooster import configuration, api_client
from digitalrooster.rest import ApiException


def test_read_alarms(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all()
    assert r[0].id == '8364287c-3036-4cd0-b243-0a4f5dd863ae'
    assert r[0].period == 'workdays'
    assert r[0].url == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert r[0].enabled == True

def test_get_alarm_by_id(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_one("30df096e-f776-404c-8619-3c4c2c4da212")
    assert r.id == "30df096e-f776-404c-8619-3c4c2c4da212"
    assert r.period == 'once'
    assert r.url == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert r.enabled == False


def test_create_alarm_ok(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     alarm = digitalrooster.Alarm(time="06:30",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
     assert c.alarms_create(alarm) == None
     r= c.alarms_read_all()
     assert len(r) == 3 


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

