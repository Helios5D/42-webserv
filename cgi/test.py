import cgi, cgitb
from datetime import date

form = cgi.FieldStorage()

first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
gender = form.getvalue('gender')
birthdate = form.getvalue('birthday')
height = form.getvalue('height')
weight = form.getvalue('weight')
activity = form.getvalue('activity')

today = date.today()
age = today.year - birthdate.year - ((today.month, today.day) < (birthdate.month, birthdate.day))
age_days = (today - birthdate).days
age_weeks = age_days // 7
age_months = (today.year - birthdate.year) * 12 + today.month - birthdate.month
bmi = weight / ((height / 100)**2)
if (gender == "male"):
	bmr = 88.362 + (13.397 * weight) + (4.799 * height) - (5.677 * age)
else:
	bmr = 447.593 + (9.247 * weight) + (3.098 * height) - (4.330 * age)
if (activity == "sedentary"):
	tdee_multiplier = 1.2
elif (activity == "lightly_active"):
	tdee_multiplier = 1.375
elif (activity == "moderately_active"):
	tdee_multiplier = 1.55
elif (activity == "very_active"):
	tdee_multiplier = 1.725
else:
	tdee_multiplier = 1.9
tdee = bmr * tdee_multiplier

weight_upper = 18.5 * ((height / 100)**2)
weight_lower = 24.9 * ((height / 100)**2)

print ("Content-type:text/html")
print()
print ("<html>")
print ('<head>')
print ("<title>Test Form</title>")
print ('</head>')
print ('<body>')
print ("<h1>Hello %s %s, here is some info about you</h1>" % (first_name, last_name))
print("<p>Calculating your age in years would be too easy")
print("Age in years: %d<br> Age in days: %d<br>Age in months: %d<br>Age in weeks: %d</p>" % (age, age_days, age_months, age_weeks))
print("<p>Your current body mass index is %d</p>" % bmi)
print("<p>Your current basal meteabolic rate is %d</p>" % bmr)
print("<p>Your daily caloric needs is estimated at %dkcal / day</p>" % tdee)
if (weight >= weight_upper):
	print("<p>You are currently overweight and should eat less.</p>");
elif (weight < weight_lower):
	print("<p>You are currently underweight and should eat more.</p>");
else:
	print("<p>You weight is within the ideal range!.</p>");
print ('</body>')
print ('</html>')
