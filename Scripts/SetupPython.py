import sys
import subprocess

def InstallPackage(packageName):        
        print(f"Installing {packageName} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])


InstallPackage("requests")
InstallPackage("pypiwin32")
InstallPackage("pyuac")