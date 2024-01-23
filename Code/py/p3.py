def calc_miles(gallons, mpg=20.0):
           return gallons*mpg
       print( calc_miles(10.0, 15.0) )
       print( calc_miles(10.0) )

 for line in infile:
        #STUFF DELETED HERE
        m, d, y = date.split('/')
        month = int(m)
        day = int(d)
        year = int(y)
        #Put data into list
        datalist.append([day, month, year, lowtemp, hightemp,
          rainfall])
    #STUFF DELETED HERE
    #Find historical data for date
    gooddata = []
    for singleday in datalist:
        if (singleday[0] == month) and (singleday[1] == day):
            gooddata.append([singleday[2], singleday[3],
              singleday[4], singleday[5]])

from random import choice
    import pyglet
    window = pyglet.window.Window(width=400, height = 450,
      caption="GameWindow")
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
                    if (board[i][j] == board[i][j+1]) and (board[i][j] ==
                      board[i][j+2]):
                        #three in a row are the same!
                        remove[i][j] = 1;
                        remove[i][j+1] = 1;
                        remove[i][j+2] = 1;
            #Go through columns
            for j in range(8):
                for i in range(6):
                    if (board[i][j] == board[i+1][j]) and (board[i][j] ==
                      board[i+2][j]):
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
    label = pyglet.text.Label('Turn: '+str(turn)+
     'Score: '+str(score), font_name='Arial', font_size=18, x=20,
      y = 10)
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

from matplotlib import pyplot
    pyplot.plot([0,1,2,3,4,5], [0,1,4,9,16,25])
    pyplot.axis([0,5,0,25])
    pyplot.show()
    
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

 class BankAccount:
            balance = 0.0
            def __init__(self):
                self.deposits = []
        checking_account = BankAccount()
        savings_account = BankAccount()
        checking_account.deposits.append(100.0)
        print(savings_account.deposits)

class FootballPlayer:
            name = "John Doe"
            team = "None"
            years_in_league = 0
            def printPlayer(self):
                print(self.name+" playing for the "+self.team+":")
        class Quarterback(FootballPlayer):
            pass_attempts = 0
            completions = 0
            pass_yards = 0
      
         def completionRate(self):
            return self.completions/self.pass_attempts
         def yardsPerAttempt(self):
            return self.pass_yards/self.pass_attempts
        class RunningBack(FootballPlayer):
            rushes = 0
            rush_yards = 0
        def yardsPerRush(self):
            return self.rush_yards/self.rushes

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

    book_queue = []
    book_queue.append(medium_book)
    book_queue.append(short_book)
    book_queue.append(long_book)
    next_book = book_queue.pop(0)

