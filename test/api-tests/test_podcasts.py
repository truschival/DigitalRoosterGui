# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later
#
import pytest
import digitalrooster
import json
from digitalrooster.rest import ApiException
from digitalrooster.apis.tags.podcasts_api import PodcastsApi
from digitalrooster.model.podcast import Podcast


def test_read_podcasts(api_client):
    c = PodcastsApi(api_client)
    r = c.podcasts_read_all().body
    assert len(r) == 2
    assert r[1]['id'] == '6d2ebae6-d961-411d-aecc-7820d1be1650'
    assert r[1]['title'] == 'Arms Control Wonk'
    assert r[1]['url'] == 'http://armscontrolwonk.libsyn.com/rss'
    assert r[1]['updateInterval'] == 1200


def test_get_podcast_by_id(api_client):
    c = PodcastsApi(api_client)
    params = {'id': '61f6bcda-0334-49a7-bfe9-b206b9f0bef7'}
    r = c.podcasts_read_one(path_params=params).body
    assert r['id'] == "61f6bcda-0334-49a7-bfe9-b206b9f0bef7"
    assert r['url'] == "https://alternativlos.org/alternativlos.rss"
    assert r['title'] == "Alternativlos"


def test_get_podcast_by_wrong_id(api_client):
    c = PodcastsApi(api_client)
    params = {'id': 'c0ffee00-404c-f776-8619-3c4c2c4da212'}
    with pytest.raises(ApiException) as apiexc:
        c.podcasts_read_one(path_params=params)
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'


def test_create_podcast_ok(api_client):
    c = PodcastsApi(api_client)
    podcast = Podcast(title="The Title", url="foo://bar.baz")
    x = c.podcasts_create(podcast)
    r = c.podcasts_read_all().body
    assert len(r) == 3


def test_create_podcast_max_episodes(api_client):
    c = PodcastsApi(api_client)
    podcast = Podcast(title="The Title",
                      url="http://armscontrolwonk.libsyn.com/rss",
                      maxEpisodes=5)
    x = c.podcasts_create(podcast).body
    params = {'id': x['id']}
    r = c.podcasts_read_one(path_params=params).body
    assert r['maxEpisodes'] == 5


def test_create_podcast_wrong_url(api_client):
    c = PodcastsApi(api_client)
    podcast = Podcast(url="Not An URL!",
                      title="Thomas")
    with pytest.raises(ApiException) as apiexc:
        c.podcasts_create(podcast)
    assert apiexc.value.status == 400


def test_delete_podcast(api_client):
    c = PodcastsApi(api_client)
    c.podcasts_delete(
        path_params={'id': "61f6bcda-0334-49a7-bfe9-b206b9f0bef7"})
    r = c.podcasts_read_all().body
    assert len(r) == 1


def test_delete_podcast_wrong_id(api_client):
    c = PodcastsApi(api_client)
    with pytest.raises(ApiException) as apiexc:
        c.podcasts_delete(
            path_params={'id': "c0ffee00-404c-f776-8619-3c4c2c4da212"})
    assert apiexc.value.status == 400
    msg = json.loads(apiexc.value.body)
    assert msg['code'] == 400
    assert msg['message'] == 'no item for this UUID'
    r = c.podcasts_read_all().body
    assert len(r) == 2
