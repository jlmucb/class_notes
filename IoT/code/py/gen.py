#!/usr/bin/python
print "Hello, Python!";

if True:
 print "True"
else:
 print "False"

#!/usr/bin/python
import sys
try:
 # open file stream
 file = open(file_name, "w")
except IOError:
 print "There was an error writing to", file_name
 sys.exit()
print "Enter '", file_finish,
print "' When finished"
while file_text != file_finish:
 file_text = raw_input("Enter text: ")
 if file_text == file_finish:
 # close the file
 file.close
 break
 file.write(file_text)
 file.write("\n")
file.close()


#!/usr/bin/python
import sys, getopt
def main(argv):
 inputfile = ''
 outputfile = ''
 try:
 opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
 except getopt.GetoptError:
 print 'test.py -i <inputfile> -o <outputfile>'
 sys.exit(2)
 for opt, arg in opts:
 if opt == '-h':
 print 'test.py -i <inputfile> -o <outputfile>'
 sys.exit()
 elif opt in ("-i", "--ifile"):
 inputfile = arg
 elif opt in ("-o", "--ofile"):
 outputfile = arg

#!/usr/bin/python
tuple = ( 'abcd', 786 , 2.23, 'john', 70.2 )
list = [ 'abcd', 786 , 2.23, 'john', 70.2 ]
tuple[2] = 1000 # Invalid syntax with tuple
list[2] = 1000 # Valid syntax with list


if ( a > b ):
 print "Line 5 - a is greater than b"
else:
 print "Line 5 - a is not greater than b"


#!/usr/bin/python
a = 10
b = 20
list = [1, 2, 3, 4, 5 ];TUTORIALS POINT
Simply	Easy	Learning
if ( a in list ):
 print "Line 1 - a is available in the given list"
else:
 print "Line 1 - a is not available in the given list"
if ( b not in list ):
 print "Line 2 - b is not available in the given list"
else:
 print "Line 2 - b is available in the given list"
a = 2
if ( a in list ):
 print "Line 3 - a is available in the given list"
else:
 print "Line 3 - a is not available in the given list"


#!/usr/bin/python
count = 0
while (count < 9):
 print 'The count is:', count
 count = count + 1
print "Good bye!"

#!/usr/bin/python
for letter in 'Python': # First Example
 print 'Current Letter :', letter
fruits = ['banana', 'apple', 'mango']
for fruit in fruits: # Second Example
 print 'Current fruit :', fruit
print "Good bye!"

#!/usr/bin/python
fruits = ['banana', 'apple', 'mango']
for index in range(len(fruits)):
 print 'Current fruit :', fruits[index]
print "Good bye!"

#!/usr/bin/python
for letter in 'Python': # First Example
 if letter == 'h':
 continue
 print 'Current Letter :', letter
var = 10 # Second Example
while var > 0:
 var = var -1
 if var == 5:
 continue
 print 'Current variable value :', var
print “Good bye!”

#!/usr/bin/python
import random
random.seed( 10 )
print "Random number with seed 10 : ", random.random()
# It will generate same random number
random.seed( 10 )
print "Random number with seed 10 : ", random.random()
# It will generate same random number
random.seed( 10 )
print "Random number with seed 10 : ", random.random()

var1 = 'Hello World!'
var2 = "Python Programming"
print "var1[0]: ", var1[0]
print "var2[1:5]: ", var2[1:5]

#!/usr/bin/python
dict = {'Name': 'Zara', 'Age': 7, 'Class': 'First'};
print "dict['Name']: ", dict['Name'];
print "dict['Age']: ", dict['Age'];

#!/usr/bin/python
import time; # This is required to include time module.
ticks = time.time()
print "Number of ticks since 12:00am, January 1, 1970:", ticks

def functionname( parameters ):
 "function_docstring"
 function_suite
 return [expression]

All parameters (arguments) in the Python language are passed by reference. It means if you change what a
parameter refers to within a function, the change also reflects back in the calling function. For example:
#!/usr/bin/python
# Function definition is here
def changeme( mylist ):
 "This changes a passed list into this function"
 mylist.append([1,2,3,4]);
 print "Values inside the function: ", mylist
 return
# Now you can call changeme function
mylist = [10,20,30];
changeme( mylist );
print "Values outside the function: ", mylist

#!/usr/bin/python
# Function definition is here
sum = lambda arg1, arg2: arg1 + arg2;
# Now you can call sum as a function
print "Value of total : ", sum( 10, 20 )
print "Value of total : ", sum( 20, 20 )

The Python code for a module named aname normally resides in a file named aname.py. Here's an example of a
simple module, hello.py
def print_func( par ):
 print "Hello : ", par
 return
The import Statement:
You can use any Python source file as a module by executing an import statement in some other Python source
file. The import has the following syntax:
import module1[, module2[,... moduleN]
When the interpreter encounters an import statement, it imports the module if the module is present in the search
path. A search path is a list of directories that the interpreter searches before importing a module. For example, to
import the module hello.py, you need to put the following command at the top of the script:
#!/usr/bin/python
# Import module hello
import hello
# Now you can call defined function that module as follows
hello.print_func("Zara")
When the above code is executed, it produces the following result:
Hello : Zara

#!/usr/bin/python
Money = 2000
def AddMoney():
 # Uncomment the following line to fix the code:
 # global Money
 Money = Money + 1
print Money
AddMoney()
print Money


#!/usr/bin/python
str = raw_input("Enter your input: ");
print "Received input is : ", str

#!/usr/bin/python
# Open a file
fo = open("foo.txt", "r+")
str = fo.read(10);
print "Read String is : ", str
# Close opened file
fo.close()

input(prompt) 
len(thing) 
open(filename, mode) 
print(line) 
type(thing) 
ord(char)
chr(number) Not that many!
bool(thing) 
float(thing)
int(thing)
iter(list)
list(thing)
range(from, to, stride) 
str(thing)

