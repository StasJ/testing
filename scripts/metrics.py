base  = "https://api.github.com"
owner = "NCAR"
repo  = "VAPOR"
tag   = "3.2.0"
release_id = "2195"
asset = "download_count"

idTag = "id"


import urllib
import requests

def GetRepoIds( repoIds ):
    query = '/'.join([base, "repos", owner, repo, "releases"])

    myJson = requests.get( query ).json()

    for key in myJson:
        repoIds.append( key[ idTag ] );

def GetDownloadCount( repoId ):
    query = '/'.join([base, "repos", owner, repo, "releases", str(repoId)])
    myJson = requests.get( query ).json()
    
    
    print (myJson["assets"][0]["download_count"])

    #print( myJson[asset] )

#header = {'Authorization': 'token b0c076b78e002e1f475d644f4fdf9460e83521b4'}
token = 'token b0c076b78e002e1f475d644f4fdf9460e83521b4'
requests.get(base, auth=("sgpearse",token))

repoIds = []
GetRepoIds( repoIds )
for repoId in repoIds:
    GetDownloadCount( repoId )

print( repoIds)
#print( requests.get( query ).json()[0].keys() )
#print( requests.get( query ).json()[0]["id"] )

