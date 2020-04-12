# Digital Rooster REST API

DigitalRooster implements a REST configuration API. This API is not secured.
There is __no authentication__ and __no TLS encryption__ yet!

Make sure no attacker has access to the network of your DigitalRooster or
they might change your radio stations and alarms. You will be late to work
and lose your job!

The REST API however is pretty convienient if you do not want to add
radio stations using a text editor. Hopefully most errors are handled and
the data integrity is maintained.

You can conveniently list, add and delete alarms, Internet radio stations
and Podcast RSS streams. The default endpoint is
``http://<your_ip>:6666/api/1.0/``

The Api is specified in [REST/openapi.yml](../REST/openapi.yml) using the
swagger/openapi specification language.
To browse it conveniently load the ``openapi.yml`` in the
online [swagger editor](https://editor.swagger.io/) this will give you an
overview and ``curl`` examples how to call the API.

## Example for using the REST API with curl

The resouces at ``http://<your_ip>:6666/api/1.0/`` are ``alarms``, ``radios``
and ``podcasts``. All work in a similar manner and support the same
methods: ``GET``, ``POST``, ``DELETE`` - there is no ``PUT`` to update a
resource.
<sup><sub>
The resources are small and simple an I rather delete and recreate it than create
 an update function that deals with all the consistency and thread safety issues.
</sub></sup>

### List
Read all radio stations just call:

```
curl -V -X GET http://<your_ip>:6666/api/1.0/radios
```

You can also retrieve a subset with ``offset`` and ``length`` query parameters.

```
curl -V -X GET http://<your_ip>:6666/api/1.0/radios?offset=1&length=1
```

This will give you maximum of length entries starting at offset.


### Create a resource
To create a radio station (very simple object) only the URL and a name is required.
Please refer the the openapi spec what properties are required for the individual
resources.

```
    curl -X POST localhost:6666/api/1.0/radios/ \
    -d "{\"url\":\"http://foo.bar\", \"name\":\"FooRadio\"}"
```
It will return you the Id of the created resource:

```
{
    "id": "ce0087c7-97ff-4794-b54a-31e137abb738"
}
```

### Get a resource by id
To read an individual ressource.

```
curl -X GET \
	http://localhost:6666/api/1.0/radios/ce0087c7-97ff-4794-b54a-31e137abb738
```

### Delete a resource

Using the Id of the ressource you can delete it.

```
	curl -X DELETE \
	http://localhost:6666/api/1.0/radios/ce0087c7-97ff-4794-b54a-31e137abb738
```


## Creating an REST client from the specification

Using ``curl`` works but for front-ends it is nice to generate a client.
The api is tested using an online generated python client.
To generate a client in your language use the generator form
[openapi-generator.tech](https://openapi-generator.tech/).
There a client generators for tons of languages.

You can run it in a docker container (example generates a Python client):

```
    cd DigitalRoosterGui
	docker pull openapitools/openapi-generator-cli
	docker run -v $(pwd):/local -v $(pwd)/REST/:/input openapitools/openapi-generator-cli generate \
	       -i /input/openapi.yml -g python -o local/ -p packageName=digitalrooster

```

Or have a look at [buildscripts/get_openapi_client.py](../buildscripts/get_openapi_client.py)

This script uploads the openapi spec to the web service which generates a python
client for test. The scripts in [test/api-tests](../test/api-tests) use the generated
code to abstact the HTTP calls and error handling.
