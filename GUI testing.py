from bleak import BleakScanner, BleakClient
import sys, asyncio
import pygame
import Tractor

t = Tractor.Tractor()

t.connect()

for service in t.bluetoothClient.services:
    print('\nservice', service.handle, service.uuid, service.description)

    characteristics = service.characteristics

    for char in characteristics:
        print('  characteristic', char.handle, char.uuid, char.description, char.properties)

        descriptors = char.descriptors

        for desc in descriptors:
            print('    descriptor', desc)

class Button():
    def __init__(self, surface=None, x=None, y=None, width=50, height=50, color=(128, 128, 128)):
        self.surface = surface
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.color = color
        self.text = None

    def setText(self, text):

        self.text = font.render(text, True, (0, 0, 0))

    def draw(self, surface=None, x=None, y=None):
        if (surface == None):
            surface = self.surface
        if (x == None):
            x = self.x
        if (y == None):
            y = self.y

        pygame.draw.rect(surface, self.color, (x, y, self.width, self.height))
        surface.blit(self.text, (x + 50, y + 40, 150, 100))

    def bind(self, function):

        self.func = function

    def inside(self, x, y):
        return x > self.x and x < self.x + self.width and y > self.y and y < self.y + self.height

    def run(self):

        self.func()


pygame.init()
display_width = 800
display_length = 600
screen = pygame.display.set_mode((display_width, display_length))

pygame.display.set_caption("IDP Group 7 Pygame Tractor GUI")

font = pygame.font.Font('freesansbold.ttf', 32)

startButton = Button(screen, 50, 50, 500, 100)
stopButton = Button(screen, 50, 200, 500, 100)
reportButton = Button(screen, 50, 350, 500, 100)

startButton.bind(lambda: t.send(b'Go'))
stopButton.bind(lambda: t.send(b'Stop'))

startButton.setText("Start Tractor")
stopButton.setText("Stop Tractor")
reportButton.setText("Generate Report")

#tractorImage = pygame.image.load('tractorJPEG')

x = (display_width * .9)
y = (display_length * .1)


#def tractor(x, y):
#    screen.blit(tractorImage, (x, y))

#def move_image(x, y):
#    screen.blit(image, (x, y))

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()
        if event.type == pygame.MOUSEBUTTONUP:
            pos = pygame.mouse.get_pos()

            if startButton.inside(pos[0], pos[1]):
                startButton.run()

            if stopButton.inside(pos[0], pos[1]):
                stopButton.run()

            if reportButton.inside(pos[0], pos[1]):
                reportButton.run()

    screen.fill((255, 255, 255))
    startButton.draw()
    stopButton.draw()
    reportButton.draw()
#   move_image(t.getpos('x'), t.getpos('y'))
    pygame.display.update()
