from bleak import BleakScanner, BleakClient
import sys, asyncio
import pygame
import Tractor


def drawTrace(surface, x, y, prevX, prevY):
    rec = pygame.Rect(surface, x - prevX, y - prevY)

    pygame.draw.rect(surface, (0, 0, 0), rec)


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

        self.run = function

    def inside(self, x, y):
        return x > self.x and x < self.x + self.width and y > self.y and y < self.y + self.height


async def main():
    t = Tractor.Tractor()

    await t.connect()

    await t.setRecv()

    pygame.init()
    screen = pygame.display.set_mode((600, 600))

    pygame.display.set_caption("IDP Group 7 Pygame Tractor GUI - Main Menu")

    global font
    font = pygame.font.Font('freesansbold.ttf', 32)

    startButton = Button(screen, 50, 50, 500, 100)
    stopButton = Button(screen, 50, 200, 500, 100)
    reportButton = Button(screen, 50, 350, 500, 100)

    startButton.bind(lambda: t.send(b'Go'))
    stopButton.bind(lambda: t.send(b'Stop'))

    startButton.setText("Start Tractor")
    stopButton.setText("Stop Tractor")
    reportButton.setText("Generate Report")

    tractorImage = pygame.image.load('tractorJPEG.jpg').convert()
    tractorImage = pygame.transform.scale(tractorImage, (25, 25))

    running = True
    reportMenu = False
    pygame.display.flip() #run first frame because of inconsistent bug
    while running:
        screen.fill((255, 255, 255))

        if not reportMenu:
            startButton.draw()
            stopButton.draw()
            reportButton.draw()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                pygame.quit()
            if event.type == pygame.MOUSEBUTTONUP and not reportMenu:
                pos = pygame.mouse.get_pos()

                if startButton.inside(pos[0], pos[1]):
                    await startButton.run()

                if stopButton.inside(pos[0], pos[1]):
                    await stopButton.run()

                if reportButton.inside(pos[0], pos[1]):
                    await reportButton.run()
                    reportMenu = True
                    pygame.display.set_caption("IDP Group 7 Pygame Tractor GUI - Trip Report")

            if reportMenu:
                screen.blit(tractorImage, (t.PosX, t.PosY))
                if t.AngleZ != t.idealAngleZ:
                    tractorImage = pygame.transform.rotate(tractorImage, t.AngleZ)

                #draw behind if changed pos
                if t.PosX != t.prevPosX:
                    drawTrace(screen, t.PosX, t.PosY, t.prevPosX, t.prevPosY)

        pygame.display.update()

        await asyncio.sleep(0.1)


asyncio.run(main())

