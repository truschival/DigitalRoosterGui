# openapi_client.InternetRadioApi

All URIs are relative to *http://0.0.0.0:8080/v1*

Method | HTTP request | Description
------------- | ------------- | -------------
[**streams_create**](InternetRadioApi.md#streams_create) | **POST** /radios | Create a station info and add it to the list
[**streams_delete**](InternetRadioApi.md#streams_delete) | **DELETE** /radios/{uid} | Delete Internet radio station from list
[**streams_read_all**](InternetRadioApi.md#streams_read_all) | **GET** /radios | Read all Internet radio streams
[**streams_read_one**](InternetRadioApi.md#streams_read_one) | **GET** /radios/{uid} | Read one Internet radio station


# **streams_create**
> streams_create(station)

Create a station info and add it to the list

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
    api_instance = openapi_client.InternetRadioApi(api_client)
    station = openapi_client.Station() # Station | Internet radio station to create

    try:
        # Create a station info and add it to the list
        api_instance.streams_create(station)
    except ApiException as e:
        print("Exception when calling InternetRadioApi->streams_create: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **station** | [**Station**](Station.md)| Internet radio station to create | 

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

# **streams_delete**
> streams_delete(uid)

Delete Internet radio station from list

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
    api_instance = openapi_client.InternetRadioApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Delete Internet radio station from list
        api_instance.streams_delete(uid)
    except ApiException as e:
        print("Exception when calling InternetRadioApi->streams_delete: %s\n" % e)
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
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **streams_read_all**
> list[Station] streams_read_all(length=length, offset=offset)

Read all Internet radio streams

returns a list of Internet radio streams

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
    api_instance = openapi_client.InternetRadioApi(api_client)
    length = 56 # int | number of items if omitted or invalid all remaining elements will be assumed (optional)
offset = 56 # int | offset from start of list if omitted or invalid 0 will be assumed (optional)

    try:
        # Read all Internet radio streams
        api_response = api_instance.streams_read_all(length=length, offset=offset)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling InternetRadioApi->streams_read_all: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **length** | **int**| number of items if omitted or invalid all remaining elements will be assumed | [optional] 
 **offset** | **int**| offset from start of list if omitted or invalid 0 will be assumed | [optional] 

### Return type

[**list[Station]**](Station.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Successfully read stream list |  -  |
**401** | Unauthorized |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **streams_read_one**
> Station streams_read_one(uid)

Read one Internet radio station

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
    api_instance = openapi_client.InternetRadioApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Read one Internet radio station
        api_response = api_instance.streams_read_one(uid)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling InternetRadioApi->streams_read_one: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **uid** | **str**| unique id to identify item | 

### Return type

[**Station**](Station.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Successfully read a single radio station |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

