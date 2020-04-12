import pytest
import digitalrooster
import json
from digitalrooster import api_client
from digitalrooster.rest import ApiException

def test_read_stations(api_client):
    c = digitalrooster.InternetRadioApi(api_client)
    r= c.iradio_read_all()
    assert len(r) == 2
    assert r[0].id == '0a2152ef-da4f-4f26-8a77-078193da536e'
    assert r[0].name == 'BBC Radio 4'
    assert r[0].url == 'http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk'


def test_read_stations_negative_offset(api_client):
    c = digitalrooster.InternetRadioApi(api_client)
    r= c.iradio_read_all(offset=-1)
    assert len(r) == 2

def test_get_station_by_id(api_client):
    c = digitalrooster.InternetRadioApi(api_client)
    r=c.iradio_read_one("09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d")
    assert r.id == "09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d"
    assert r.url == "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3"
    assert r.name == "Deutschlandfunk"


def test_get_station_by_wrong_id(api_client):
    c = digitalrooster.InternetRadioApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        r= c.iradio_read_one("c0ffee00-404c-f776-8619-3c4c2c4da212")
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'


def test_create_station_ok(api_client):
     c = digitalrooster.InternetRadioApi(api_client)
     station = digitalrooster.Station(name="AName", url="foo://bar.baz")
     x= c.iradio_create(station)
     r= c.iradio_read_all()
     assert len(r) == 3

     
def test_create_station_wrong_url(api_client):
     c = digitalrooster.InternetRadioApi(api_client)
     station = digitalrooster.Station(url="Not : # üß !an.url/",
                                 name="Thomas")
     with pytest.raises(ApiException) as apiexc:
         c.iradio_create(station)
     assert apiexc.value.status == 400
     
    
def test_delete_station(api_client):
     c = digitalrooster.InternetRadioApi(api_client)
     c.iradio_delete("09be8e85-a9d3-4db8-b2c5-02e3eb3ff66d")
     r= c.iradio_read_all()
     assert len(r) == 1
      
def test_delete_station_wrong_id(api_client):
     c = digitalrooster.InternetRadioApi(api_client)
     with pytest.raises(ApiException) as apiexc:
         c.iradio_delete("c0ffee00-404c-f776-8619-3c4c2c4da212")
     assert apiexc.value.status == 400
     msg = json.loads(apiexc.value.body)
     assert msg['code'] == 400
     assert msg['message'] == 'no item for this UUID'
     r= c.iradio_read_all()
     assert len(r) == 2
