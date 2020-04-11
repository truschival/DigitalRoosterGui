import pytest
import digitalrooster
from digitalrooster import configuration, api_client
from digitalrooster.rest import ApiException


def test_read_alarms(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all()
    assert len(r) == 3
    assert r[0].id == '8364287c-3036-4cd0-b243-0a4f5dd863ae'
    assert r[0].period == 'workdays'
    assert r[0].url == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert r[0].enabled == True

def test_read_alarms_w_offset(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all(offset=1)
    assert len(r) == 2
    assert r[0].id == '30df096e-f776-404c-8619-3c4c2c4da212'

def test_read_alarms_w_offset_length(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all(offset=1,length=1)
    assert len(r) == 1
    assert r[0].id == '30df096e-f776-404c-8619-3c4c2c4da212'

def test_read_alarms_w_offset_invalid(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all(offset=4)
    assert len(r) == 0

def test_read_alarms_w_length_invalid(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_all(length=8)
    assert len(r) == 3


def test_get_alarm_by_id(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    r= c.alarms_read_one("30df096e-f776-404c-8619-3c4c2c4da212")
    assert r.id == "30df096e-f776-404c-8619-3c4c2c4da212"
    assert r.period == 'once'
    assert r.url == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert r.enabled == False


def test_get_alarm_by_wrong_id(api_client):
    c = digitalrooster.AlarmsApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        r= c.alarms_read_one("c0ffee00-404c-f776-8619-3c4c2c4da212")
    assert apiexc.value.status == 400


def test_create_alarm_ok(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     alarm = digitalrooster.Alarm(time="06:30",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
     x= c.alarms_create(alarm)
     assert x
     r= c.alarms_read_all()
     assert len(r) == 4


def test_create_alarm_with_uuid(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     uid = "c0ffee00-3964-4712-83cf-11e8423e7a99"
     alarm = digitalrooster.Alarm(id=uid,
                                 time="07:45",
                                 period="once",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=True)
     x= c.alarms_create(alarm)
     assert x.id == uid
     r= c.alarms_read_all()
     assert len(r) == 4


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


def test_delete_created_alarm(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     alarm = digitalrooster.Alarm(time="12:45",
                                 period="daily",
                                 url="http://foo.bar/baz.mp3",
                                 enabled=False)
     x= c.alarms_create(alarm)
     r= c.alarms_read_all()
     assert len(r) == 4
     c.alarms_delete(x.id)
     r= c.alarms_read_all()
     assert len(r) == 3


def test_delete_invalid_alarm(api_client):
     c = digitalrooster.AlarmsApi(api_client)
     with pytest.raises(ApiException) as apiexc:
        r= c.alarms_delete("c0ffee00-404c-f776-8619-3c4c2c4da212")
     assert apiexc.value.status == 400
