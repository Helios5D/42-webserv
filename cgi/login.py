#!/usr/bin/python3
import os
import sys
import urllib.parse

input_data = sys.stdin.read()
params = urllib.parse.parse_qs(input_data)

username = params.get('username', [None])[0]

body = f"You are succesfully logged in, {username}!"

content_length = len(body)

print(f"Set-Cookie: username={username}; path=/\r")
print(f"Content-Length: {content_length}\r")
print("Content-Type: text/plain\r\n\r")
print(body)