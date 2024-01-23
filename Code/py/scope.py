def f(a,b):
    global c
    return (a + b + c)
c = 6
print(f(1,2))

#mutable
# lists are mutable
# so are objects

class BankAccount:
    balance = 0.0

class BankAccount:
    balance = 0.0
    def __init__(self):
	self.deposits = []


class Inventory:
    item = ""
    barcode = 0
    quantity = 0
    price = 0.00
    sales = 0.00
    def __init__(self, product, bar, pr):
        self.item = product
        self.barcode = bar
        self.price = pr
    def changeprice(self, newprice):
        self.price = newprice
    def sell(self, n):
        self.quantity -= n
        self.sales += self.price*n
    def restock(self, n):
        self.quantity += n

widget = Inventory("widget", 1112223334, 10.00)
widget.restock(30)
widget.sell(10)
print(widget.quantity)
print(widget.sales)
widget.changeprice(20.0)
widget.sell(10)
print(widget.quantity)
print(widget.sales)

#inheritence
class FootballPlayer:
    name = "John Doe"
    team = "None"
class Quarterback(FootballPlayer):
    pass_attempts = 0
    completions = 0
    pass_yards = 0
class RunningBack(FootballPlayer):
    rushes = 0
    rush_yards = 0

class FootballPlayer:
name = "John Doe"
team = "None"
years_in_league = 0
def printPlayer(self):
    print(self.name+" playing for the "+self.team+":")
def isGood(self):
    print("Error! isGood is not defined!")
    return False


class FootballPlayer:
    name = "John Doe"
    team = "None"
    years_in_league = 0
    def printPlayer(self):
	print(self.name+" playing for the "+self.team+":")
    def isGood(self):
	print("Error! isGood is not defined!")
    return False

class Quarterback(FootballPlayer):
    pass_attempts = 0
    completions = 0
    pass_yards = 0
    def completionRate(self):
	return self.completions/self.pass_attempts
    def yardsPerAttempt(self):
	return self.pass_yards/self.pass_attempts
    def isGood(self):
	return (self.yardsPerAttempt() > 7)
class RunningBack(FootballPlayer):
    rushes = 0
    rush_yards = 0
    def yardsPerRush(self):
	return self.rush_yards/self.rushes
    def isGood(self):
	return (self.yardsPerRush() > 4)


player1 = Quarterback()
player1.name = "John"
player1.team = "Cowboys"
player1.pass_attempts = 10
player1.completions = 6
player1.pass_yards = 57
player2 = RunningBack()
player2.name = "Joe"
player2.team = "Eagles"
player2.rushes = 12
player2.rush_yards = 73
playerlist = []
playerlist.append(player1)
playerlist.append(player2)
for player in playerlist:
    player.printPlayer()
    if (player.isGood()):
        print("  is a GOOD player")
    else:
	print("  is NOT a good player")

import numpy
import openssl

#swigging python?

import webbrowser
webrowser.open("http://www.thegreatcourses.com")

import by filename

import pyglet

window = pyglet.window.Window(width=400, height=300, caption="TestWindow")
pyglet.app.run()

from random import choice
    import pyglet

window = pyglet.window.Window(width=400, height = 450, caption="GameWindow")
Im1 = pyglet.image.load('BlueTri.jpg')
Im2 = pyglet.image.load('PurpleStar.jpg')
Im3 = ('OrangeDiamond.jpg')
Im4 = pyglet.image.load('YellowCircle.jpg')
Im5 = pyglet.image.load('RedHex.jpg')
def InitializeGrid(board):
    #Initialize Grid by reading in from file
    for i in range(8):
        for j in range(8):
            board[i][j] = choice(['A', 'B', 'C', 'D', 'E'])
def Initialize(board):
    #Initialize game
    #Initialize grid
    InitializeGrid(board)
    #Initialize score
    global score
    score = 0
    #Initialize turn number
    global turn
    turn = 1
    #Set up graphical info
def ContinueGame(current_score, goal_score = 100):
    #Return false if game should end, true if game is not over
    if (current_score >= goal_score):
        return False
    else:
    return True

def SwapPieces(board, move):
        #Swap objects in two positions
        temp = board[move[0]][move[1]]
        board[move[0]][move[1]] = board[move[2]][move[3]]
        board[move[2]][move[3]] = temp

