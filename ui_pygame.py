from bleak import BleakScanner, BleakClient;
import sys, asyncio
import pygame
import Tractor


class Button():
    def __init__(self, surface=None, x=None, y=None, width=50, height=50, color = (128,128,128)):
        self.surface = surface
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.color = color
        self.text = None

    def setText(self, text):

        self.text = font.render(text, True, (0,0,0))
        
    def draw(self, surface=None, x=None, y=None):
        if(surface == None):
            surface = self.surface
        if(x == None):
            x = self.x
        if(y == None):
            y = self.y

        pygame.draw.rect(surface, self.color, (x,y,self.width,self.height))
        surface.blit(self.text, (x+50,y+40,150,100))

    def bind(self, function):
        
        self.run = function

    def inside(self, x, y):
        return x > self.x  and  x < self.x+self.width and y > self.y and y < self.y+self.height


async def main():
    tractor = Tractor.Tractor()

    await tractor.connect()

    await tractor.setRecv()

    pygame.init()
    screen = pygame.display.set_mode((600,600))

    pygame.display.set_caption("IDP Group 7 Pygame Tractor GUI")

    global font
    font = pygame.font.Font('freesansbold.ttf', 32)

    startButton = Button(screen, 50,50,500,100)
    stopButton = Button(screen, 50,200,500,100)
    reportButton = Button(screen, 50,350,500,100)


    startButton.bind(lambda: t.send(b'Go'))
    stopButton.bind(lambda: t.send(b'Stop'))

    startButton.setText("Start Tractor")
    stopButton.setText("Stop Tractor")
    reportButton.setText("Generate Report")

    running = True
    while running:

        screen.fill((255,255,255))

        startButton.draw()
        stopButton.draw()
        reportButton.draw()

        pygame.display.update()
        
        for event in pygame.event.get():
            
            if event.type == pygame.QUIT:
                running = False;
                await tractor.quit()
                pygame.quit()
                
            if event.type == pygame.MOUSEBUTTONUP:
                pos = pygame.mouse.get_pos()

                if startButton.inside(pos[0], pos[1]):
                    await startButton.run()
                    
                if stopButton.inside(pos[0], pos[1]):
                    await stopButton.run()
                    
                if reportButton.inside(pos[0], pos[1]):
                    await reportButton.run()



        await asyncio.sleep(0.1)

asyncio.run(main())
    
