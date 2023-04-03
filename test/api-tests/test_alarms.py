# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later
#
import pytest
import digitalrooster
import json

import pprint
import sys
import digitalrooster
from digitalrooster.apis.tags.alarms_api import AlarmsApi
from digitalrooster.models import *
from digitalrooster.rest import ApiException


def test_read_alarms(api_client):
    c = AlarmsApi(api_client)
    alarm_list = c.alarms_read_all().body
    assert len(alarm_list) == 3
    assert alarm_list[0]['id'] == '8364287c-3036-4cd0-b243-0a4f5dd863ae'
    assert alarm_list[0]['period'] == 'workdays'
    assert alarm_list[0]['url'] == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert alarm_list[0]['enabled'].TRUE


def test_read_alarms_w_offset(api_client):
    client = AlarmsApi(api_client)
    query_params = {
        'offset': 1,
    }
    response = client.alarms_read_all(query_params=query_params)
    assert len(response.body) == 2
    assert response.body[0]['id'] == '30df096e-f776-404c-8619-3c4c2c4da212'


def test_read_alarms_w_offset_length(api_client):
    client = AlarmsApi(api_client)
    response = client.alarms_read_all(query_params={'offset': 1, 'length': 1})
    assert len(response.body) == 1
    assert response.body[0]['id'] == '30df096e-f776-404c-8619-3c4c2c4da212'


def test_read_alarms_w_offset_invalid(api_client):
    c = AlarmsApi(api_client)
    r = c.alarms_read_all(query_params={'offset': 4}).body
    assert len(r) == 0


def test_read_alarms_w_length_invalid(api_client):
    c = AlarmsApi(api_client)
    r = c.alarms_read_all(query_params={'length': 8}).body
    assert len(r) == 3


def test_get_alarm_by_id(api_client):
    c = AlarmsApi(api_client)
    r = c.alarms_read_one(
        path_params={'id': "30df096e-f776-404c-8619-3c4c2c4da212"}).body
    assert r['id'] == "30df096e-f776-404c-8619-3c4c2c4da212"
    assert r['period'] == 'once'
    assert r['url'] == 'http://st01.dlf.de/dlf/01/128/mp3/stream.mp3'
    assert r['enabled'].TRUE


def test_get_alarm_by_wrong_id(api_client):
    c = AlarmsApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        c.alarms_read_one(
            path_params={'id': "c0ffee00-404c-f776-8619-3c4c2c4da212"})
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] != ''


def test_create_alarm_ok(api_client):
    c = AlarmsApi(api_client)
    alarm = Alarm(time="06:30",
                  period="daily",
                  url="http://foo.bar/baz.mp3",
                  enabled=True)
    x = c.alarms_create(alarm)
    assert x
    r = c.alarms_read_all().body
    assert len(r) == 4


def test_create_alarm_with_uuid(api_client):
    c = AlarmsApi(api_client)
    uid = "c0ffee00-3964-4712-83cf-11e8423e7a99"
    alarm = Alarm(id=uid,
                  time="07:45",
                  period="once",
                  url="http://foo.bar/baz.mp3",
                  enabled=True)
    x = c.alarms_create(alarm).body
    assert x['id'] == uid
    r = c.alarms_read_all().body
    assert len(r) == 4


def test_create_alarm_wo_time_raises(api_client):
    c = AlarmsApi(api_client)
    alarm = Alarm(time="",
                  period="once",
                  url="http://foo.bar/baz.mp3",
                  enabled=True)
    with pytest.raises(ApiException) as apiexc:
        c.alarms_create(alarm)
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] != ''


def test_delete_created_alarm(api_client):
    c = AlarmsApi(api_client)
    alarm = Alarm(time="12:45",
                  period="daily",
                  url="http://foo.bar/baz.mp3",
                  enabled=False)
    x = c.alarms_create(alarm).body
    r = c.alarms_read_all().body
    assert len(r) == 4
    c.alarms_delete(path_params={'id': x['id']})
    r = c.alarms_read_all().body
    assert len(r) == 3


def test_delete_invalid_alarm(api_client):
    c = AlarmsApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        c.alarms_delete(
            path_params={'id': "c0ffee00-404c-f776-8619-3c4c2c4da212"})
    assert apiexc.value.status == 400
