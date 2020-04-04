# coding: utf-8

"""
    DigitalRooster

    Open API for Digital Rooster  # noqa: E501

    The version of the OpenAPI document: 0.8.0
    Generated by: https://openapi-generator.tech
"""


from __future__ import absolute_import

import unittest

import openapi_client
from openapi_client.api.alarms_api import AlarmsApi  # noqa: E501
from openapi_client.rest import ApiException


class TestAlarmsApi(unittest.TestCase):
    """AlarmsApi unit test stubs"""

    def setUp(self):
        self.api = openapi_client.api.alarms_api.AlarmsApi()  # noqa: E501

    def tearDown(self):
        pass

    def test_alarms_create(self):
        """Test case for alarms_create

        Create a new alarm entry and add it to the list  # noqa: E501
        """
        pass

    def test_alarms_delete(self):
        """Test case for alarms_delete

        Delete alarm from list  # noqa: E501
        """
        pass

    def test_alarms_read_all(self):
        """Test case for alarms_read_all

        Read all Alarms  # noqa: E501
        """
        pass

    def test_alarms_read_one(self):
        """Test case for alarms_read_one

        Read one Alarm  # noqa: E501
        """
        pass


if __name__ == '__main__':
    unittest.main()
