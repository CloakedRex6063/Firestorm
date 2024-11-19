import sys
import os
import winreg

import requests
import time
import urllib

def DownloadFile(url, path):
    try:
        # Send a GET request to the URL
        response = requests.get(url)

        # Check if the request was successful (status code 200)
        if response.status_code == 200:
            # Open the file in write-binary mode and save the content
            with open(path, 'wb') as file:
                file.write(response.content)
            print(f"File downloaded successfully and saved as {path}.")
            return True
        else:
            print(f"Failed to download file. Status code: {response.status_code}")
            return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False