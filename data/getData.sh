wget https://bcjh.xyz/api/download_data?id=74877 -O tmp 
jq ".data | fromjson" tmp > userData.json
rm tmp
