import gdown 
import os

#
# Third party libraries
#
#url = "https://drive.google.com/a/ucar.edu/uc?id=1sRlE06jSVrCScrt546G4UtI3hj93xV2W"
url = "https://drive.google.com/a/ucar.edu/uc?id=1NRMu4_g8ZXu9bILBVRDsuUKIGBiT2016"
output = "2019-Aug-Win32.zip"
gdown.download(url, output, quiet=False)

import zipfile
with zipfile.ZipFile("2019-Aug-Win32.zip", 'r') as zip_ref:
    zip_ref.extractall('C:\\')

#
# Download NSIS installer from google drive, until Nullsoft fixes their download certificate
#
url = "https://drive.google.com/a/ucar.edu/uc?id=1sRlE06jSVrCScrt546G4UtI3hj93xV2W"
#url = "https://drive.google.com/a/ucar.edu/uc?id=1YweSaUX0S08E8HEF1Mee2o5gei7WZj1a"
output = "nsis-3.06.1-setup.exe"
gdown.download(url, output, quiet=False)
#os.system("./nsis-3.06.1-setup.exe")
os.system("C:\\Users\\circleci\\project\\nsis-3.06.1-setup.exe /S /NCRC")
