import requests
import json
import argparse
import base64
import time
import random
import hmac
from hashlib import sha256
from collections import OrderedDict
import urllib.parse

import _utils_


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', type=str, help='the address of the ehsm_kms_server', required=True)
    parser.add_argument('--mr_enclave', type=str, help='mr_enclave you will upload', required=True)
    parser.add_argument('--mr_signer', type=str, help='mr_signer you will upload', required=True)
    args = parser.parse_args()

    base_url = args.url + "/ehsm?Action="
    print(base_url)
    return base_url, args.mr_enclave, args.mr_signer

def uploadQuotePolicy(base_url, mr_enclave, mr_signer):
    payload = OrderedDict()
    payload["mr_enclave"] = mr_enclave
    payload["mr_signer"] = mr_signer
    params = _utils_.init_params(payload)
    print('uploadQuotePolicy req:\n%s\n' %(params))
    
    resp = requests.post(url=base_url + "UploadQuotePolicy", data=json.dumps(params), headers=_utils_.headers, verify=_utils_.use_secure_cert)
    if(_utils_.check_result(resp, 'UploadQuotePolicy') == False):
        return
    print('uploadQuotePolicy resp:\n%s\n' %(resp.text))

    policyId = json.loads(resp.text)['result']['policyId']
    return policyId

if __name__ == "__main__":
    headers = _utils_.headers

    base_url, mr_enclave, mr_signer = get_args()

    uploadQuotePolicy(base_url, mr_enclave, mr_signer)

