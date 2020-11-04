total_cost = 100.00 days = 3
        cost_per_day = total_cost / days
        if cost_per_day > 40:
            print("Too expensive")
        elif cost_per_day > 30:
            print("Reasonable cost")
        elif cost_per_day > 20:
            print("Excellent cost")
        else:
            print("Incredible bargain")
#Get information from user
balance = input("Enter how much you want to save: ")
payment = input("Enter how much you will save each period: ")
#Calculate number of payments that will be needed
#Present information to user
#Get information from user
balance = float(input("Enter how much you want to save: "))

while value <= 0:
    value = int(input ("Enter a Positive value!"))

myfile = open("Filename", "r")

myfile = open("Filename", "w")
myfile.write("This line is written to the file.")
myfile.close()

# lists
print (daily_high_temps[4])

for i in range(7):
    print(daily_high_temps[i])
daily_high_temps = [83, 80, 73, 75, 79, 83, 86]

x = len(daily_high_temps)
list1 += list2
list1.append(3.9)

varname[a:b]
car_tuple = "Buick", "Century", 23498

########## Analyze Data ##########
#Get date of interest
month = int(input("For the date you care about, enter the month: "))
day = int(input("For the date you care about, enter the day: "))
#Find historical data for date
gooddata = []
for singleday in datalist:
    if (singleday[0] == day) and (singleday[1] == month):
	gooddata.append([singleday[2], singleday[3],
              singleday[4], singleday[5]])
    #Perform analysis
       minsofar = 120
       maxsofar = -100
       numgooddates = 0
       sumofmin=0
       sumofmax=0
       for singleday in gooddata:
           numgooddates += 1
           sumofmin += singleday[1]
           sumofmax += singleday[2]
           if singleday[1] < minsofar:
               print(minsofar, singleday[1])
               minsofar = singleday[1]
           if singleday[2] > maxsofar:
               maxsofar = singleday[2]
       avglow = sumofmin / numgooddates
       avghigh = sumofmax / numgooddates

def getName():
    first = input("Enter your first name:")
    last = input("Enter your last name:")
    full_name = first + ' ' + last
    return full_name
name = getName()
