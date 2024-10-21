import cgi
from datetime import datetime, date

cgi.enable()

form = cgi.FieldStorage()

first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
gender = form.getvalue('gender')

birthdate_str = form.getvalue('birthday')
birthdate = datetime.strptime(birthdate_str, '%Y-%m-%d').date()

height = float(form.getvalue('height'))
weight = float(form.getvalue('weight'))
activity = form.getvalue('activity')

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

print("Content-type:text/html")
print()
print("<html>")
print("<head>")
print("<title>Test Form Results</title>")
print("</head>")
print("<body>")
print(f"<h1>Hello {first_name} {last_name}, here is some info about you:</h1>")
print(f"<p>Age in years: {age}<br>Age in days: {age_days}<br>Age in months: {age_months}<br>Age in weeks: {age_weeks}</p>")
print(f"<p>Your current body mass index (BMI) is: {bmi:.2f}</p>")
print(f"<p>Your current basal metabolic rate (BMR) is: {bmr:.2f} kcal/day</p>")
print(f"<p>Your total daily energy expenditure (TDEE) is: {tdee:.0f} kcal/day</p>")
if weight >= weight_upper:
	print("<p>You are currently overweight and should eat less.</p>")
elif weight < weight_lower:
	print("<p>You are currently underweight and should eat more.</p>")
else:
	print("<p>Your weight is within the ideal range!</p>")
print("</body>")
print("</html>")