def isIn(L, v): i= 0
        while (i<len(L)):
            if L[i] == v:
                return True
            else:
                i += 1
        return False
    favorite_foods = ['pizza', 'barbeque', 'gumbo', 'chicken and
      dumplings', 'pecan pie', 'ice cream']
    print(isIn(favorite_foods, 'gumbo'))
    print(isIn(favorite_foods, 'coconut'))

    OUTPUT:
    True
    False

 def mergeSort(L):
        n = len(L)
        if n <= 1:
            return
        L1 = L[:n//2]
        L2 = L[n//2:]
        mergeSort(L1)
        mergeSort(L2)
        merge(L, L1, L2)
        return

def merge(L, L1, L2):
    i= 0
    j= 0
    k= 0
    while (j < len(L1)) or (k < len(L2)):
        if j < len(L1):
            if k < len(L2):
                #we are not at the end of L1 or L2, so pull the smaller value
                if L1[j] < L2[k]:
                    L[i] = L1[j]
                    j += 1
                else:
                    L[i] = L2[k]
                k += 1
            else:
                #we are at the end of L2, so just pull from L1
                L[i] = L1[j]
                j += 1
         else:
                #we are at the end of L1, so just pull from L2
                L[i] = L2[k]
                k += 1
                i +=  1
    return 

def merge(L, L1, L2):
    i= 0
    j= 0
    k= 0
    while (j < len(L1)) or (k < len(L2)):
        if j < len(L1):
            if k < len(L2):
                #we are not at the end of L1 or L2, so pull the
                  smaller value
                if L1[j] < L2[k]:
                    L[i] = L1[j]
                    j += 1
                else:
                    L[i] = L2[k]
                    k += 1
            else:
                #we are at the end of L2, so just pull from L1
                L[i] = L1[j]
                j += 1
         else:
             #we are at the end of L1, so just pull from L2
             L[i] = L2[k]
             k += 1
             i += 1
    return

class node:
        def __init__(self, name, parent=-1):
            self._name = name
            self._parent = parent
            self._left = -1
            self._right = -1
        def getName(self):
            return self._name
        def getParent(self):
            return self._parent
        def getLeft(self):
            return self._left
        def getRight(self):
            return self._right
        def setParent(self, p):
            self._parent = p
        def setLeft(self, l):
            self._left = l
        def setRight(self, r):
            self._right = r

class node:
        def __init__(self, name):
            self._name = name
            self._friends = []
            self._status = 0
            self._discoveredby = 0
        def getName(self):
            return self._name
        def getFriends(self):
            return self._friends
        def addFriend(self, friend_index):
            self._friends.append(friend_index)
        def isUnseen(self):
            if self._status == 0:
                return True
            else:
                return False
        def isSeen(self):
            if self._status == 1:
                return True
            else:
                    return False
            def setUnseen(self):
                self._status = 0
            def setSeen(self):
                self._status = 1
            def discover(self, n):
                self._discoveredby = n
            def discovered(self):
                return self._discoveredby

 def BFS(nodelist, start, goal):
            to_visit = queue()
            nodelist[start].setSeen()
            to_visit.enqueue(start)
            found = False
            while (not found) and (not to_visit.isEmpty()):
                current = to_visit.dequeue()
                neighbors = nodelist[current].getNeighbors()
                for neighbor in neighbors:
                    if nodelist[neighbor].isUnseen():
                        nodelist[neighbor].setSeen()
                        nodelist[neighbor].discover(current)
                        if neighbor == goal:
                            found = True
                        else:
                            to_visit.enqueue(neighbor)

def retrievePath(nodelist, start, goal):
            #Return the path from start to goal
        def BFS(nodelist, start, goal):
            to_visit = queue()
            nodelist[start].setSeen()
            to_visit.enqueue(start)
            found = False
        
         while (not found) and (not to_visit.isEmpty()):
            current = to_visit.dequeue()
            neighbors = nodelist[current].getNeighbors()
            for neighbor in neighbors:
                if nodelist[neighbor].isUnseen():
                    nodelist[neighbor].setSeen()
                    nodelist[neighbor].discover(current)
                    if neighbor == goal:
                        found = True
                    else:
                        to_visit.enqueue(neighbor)
        return retrievePath(noswliar, start, goal)

from multiprocessing import Process
    def print_process(number):
        print("Printing from process", number)
    if __name__ == '__main__':
        process_list = []
        for i in range(20):
            p = Process(target=print_process, args=(i,))
            process_list.append(p)
        for p in process_list:
            p.start()

for line in infile:
        #STUFF DELETED HERE
        m, d, y = date.split('/')
        month = int(m)
        day = int(d)
        year = int(y)
        #Put data into list
        datalist.append([day, month, year, lowtemp, hightemp,
          rainfall])
    #STUFF DELETED HERE
    #Find historical data for date
    gooddata = []
    for singleday in datalist:
        if (singleday[0] == month) and (singleday[1] == day):
            gooddata.append([singleday[2], singleday[3],
              singleday[4], singleday[5]])

 try:
        #Commands to try out
    except <name of exception>:
        #how to handle that exception

import math
    print(math.sin(math.pi/2))

import webbrowser
    webbrowser.open("http://www.thegreatcourses.com")

import shutil
        shutil.copy("File1.txt", "File2.txt")


