#!/bin/bash

# Exit when any command fails
# set -ex

# Colors variables
green='\e[32m'
red='\e[31m'
blue='\e[34m'
reset='\e[0m'

# Server1 variables
server1_host='127.0.0.1'
server1_port='8082'
server1_location='http://127.0.0.1:8082'
server1_upload_on='LAME_UPLOAD'

# Server2 variables
server2_host='127.0.0.2'
server2_port='8082'
server2_location='http://127.0.0.2:8081'
server2_upload_on='LAME_UPLOAD'


# Echo an error message before exiting
if [ ! "$0" ]; then
	trap 'printf "\"${red}${last_command}${reset}\" command failed with exit code $?."' EXIT
else
	trap 'printf "${green}success${reset}\n"' EXIT
fi

curl "$server1_location"
curl --resolve index.html:"$server1_port":"$server1_host" http://index.html:"$server1_port"/
curl "$server1_location"/kaamelott.html
curl -X POST -d "sign=Leo" "$server1_location"/cgi/astro.php

# ERREUR
curl -X POST -d "sign=NOSIGN" "$server1_location"/cgi/astro.php
curl -X POSTIIIIII -d "sign=Leo" "$server1_location"/cgi/astro.php
curl "$server1_location"/index.html -X DELETE
curl "$server1_location"/delete/"${server1_upload_on}"/loveeeeeeeeeers.jpeg -X DELETE

# server2
curl "$server2_location"
curl --resolve index.html:"$server2_port":"$server2_host" http://index.html:"$server2_port"/
curl "$server2_location"/kaamelott.html
curl -X POST -d "sign=Leo" "$server2_location"/cgi/astro.php

# ERREUR
curl -X POST -d "sign=NOSIGN" "$server2_location"/cgi/astro.php
curl -X POSTIIIIII -d "sign=Leo" "$server2_location"/cgi/astro.php
curl "$server2_location"/index.html -X DELETE
curl "$server2_location"/delete/"${server2_upload_on}"/loveeeeeeeeeers.jpeg -X DELETE
