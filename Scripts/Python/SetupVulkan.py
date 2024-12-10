import os
import subprocess
from pathlib import Path
from urllib.request import urlopen
import Utils

# Inspired from Hazel's setup


def CheckVulkanSDK() -> bool:
    vulkanSDK = os.environ.get("VULKAN_SDK")
    if (vulkanSDK is None):
        print("Vulkan not found")
        InstallSDK()
    else:
        print(f"Found Vulkan at {vulkanSDK}")
        return True

def InstallSDK():
    url = f"https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe"
    print("Downloading Vulkan")
    scriptDir = os.path.dirname(os.path.abspath(__file__))
    vulkanPath = os.path.join(scriptDir, "VulkanSDK.exe")
    result = Utils.DownloadFile(url, vulkanPath)
    if True:
        print("Successfully downloaded Vulkan SDK")
        print("Installing Vulkan")
        command = [
            vulkanPath,
            "--accept-licenses",
            "--default-answer",
            "--confirm-command",
            "install",
            "com.lunarg.vulkan.sdl2",
            "com.lunarg.vulkan.glm",
            "com.lunarg.vulkan.vma"
        ]
        try:
            # The 'runas' command prompts for the admin password.
            subprocess.run(command, check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error occurred: {e}")

CheckVulkanSDK()
