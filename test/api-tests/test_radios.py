# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later
#
import pytest
import digitalrooster
import json
from digitalrooster.rest import ApiException
from digitalrooster.apis.tags.radios_api import RadiosApi
from digitalrooster.model.station import Station


def test_read_stations(api_client):
    c = RadiosApi(api_client)
    r = c.iradio_read_all().body
    assert len(r) == 2
    assert r[0]['id'] == '0a2152ef-da4f-4f26-8a77-078193da536e'
    assert r[0]['name'] == 'BBC Radio 4'
    assert r[0]['url'] == 'http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk'


def test_read_stations_negative_offset(api_client):
    c = RadiosApi(api_client)
    params = {'offset': -1}
    r = c.iradio_read_all(query_params=params).body
    assert len(r) == 2


def test_get_station_by_id(api_client):
    c = RadiosApi(api_client)
    r = c.iradio_read_one(
        path_params={'id': "09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d"}).body
    assert r['id'] == "09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d"
    assert r['url'] == "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3"
    assert r['name'] == "Deutschlandfunk"


def test_get_station_by_wrong_id(api_client):
    c = RadiosApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        c.iradio_read_one(
            path_params={'id': "c0ffee00-404c-f776-8619-3c4c2c4da212"})
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'


def test_create_station_ok(api_client):
    c = RadiosApi(api_client)
    station = Station(name="AName", url="foo://bar.baz")
    c.iradio_create(station)
    r = c.iradio_read_all().body
    assert len(r) == 3


def test_create_station_wrong_url(api_client):
    c = RadiosApi(api_client)
    station = Station(url="Not : # üß !an.url/",
                      name="Thomas")
    with pytest.raises(ApiException) as apiexc:
        c.iradio_create(station)
    assert apiexc.value.status == 400


def test_delete_station(api_client):
    c = RadiosApi(api_client)
    c.iradio_delete(path_params={'id': "09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d"})
    r = c.iradio_read_all().body
    assert len(r) == 1


def test_delete_station_wrong_id(api_client):
    c = RadiosApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        c.iradio_delete(
            path_params={'id': "c0ffee00-404c-f776-8619-3c4c2c4da212"})
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'
    r = c.iradio_read_all().body
    assert len(r) == 2
