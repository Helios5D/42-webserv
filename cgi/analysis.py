#!/usr/bin/python3
import os
import sys
from datetime import datetime, date
import urllib.parse

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length)
form = urllib.parse.parse_qs(post_data)

first_name = os.getenv('first_name')
last_name = os.getenv('last_name')
gender = os.getenv('gender')

birthdate_str = os.getenv('birthdate')
birthdate = datetime.strptime(birthdate_str, '%Y-%m-%d').date()

height = float(os.getenv('height'))
weight = float(os.getenv('weight'))
activity = os.getenv('activity')

today = date.today()
age = today.year - birthdate.year - ((today.month, today.day) < (birthdate.month, birthdate.day))
age_days = (today - birthdate).days
age_weeks = age_days // 7
age_months = (today.year - birthdate.year) * 12 + today.month - birthdate.month

bmi = weight / ((height / 100) ** 2)

if gender == "male":
	bmr = 88.362 + (13.397 * weight) + (4.799 * height) - (5.677 * age)
else:
	bmr = 447.593 + (9.247 * weight) + (3.098 * height) - (4.330 * age)

tdee_multipliers = {
	"sedentary": 1.2,
	"lightly_active": 1.375,
	"moderately_active": 1.55,
	"very_active": 1.725,
	"super_active": 1.9
}
tdee = bmr * tdee_multipliers[activity]

weight_lower = 18.5 * ((height / 100) ** 2)
weight_upper = 24.9 * ((height / 100) ** 2)

body = f"""
<html>
<head>
<title>Test Form Results</title>
<style>
body {{
font-family: 'Arial', sans-serif;
background-color: #f4f6f9;
color: #333;
margin: 0;
padding: 20px;
line-height: 1.6;
}}
.container {{
max-width: 800px;
margin: 0 auto;
background-color: #fff;
padding: 30px;
box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
border-radius: 8px;
}}
h1 {{
color: #2c3e50;
margin-bottom: 20px;
}}
p {{
margin-bottom: 15px;
}}
.highlight {{
font-weight: bold;
color: #3498db;
}}
</style>
</head>
<body>
<div class="container">
<h1>Hello <span class="highlight">{first_name} {last_name}</span>, here is some info about you:</h1>
<p>Age in years: <span class="highlight">{age}</span></p>
<p>Age in days: <span class="highlight">{age_days}</span></p>
<p>Age in months: <span class="highlight">{age_months}</span></p>
<p>Age in weeks: <span class="highlight">{age_weeks}</span></p>
<p>Your current body mass index (BMI) is: <span class="highlight">{bmi:.2f}</span></p>
<p>Your current basal metabolic rate (BMR) is: <span class="highlight">{bmr:.2f}</span> kcal/day</p>
<p>Your total daily energy expenditure (TDEE) is: <span class="highlight">{tdee:.0f}</span> kcal/day</p>
"""

if weight >= weight_upper:
	suggested_kcal = tdee - 300
	body += f"<div class='overweight'><p>You are currently <span class='highlight'>overweight</span> and should eat less.</p>"
	body += f"<p>We suggest you to eat around <span class=\"highlight\">{suggested_kcal:.0f}</span> kcal/day to gain some weight.</p></div>"
elif weight < weight_lower:
	suggested_kcal = tdee + 300
	body += f"<div class='underweight'><p>You are currently <span class='highlight'>underweight</span> and should eat more.</p></div>"
	body += f"<p>We suggest you to eat around <span class=\"highlight\">{suggested_kcal:.0f}</span> kcal/day to lose some weight.</p></div>"
else:
	body += "<div class='healthy'><p>Your weight is within the ideal range! Keep up the good work!</p></div>"
	body += f"Keep eating around <span class=\"highlight\">{tdee:.0f}</span> kcal/day and you'll be fine!"

body += """
</div>
</body>
</html>
"""

content_length = len(body)

print("HTTP/1.1 200 OK\r")
print("Content-type: text/html\r")
print(f"Content-Length: {content_length}\r\n")
print(body)
