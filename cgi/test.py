#!/usr/bin/python3

body = f"""
<!DOCTYPE html>
<html>
<head>
<title>Test CGI</title>
<body>
<h2>I guess i'm the output of a CGI huh ?</h2>
</body>
</html>
"""

content_length = len(body)

print(f"Content-Length: {content_length}\r")
print ("Content-Type: text/html\r\n\r")
print(body)
