#!/bin/bash

# Colors
green='\e[32m'
red='\e[31m'
blue='\e[34m'
reset='\e[0m'

# Define servers in an array (host, port, location, upload_dir)
servers=(
	"Server1 127.0.0.1 8082 http://127.0.0.1:8082 LAME_UPLOAD"
	"Server2 127.0.0.2 8081 http://127.0.0.2:8081 BADASS_UPLOAD"
	"Server3 127.0.0.2 8081 http://127.0.0.2:8081 TEGE_UPLOAD"
	"Server4 127.0.0.3 8080 http://127.0.0.3:8080 TIGHT_UPLOAD"
)

# Function to run curl commands
run_curl() {
	local url=$1
	local method=$2
	local data=$3
	local flag=$4

	if [ "$method" == "GET" ]; then
		curl -s -o /dev/null "$url"
	elif [ "$method" == "POST" ]; then
		if [[ "$data" == *"@"* ]]; then
			curl -s -o /dev/null -X POST -F "$data" "$url"
		else
			curl -s -o /dev/null -X POST -d "$data" "$url"
		fi
	elif [ "$method" == "DELETE" ]; then
		if [ -n "$data" ]; then
			curl -s -o /dev/null -X DELETE -d "$data" "$url"
		else
			curl -s -o /dev/null -X DELETE "$url"
		fi
	else
		curl -s -o /dev/null -X "$method" "$url"
	fi

	if [ $? -eq 0 ]; then
		echo -e "${green}✓ SUCCESS${reset}: $method $url"
	else
		echo -e "${red}✗ FAIL${reset}: $method $url"
	fi
}

# Loop through each server
for server in "${servers[@]}"; do
	read -r name host port location upload_on <<< "$server"

	echo -e "\n${blue}Testing $name: $location${reset}"

    # Valid requests
    run_curl "$location" "GET"
    run_curl "$location/index.html" "GET"
    run_curl "$location/kaamelott.html" "GET"
    run_curl "$location/cgi/astro.php" "POST" "sign=Leo"
    run_curl "$location/index.html" "DELETE"
    run_curl "$location/index.html" "POST" "file=@www/cgi/random_number.cpp"
    run_curl "$location/index.html" "POST" "file=@www/cgi/kaamelott.js"
    run_curl "$location/delete.html" "DELETE" "file=www/$upload_on/random_number.cpp"

    # Error requests
    run_curl "$location/cgi/astro.php" "POST" "sign=NOSIGN"
    run_curl "$location/cgi/astro.php" "POSTIIIIII" "sign=Leo"
    run_curl "$location/index.html" "DELETE"
    run_curl "$location/delete/$upload_on/loveeeeeeeeeers.jpeg" "DELETE"
done