def RemovePieces(board):
    #Remove 3-in-a-row and 3-in-a-column pieces
    #Create board to store remove-or-not
    remove = [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0], [0, 0,
      0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0,
      0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]
    #Go through rows
    for i in range(8):
        for j inpyglet.image.load range(6):
            if (board[i][j] == board[i][j+1]) and (board[i][j] == board[i][j+2]):
                #three in a row are the same!
                remove[i][j] = 1;
                remove[i][j+1] = 1;
                    remove[i][j+2] = 1;
            #Go through columns
            for j in range(8):
                for i in range(6):
                    if (board[i][j] == board[i+1][j]) and (board[i][j] == board[i+2][j]):
                        #three in a row are the same!
                        remove[i][j] = 1;
                        remove[i+1][j] = 1;
                        remove[i+2][j] = 1;
            #Eliminate those marked
            global score
            removed_any = False
   
    for i in range(8):
        for j in range(8):
            if remove[i][j] == 1:
                board[i][j] = 0
                score += 1
                removed_any = True
    return removed_any
def DropPieces(board):
    #Drop pieces to fill in blanks
    for j in range(8):
        #make list of pieces in the column
        listofpieces = []
        for i in range(8):
            if board[i][j] != 0:
                listofpieces.append(board[i][j])
        #copy that list into colulmn
        for i in range(len(listofpieces)):
            board[i][j] = listofpieces[i]
        #fill in remainder of column with 0s
        for i in range(len(listofpieces), 8):
            board[i][j] = 0

def FillBlanks(board):
    #Fill blanks with random pieces
    for i in range(8):
        for j in range(8):
            if (board[i][j] == 0):
                board[i][j] = choice(['A', 'B', 'C', 'D', 'E'])

def Update(board, move):
    #Update the board according to move
    SwapPieces(board, move)
    pieces_eliminated = True
    while pieces_eliminated:
        pieces_eliminated = RemovePieces(board)
        DropPieces(board)
        FillBlanks(board)
        @window.event
        def on_draw():
            window.clear()
            for i in range(7,-1,-1):
                #Draw each row
                y = 50+50*i
                for j in range(8):
                    #draw each piece, first getting position
                    x = 50*j
                    if board[i][j] == 'A':
                        Im1.blit(x,y)
                    elif board[i][j] == 'B':
                        Im2.blit(x,y)
                    elif board[i][j] == 'C':
                        Im3.blit(x,y)
                    elif board[i][j] == 'D':
                        Im4.blit(x,y)
                    elif board[i][j] == 'E':
              Im5.blit(x,y)
label = pyglet.text.Label('Turn: '+str(turn)+' Score:
'+str(score), font_name='Arial', font_size=18, x=20, y = 10)
    label.draw()
@window.event
def on_mouse_press(x, y, button, modifiers):
    #Get the starting cell
    global startx
    global starty
    startx = x
    starty = y
@window.event
def on_mouse_release(x, y, button, modifiers):
    #Get starting and ending cell and see if they are adjacent
    startcol = startx//50
    startrow = (starty-50)//50
    endcol = x//50
    endrow = (y-50)//50
         #Check whether ending is adjacent to starting and if so,
          make move.
        if ((startcol==endcol and startrow==endrow - 1)
          or (startcol==endcol and startrow==endrow+1) or
          (startrow==endrow and startcol==endcol-1) or
          (startrow==endrow and startcol==endcol+1)):
            Update(board,[startrow,startcol,endrow,endcol])
            global turn
            turn += 1
            #See if game is over
            if not ContinueGame(score):
                print("You won in", turn, "turns!")
                exit()
    #State main variables
    score = 100
    turn = 100
    goalscore = 100
    board = [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0],
             [0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0],
             [0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0],
             [0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]
    #Initialize game
    Initialize(board)
    pyglet.app.run()

from matplotlib.pyplot import plot, show
#Set initial conditions
time = 0
balance = 1000
#Set list to store data
timelist=[time]
balancelist=[balance]
while (time < 10):
    #Increase balance and time
    balance += balance*0.03
    time += 1
    #Store time and balance in lists
    timelist.append(time)
    balancelist.append(balance)
#Output the simulation results
    for i in range(len(timelist)):
        print("Year:", timelist[i], "  Balance:", balancelist[i])
plot(timelist, balancelist)
show()

