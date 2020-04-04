# openapi_client.AlarmsApi

All URIs are relative to *http://0.0.0.0:8080/v1*

Method | HTTP request | Description
------------- | ------------- | -------------
[**alarms_create**](AlarmsApi.md#alarms_create) | **POST** /alarms | Create a new alarm entry and add it to the list
[**alarms_delete**](AlarmsApi.md#alarms_delete) | **DELETE** /alarms/{uid} | Delete alarm from list
[**alarms_read_all**](AlarmsApi.md#alarms_read_all) | **GET** /alarms | Read all Alarms
[**alarms_read_one**](AlarmsApi.md#alarms_read_one) | **GET** /alarms/{uid} | Read one Alarm


# **alarms_create**
> alarms_create(alarm)

Create a new alarm entry and add it to the list

### Example

```python
from __future__ import print_function
import time
import openapi_client
from openapi_client.rest import ApiException
from pprint import pprint

# Enter a context with an instance of the API client
with openapi_client.ApiClient() as api_client:
    # Create an instance of the API class
    api_instance = openapi_client.AlarmsApi(api_client)
    alarm = openapi_client.Alarm() # Alarm | Alarm object to create

    try:
        # Create a new alarm entry and add it to the list
        api_instance.alarms_create(alarm)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_create: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **alarm** | [**Alarm**](Alarm.md)| Alarm object to create | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Request processed with success! |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **alarms_delete**
> alarms_delete(uid)

Delete alarm from list

### Example

```python
from __future__ import print_function
import time
import openapi_client
from openapi_client.rest import ApiException
from pprint import pprint

# Enter a context with an instance of the API client
with openapi_client.ApiClient() as api_client:
    # Create an instance of the API class
    api_instance = openapi_client.AlarmsApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Delete alarm from list
        api_instance.alarms_delete(uid)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_delete: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **uid** | **str**| unique id to identify item | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Request processed with success! |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **alarms_read_all**
> list[Alarm] alarms_read_all(length=length, offset=offset)

Read all Alarms

### Example

```python
from __future__ import print_function
import time
import openapi_client
from openapi_client.rest import ApiException
from pprint import pprint

# Enter a context with an instance of the API client
with openapi_client.ApiClient() as api_client:
    # Create an instance of the API class
    api_instance = openapi_client.AlarmsApi(api_client)
    length = 56 # int | number of items if omitted or invalid all remaining elements will be assumed (optional)
offset = 56 # int | offset from start of list if omitted or invalid 0 will be assumed (optional)

    try:
        # Read all Alarms
        api_response = api_instance.alarms_read_all(length=length, offset=offset)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_read_all: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **length** | **int**| number of items if omitted or invalid all remaining elements will be assumed | [optional] 
 **offset** | **int**| offset from start of list if omitted or invalid 0 will be assumed | [optional] 

### Return type

[**list[Alarm]**](Alarm.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Successfully read alarm list |  -  |
**404** | Wrong length/offset |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **alarms_read_one**
> Alarm alarms_read_one(uid)

Read one Alarm

### Example

```python
from __future__ import print_function
import time
import openapi_client
from openapi_client.rest import ApiException
from pprint import pprint

# Enter a context with an instance of the API client
with openapi_client.ApiClient() as api_client:
    # Create an instance of the API class
    api_instance = openapi_client.AlarmsApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Read one Alarm
        api_response = api_instance.alarms_read_one(uid)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling AlarmsApi->alarms_read_one: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **uid** | **str**| unique id to identify item | 

### Return type

[**Alarm**](Alarm.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Successfully read Alarm |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

