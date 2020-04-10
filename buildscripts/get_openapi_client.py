#!/usr/bin/env python3

import requests
import json
from git import Repo
import os
import zipfile

request_headers={}
request_headers['Content-Type']="application/json"

def get_commit_hash():
    dir_path = os.path.dirname( os.path.dirname(os.path.realpath(__file__))+"/../")
    gitinfo=Repo(dir_path)
    return str(gitinfo.head.commit)

def build_document_url():
    openapi_yml_url = "https://raw.githubusercontent.com/truschival/DigitalRoosterGui/"
    openapi_yml_url += get_commit_hash()
    openapi_yml_url += "/REST/openapi.yml" 
    return openapi_yml_url

def request_client_module(document_url):   
    generator_options={}
    generator_options["generateSourceCodeOnly"]=True
    generator_options["packageName"]="digitalrooster"
    
    generator_request={}
    generator_request['openAPIUrl']=document_url
    generator_request['options'] = generator_options
    generator_request['spec']={}
    request_headers['Accept']="application/json"
    
    generator_api_url ="https://api.openapi-generator.tech/api/"
    generator_api_url += "gen/clients/python"

    req=requests.post(url=generator_api_url,
                      headers=request_headers,
                      data=json.dumps(generator_request))
    
    if req.status_code == requests.codes.ok:
        resp1=json.loads(req.text)
        return resp1['link']
    else:
        print(req.text)
        req.raise_for_status()
    

def download_client_module(link, filename="python-client.zip"):
    request_headers['Accept']="*/*"
    req=requests.get(url=link, headers=request_headers, stream=True)
    if req.status_code == requests.codes.ok:
        with open(filename, 'wb') as f:
            for blk in req:
                f.write(blk)
    else:
        print(req.text)
        req.raise_for_status()              

if __name__ == "__main__":
    link = request_client_module(build_document_url())
    download_client_module(link, "python-client.zip")
    with zipfile.ZipFile("python-client.zip","r") as zip:
        zip.extractall() 
