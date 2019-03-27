import json, subprocess

import os, sys

dir_path = os.path.dirname(os.path.realpath(__file__))+'/BackendBinary/oxidising-ocelots-backend'


try:
      print ("Path to binary has been specified. Will use", sys.argv[1])
      dir_path = sys.argv[1]
except:
      print ("Path to binary has not been specified. Will use", dir_path)


def connect(binaryPath = (dir_path)):
      proc = subprocess.Popen(binaryPath,
                        stdout = subprocess.PIPE, stdin = subprocess.PIPE)
      return proc

class requestFailed(Exception):
      pass

def convert_list_to_int(strList):
            for i in strList:
                  strList.remove(i)
                  strList.append(int(i))
            return strList
      
def convert_list_to_string(intList):
            padding = 10
            for i in intList:
                  intList.remove(i)
                  intList.append(int(i))
            return intList

class gameBackend:
                  

      def do_request(self, str):
            self.proc.stdin.write(str.encode('utf-8'))
            self.proc.stdin.write(b'\n')
            self.proc.stdin.flush()
            return self.proc.stdout.readline().decode('utf-8')

      def request(self, str):
            print (str+'\n\n')
            result = self.do_request(str)
            print (result)
            result = json.loads(result)
            if not result['succeeded']:
                  raise requestFailed(result['result'])
            return result['result']
            
      def __init__(self, players):
            players = convert_list_to_int(players)
            self.proc = connect()
            a = self.request(('{{"action":"init", "players":{players}}}').format(players = str(players)))

      def listCards(self):
            return (self.request('{"action":"cards"}'))

      def getCardName(self, cID): 
            return (self.request(('{{"action":"card_name", "card":{cID}}}').format(cID = cID)))
      
      def dumpState(self):
            return (self.request('{"action":"dump"}'))

      def finishGo(self):
            return (self.request('{"action":"finish"}'))

      def killPlayer(self, player):
            return (self.request(('{{"action":"kill", "player": {player}}}').format(player = player)))
      
      def playCard(self, card, player=None):
            if player:
                  return (self.request(('{{"action":"play", "card": {card}, "player": {player}').format(card = card, player = player)))
            else:
                  return (self.request(('{{"action":"play", "card": {card}}}').format(card = card)))

      def getPlayerHands(self):
            pListHand = {}
            players = (self.request('{"action":"dump"}'))['players']
            for player in players:
                  (players[player])['hand']
                  pList
                  
            return (self.request('{"action":"dump"}'))['players']
                  
            


      

if __name__ == '__main__':
      class1 = gameBackend([1, 2])
      class2 = gameBackend([3, 4])
      a = int(input ("1:  initialise the game\n2:  list all cards\n3:  dump state\n4:  get a card name\n5:  finish current go\n6:  kill a player\n7:  play a card\n"))
      while True:

            if a == 1:
                  try:
                        nameList = []
                        while True:
                              nameList.append(int(input("add a number")))
                  except:
                        print (nameList)
                        class1 = gameBackend(nameList)
            elif a == 2:
                  cards = (class1.listCards())
                  for i in cards:
                        print (i, cards[i]['id'])
            elif a == 3:
                  print (class1.dumpState())
            elif a == 4:
                  print(class1.getCardName(int(input("enter an ID"))))
            elif a == 5:
                  print(class1.finishGo())
            elif a == 6:
                  print(class1.killPlayer(int(input("enter player id"))))
            elif a == 7:
                  print(class1.playCard(int(input("enter card id"))))
            elif a == 8:
                  print(class1.getPlayerHands())
            a = int(input('number: '))
      

            
