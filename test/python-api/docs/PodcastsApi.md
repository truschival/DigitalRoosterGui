# openapi_client.PodcastsApi

All URIs are relative to *http://0.0.0.0:8080/v1*

Method | HTTP request | Description
------------- | ------------- | -------------
[**podcasts_create**](PodcastsApi.md#podcasts_create) | **POST** /podcasts | Add a podcast source with RSS feed and add it to the list
[**podcasts_delete**](PodcastsApi.md#podcasts_delete) | **DELETE** /podcasts/{uid} | Delete Podcast from list
[**podcasts_read_all**](PodcastsApi.md#podcasts_read_all) | **GET** /podcasts | Read all podcast rss sources
[**podcasts_read_one**](PodcastsApi.md#podcasts_read_one) | **GET** /podcasts/{uid} | Read one Podcast identified by UUID


# **podcasts_create**
> podcasts_create(podcast)

Add a podcast source with RSS feed and add it to the list

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
    api_instance = openapi_client.PodcastsApi(api_client)
    podcast = openapi_client.Podcast() # Podcast | Podcast source to create

    try:
        # Add a podcast source with RSS feed and add it to the list
        api_instance.podcasts_create(podcast)
    except ApiException as e:
        print("Exception when calling PodcastsApi->podcasts_create: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **podcast** | [**Podcast**](Podcast.md)| Podcast source to create | 

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

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **podcasts_delete**
> podcasts_delete(uid)

Delete Podcast from list

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
    api_instance = openapi_client.PodcastsApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Delete Podcast from list
        api_instance.podcasts_delete(uid)
    except ApiException as e:
        print("Exception when calling PodcastsApi->podcasts_delete: %s\n" % e)
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

# **podcasts_read_all**
> list[Podcast] podcasts_read_all(length=length, offset=offset)

Read all podcast rss sources

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
    api_instance = openapi_client.PodcastsApi(api_client)
    length = 56 # int | number of items if omitted or invalid all remaining elements will be assumed (optional)
offset = 56 # int | offset from start of list if omitted or invalid 0 will be assumed (optional)

    try:
        # Read all podcast rss sources
        api_response = api_instance.podcasts_read_all(length=length, offset=offset)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling PodcastsApi->podcasts_read_all: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **length** | **int**| number of items if omitted or invalid all remaining elements will be assumed | [optional] 
 **offset** | **int**| offset from start of list if omitted or invalid 0 will be assumed | [optional] 

### Return type

[**list[Podcast]**](Podcast.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | successfully read list |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **podcasts_read_one**
> Podcast podcasts_read_one(uid)

Read one Podcast identified by UUID

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
    api_instance = openapi_client.PodcastsApi(api_client)
    uid = 'uid_example' # str | unique id to identify item

    try:
        # Read one Podcast identified by UUID
        api_response = api_instance.podcasts_read_one(uid)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling PodcastsApi->podcasts_read_one: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **uid** | **str**| unique id to identify item | 

### Return type

[**Podcast**](Podcast.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

### HTTP response details
| Status code | Description | Response headers |
|-------------|-------------|------------------|
**200** | Successfully read Podcast |  -  |
**401** | Unauthorized |  -  |
**403** | Invalid request parameters! |  -  |
**404** | The specified resource was not found |  -  |

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

