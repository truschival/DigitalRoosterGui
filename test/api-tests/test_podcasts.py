import pytest
import digitalrooster
import json
from digitalrooster import api_client
from digitalrooster.rest import ApiException

def test_read_podcasts(api_client):
    c = digitalrooster.PodcastsApi(api_client)
    r= c.podcasts_read_all()
    assert len(r) == 2
    assert r[1].id == '6d2ebae6-d961-411d-aecc-7820d1be1650'
    assert r[1].title == 'Arms Control Wonk'
    assert r[1].url == 'http://armscontrolwonk.libsyn.com/rss'
    assert r[1].update_interval == 1200

def test_get_podcast_by_id(api_client):
    c = digitalrooster.PodcastsApi(api_client)
    r=c.podcasts_read_one("61f6bcda-0334-49a7-bfe9-b206b9f0bef7")
    assert r.id == "61f6bcda-0334-49a7-bfe9-b206b9f0bef7"
    assert r.url == "https://alternativlos.org/alternativlos.rss"
    assert r.title == "Alternativlos"

def test_get_podcast_by_wrong_id(api_client):
    c = digitalrooster.PodcastsApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        r= c.podcasts_read_one("c0ffee00-404c-f776-8619-3c4c2c4da212")
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'


def test_create_podcast_ok(api_client):
     c = digitalrooster.PodcastsApi(api_client)
     podcast = digitalrooster.Podcast(title="The Title", url="foo://bar.baz")
     x= c.podcasts_create(podcast)
     r= c.podcasts_read_all()
     assert len(r) == 3

     
def test_create_podcast_wrong_url(api_client):
     c = digitalrooster.PodcastsApi(api_client)
     podcast = digitalrooster.Podcast(url="Not An URL!",
                                 title="Thomas")
     with pytest.raises(ApiException) as apiexc:
         c.podcasts_create(podcast)
     assert apiexc.value.status == 400
     
    
def test_delete_podcast(api_client):
     c = digitalrooster.PodcastsApi(api_client)
     c.podcasts_delete("61f6bcda-0334-49a7-bfe9-b206b9f0bef7")
     r= c.podcasts_read_all()
     assert len(r) == 1
      
def test_delete_podcast_wrong_id(api_client):
     c = digitalrooster.PodcastsApi(api_client)
     with pytest.raises(ApiException) as apiexc:
         c.podcasts_delete("c0ffee00-404c-f776-8619-3c4c2c4da212")
     assert apiexc.value.status == 400
     msg = json.loads(apiexc.value.body)
     assert msg['code'] == 400
     assert msg['message'] == 'no item for this UUID'
     r= c.podcasts_read_all()
     assert len(r) == 2
     
