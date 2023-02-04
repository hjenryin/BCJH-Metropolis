wget https://bcjh.xyz/api/download_data?id=62404 -O tmp 
jq ".data | fromjson" tmp > userData.json
rm tmp
